// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: 04_shared_weak_ptr_demo — 共享所有权与弱引用                     ║
// ║  目标: shared_ptr、make_shared、引用计数、weak_ptr、循环引用、                     ║
// ║        enable_shared_from_this                                                ║
// ║  前置: 理解 unique_ptr、裸指针、构造函数与析构函数                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <memory>       // shared_ptr, weak_ptr, make_shared, enable_shared_from_this
#include <string>
#include <vector>
#include <cassert>
#include "shared/lesson_utils.hpp"
#include "memory/shared_weak_ptr.hpp"

int main() {
    lesson::print_header("内存管理 (4): shared_ptr 与 weak_ptr");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: shared_ptr 基础 — 共享所有权 + 引用计数
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: shared_ptr<T> 允许多个指针共享同一个对象，最后一个 shared_ptr
    //         销毁时自动 delete 对象
    // 为什么: 有些场景无法用单一所有者设计 (如多线程共享缓存、图的数据结构)
    // 何时用: 需要共享所有权时; 否则优先用 unique_ptr (更轻量)
    {
        lesson::print_subtitle("Part 1: shared_ptr 基础与引用计数");

        // make_shared 一次分配同时创建对象和控制块 (效率高)
        auto sp1 = std::make_shared<Named>("Alice");
        sp1->greet();
        std::println("  引用计数 (sp1): {}", sp1.use_count());  // 1

        {
            std::shared_ptr<Named> sp2 = sp1;  // 复制 → 引用计数 +1
            sp2->greet();
            std::println("  引用计数 (sp1 和 sp2 共享): {}", sp1.use_count());  // 2

            std::shared_ptr<Named> sp3 = sp1;
            std::println("  引用计数 (sp1, sp2, sp3 共享): {}", sp1.use_count());  // 3
        } // sp2, sp3 离开作用域 → 引用计数 -2

        std::println("  离开内层作用域后，引用计数: {}", sp1.use_count());  // 1
        // sp1 离开作用域 → 引用计数 → 0 → 自动 delete
    }
    std::println("(离开作用域, Alice 被自动释放)\n");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: make_shared vs new shared_ptr — 性能差异
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: make_shared 把对象和控制块分配在连续内存中，只需一次堆分配
    // 为什么: shared_ptr<T>(new T) 需要两次分配(对象+控制块)，效率低且非异常安全
    // 何时用: 总是优先用 make_shared，除非需要自定义删除器
    {
        lesson::print_subtitle("Part 2: make_shared 的优势");

        std::println("  shared_ptr<T>(new T):  2 次堆分配 (对象 + 控制块)");
        std::println("  make_shared<T>(args):   1 次堆分配 (对象和控制块连续)");
        std::println("  → make_shared 更快、缓存友好、异常安全");
        std::println("  → 唯一缺点: 对象内存和控制块一起释放");
        std::println("    (当 weak_ptr 存在时，对象内存无法提前回收)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: weak_ptr — 不增加引用计数的观察者
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: weak_ptr<T> 观察 shared_ptr 管理的对象，但不拥有它
    // 为什么: 打破循环引用、实现缓存(过期自动清除)、观察者模式
    // 何时用: 循环引用场景、需要检查对象是否还存活、临时借用
    {
        lesson::print_subtitle("Part 3: weak_ptr — 弱引用观察者");

        std::weak_ptr<Named> weak;
        {
            auto sp = std::make_shared<Named>("Bob");
            weak = sp;  // weak_ptr 不增加引用计数
            std::println("  作用域内: use_count = {}", sp.use_count());  // 1 (不是 2!)

            // 使用 weak_ptr 前必须先 lock() — 获取临时的 shared_ptr
            if (auto locked = weak.lock()) {
                locked->greet();
            }
        } // sp 析构 → Bob 被释放, weak 变为 "expired"

        std::println("  离开作用域后:");
        std::println("    weak.expired() = {}", weak.expired());  // true
        // lock() 返回空的 shared_ptr
        if (auto locked = weak.lock()) {
            locked->greet();  // 不会执行
        } else {
            std::println("    对象已被销毁, lock() 返回空");
        }
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 循环引用问题 — shared_ptr 的最大陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: A 持有 B 的 shared_ptr，B 持有 A 的 shared_ptr → 引用计数永不归零
    // 为什么: 引用计数只追踪 shared_ptr 数量，不考虑"谁引用谁"的拓扑
    // 何时出现: 树结构中的双向引用、观察者模式、回调函数持有 owner
    {
        lesson::print_subtitle("Part 4: 循环引用 — 内存泄漏演示");

        // ❌ 错误示范: 双向 shared_ptr
        std::println("--- 错误示范: 双向 shared_ptr ---");
        {
            auto parent = std::make_shared<Node>("Parent");
            auto child  = std::make_shared<Node>("Child");

            parent->child = child;   // parent → child (引用计数 +1)
            child->parent = parent;  // child → parent (引用计数 +1) ← 循环!

            std::println("  离开作用域前:");
            std::println("    parent use_count = {}", parent.use_count());  // 2
            std::println("    child  use_count = {}", child.use_count());   // 2
        } // parent 和 child 局部变量析构(计数各-1), 但还互相持有(计数各=1) → 永远不析构!
        std::println("  ⚠️ Parent 和 Child 的析构函数都没有被调用! 内存泄漏!\n");

        // ✅ 正确示范: 用 weak_ptr 打破循环
        std::println("--- 正确示范: weak_ptr 打破循环 ---");
        {
            auto parent = std::make_shared<SafeNode>("SafeParent");
            auto child  = std::make_shared<SafeNode>("SafeChild");

            parent->child = child;   // OK, 强引用
            child->parent = parent;  // OK, 弱引用 — 不增加引用计数

            std::println("  离开作用域前:");
            std::println("    parent use_count = {}", parent.use_count());  // 2 (parent自身 + child->parent弱引用不计)
            std::println("    child  use_count = {}", child.use_count());   // 1 (child自身, parent->child)

            // 使用 weak_ptr 前必须先 lock
            if (auto p = child->parent.lock()) {
                std::println("  通过 weak_ptr 访问父节点: {}", p->label);
            }
        } // 正常析构! child 先释放 → parent.use_count 降为 1 → parent 释放
        std::println("  ✅ SafeParent 和 SafeChild 都正确析构!\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: enable_shared_from_this — 安全地从 this 获取 shared_ptr
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: 继承 enable_shared_from_this<T>，调用 shared_from_this() 获取
    //         指向 this 的 shared_ptr
    // 为什么: 直接写 shared_ptr<T>(this) 会创建新的控制块 → 双重释放!
    //         类内部不知道自己被 shared_ptr 管理了
    // 何时用: 需要在类内部把 this 作为 shared_ptr 传递时 (注册回调、异步操作)
    {
        lesson::print_subtitle("Part 5: enable_shared_from_this");

        // 正确的使用方式
        auto dog = std::make_shared<Dog>("旺财");
        std::println("  初始 use_count = {}", dog.use_count());  // 1

        // 在类内部安全获取 shared_ptr
        auto dog2 = dog->get_shared();
        std::println("  get_shared() 后 use_count = {}", dog.use_count());  // 2

        dog->bark();
        dog2->bark();

        // ❌ 错误示范 (注释掉以防崩溃):
        // Dog* raw_dog = new Dog("危险狗");
        // auto bad = raw_dog->shared_from_this();  // 崩溃! raw_dog 没有被 shared_ptr 管理
        // 必须在对象已被 shared_ptr 管理后才能调用 shared_from_this()
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 6: 性能特性总结
    // ═══════════════════════════════════════════════════════════════════════════
    {
        lesson::print_subtitle("Part 6: 内存开销与性能小结");

        std::println("unique_ptr<T>:       大小 = 1 个指针 (零额外开销)");
        std::println("shared_ptr<T>:       大小 = 2 个指针 (对象指针 + 控制块指针)");
        std::println("weak_ptr<T>:         大小 = 2 个指针");
        std::println("控制块包含: 强引用计数、弱引用计数、删除器、分配器");
        std::println("make_shared:         对象+控制块连续分配 → 缓存友好");
        std::println("");
        std::println("选型指南:");
        std::println("  - 独占所有权 → unique_ptr (默认选择)");
        std::println("  - 共享所有权, 无循环 → shared_ptr");
        std::println("  - 打破循环/观察者 → weak_ptr");
        std::println("  - 临时观察, 不拥有 → 裸指针或引用 (仅用于参数传递!)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");
    std::println("1. 循环引用: A↔B 用 shared_ptr 互相持有 → 内存泄漏 → 用 weak_ptr 打破");
    std::println("2. 用 shared_ptr<T>(this) 创建新的控制块 → 双重释放 → 用 enable_shared_from_this");
    std::println("3. weak_ptr.lock() 返回的 shared_ptr 可能为空 → 必须检查!");
    std::println("4. 在多线程中对同一个 shared_ptr 读写 → 数据竞争 → 用 atomic 版本");
    std::println("5. make_shared + weak_ptr → 对象内存无法提前回收 (控制块阻止释放)");
    std::println("6. 不要用 shared_ptr 管理 this 如果没有继承 enable_shared_from_this");
    std::println("7. 传递 shared_ptr 给函数 → 用 const shared_ptr<T>& 避免不必要的引用计数操作");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");
    std::println("1. 写一个双向链表，用 shared_ptr 持有 next，weak_ptr 持有 prev");
    std::println("2. 故意创建循环引用 (A→B→A)，用 Valgrind/AddressSanitizer 检测泄漏");
    std::println("3. 实现一个简单的对象缓存，用 weak_ptr 存储，自动清除过期对象");
    std::println("4. 为 Node 类添加 enable_shared_from_this，实现 get_shared()");
    std::println("5. (进阶) 查看 make_shared 和 shared_ptr<T>(new T) 的汇编代码差异");
    std::println("6. (进阶) 研究 std::atomic<std::shared_ptr<T>> (C++20) 的线程安全机制");

    return 0;
}
