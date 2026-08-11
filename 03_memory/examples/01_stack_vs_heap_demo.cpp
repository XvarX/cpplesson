// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: 01_stack_vs_heap_demo — 栈 vs 堆分配                            ║
// ║  目标: 理解栈/堆区别、new/delete、数组分配、内存泄漏、operator new/delete      ║
// ║  前置: 理解变量生命周期、指针基础                                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <memory>
#include <vector>
#include "shared/lesson_utils.hpp"
#include "memory/stack_vs_heap.hpp"

int main() {
    lesson::print_header("内存管理 (1): 栈 vs 堆");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 栈分配 — 自动管理、速度快、大小受限
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: 函数内的局部变量自动在栈上分配，离开作用域自动销毁
    // 为什么: 栈是 CPU 原生支持的 LIFO 结构，分配/释放只需移动栈指针(SP)，极快
    // 何时用: 大小在编译期确定、生命周期局限在函数内的对象
    {
        lesson::print_subtitle("Part 1: 栈分配 (自动存储期)");
        std::println("进入作用域...");
        int x = 42;                    // 栈上分配一个 int
        double arr[100];               // 栈上分配 100 个 double 的数组
        Widget w{1};                   // 栈上分配对象，离开作用域自动析构
        std::println("栈变量 x = {}, w.id = {}", x, w.id);
        std::println("离开作用域 — 栈上的对象会自动销毁...");
    } // ← w 在这里自动析构，不需要手动 delete
    std::println("(已离开作用域)\n");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 堆分配 (new/delete) — 手动管理、灵活、需要程序员负责
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: new 在堆上分配内存并调用构造函数，delete 调用析构并释放内存
    // 为什么: 堆的生命周期不绑定作用域，大小可以在运行时决定
    // 何时用: 对象需要在函数外存活、大小运行时才知道、多态场景
    {
        lesson::print_subtitle("Part 2: 堆分配 (new / delete)");
        Widget* pw = new Widget{10};   // 堆上分配，返回指针
        std::println("堆对象 pw->id = {}", pw->id);
        std::println("注意: 离开作用域时指针 pw 本身在栈上，但它指向的对象还在堆上！");
        // ⚠️ 如果这里不写 delete，Widget #10 永远不会析构 → 内存泄漏
        delete pw;                     // 手动释放
        std::println("(已手动 delete)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: new[] / delete[] — 动态数组
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: new T[n] 分配 n 个对象的连续内存
    // 为什么: C 的 malloc 不调用构造函数，new[] 会为每个元素调用默认构造
    // 何时用: 运行时才知道数组大小且不想用 vector 的极端场景 (极少 — 优先用 vector!)
    // ⚠️ 致命陷阱: new[] 必须配 delete[]，不能用 delete，否则未定义行为！
    {
        lesson::print_subtitle("Part 3: 数组分配 (new[] / delete[])");
        constexpr int n = 3;
        Widget* arr = new Widget[n]{100, 101, 102};  // 一次构造 3 个 Widget
        std::println("数组元素: arr[0].id={}, arr[1].id={}, arr[2].id={}",
                     arr[0].id, arr[1].id, arr[2].id);
        delete[] arr;  // 必须用 delete[]，否则只析构第一个元素！
        std::println("(已 delete[])\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 内存泄漏 — 最常见且最难调试的 C++ 问题
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: 分配了堆内存但没有释放，导致内存占用不断增长
    // 为什么危险: 长时间运行的程序(服务器、游戏)最终会耗尽内存崩溃
    // 如何避免: 用智能指针(后续章节)、RAII、容器，尽量不用裸 new
    {
        lesson::print_subtitle("Part 4: 常见内存泄漏场景");

        // 场景 A: 忘记 delete
        {
            Widget* leaked = new Widget{999};
            // 忘记写 delete leaked;
        } // leaked 指针销毁了，但 Widget #999 还留在堆上 → 泄漏！
        std::println("⚠️ 上面作用域内的 Widget #999 永远不会被析构 — 内存泄漏!");

        // 场景 B: 异常提前退出
        std::println("如果函数在 new 和 delete 之间抛出异常，delete 不会执行 → 泄漏");
        std::println("→ 解决方案: RAII + 智能指针 (后面章节详细讲)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: operator new / operator delete — 底层内存分配接口
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: operator new 只负责分配原始内存 (不调用构造)
    //          operator delete 只负责释放原始内存 (不调用析构)
    // 为什么: 了解底层机制有助于理解 placement new、自定义分配器等
    // 何时用: 实现自定义内存池、调试内存问题、placement new 场景
    {
        lesson::print_subtitle("Part 5: operator new / operator delete 底层接口");
        // operator new 返回 void*，只分配裸内存，不构造对象
        void* raw = operator new(sizeof(Widget));
        std::println("分配裸内存: {} 字节，地址 {}", sizeof(Widget), raw);

        // 必须在裸内存上手动构造对象 (placement new — 下节课详讲)
        Widget* pw2 = new(raw) Widget{42};  // placement new: 在指定地址构造
        std::println("手动构造 Widget, id = {}", pw2->id);

        // 手动析构 (不释放内存)
        pw2->~Widget();
        std::println("手动调用了析构函数 (内存还在)");

        // 释放裸内存
        operator delete(raw);
        std::println("释放裸内存\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 6: 现代 C++ 最佳实践 — 用容器/智能指针替代裸 new
    // ═══════════════════════════════════════════════════════════════════════════
    {
        lesson::print_subtitle("Part 6: 现代 C++ 实践 — 避免裸 new");

        // ❌ 老旧写法 — 容易泄漏
        int* old_way = new int[1000];
        delete[] old_way;

        // ✅ 现代写法 — 零泄漏风险
        std::vector<int> modern_way(1000);              // 自动管理内存
        auto sp = std::make_unique<int[]>(1000);        // 智能指针管理数组
        std::println("用 vector 和 unique_ptr 替代裸 new[]，零泄漏风险\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");
    std::println("1. new[] 配 delete → 未定义行为 (必须配 delete[])");
    std::println("2. delete 同一个指针两次 → 双重释放 (double free)，程序崩溃");
    std::println("3. 释放后继续用指针 (use-after-free) → 悬垂指针，随机崩溃");
    std::println("4. 忘记 delete 异常路径上的指针 → 异常安全问题");
    std::println("5. 返回局部变量的地址 → 悬垂引用/指针 (栈变量随作用域消失)");
    std::println("6. 在构造函数中 new 但在析构中忘 delete → 逐步泄漏");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");
    std::println("1. 写一个类，在构造/析构中打印消息，用 new 创建它，观察何时析构");
    std::println("2. 故意不写 delete，运行程序，看析构函数是否被调用");
    std::println("3. 用 new[] 创建包含 5 个上述对象的数组，分别用 delete 和 delete[] 释放，观察区别");
    std::println("4. 写一个函数，在中间 throw 异常，观察 new 的对象是否泄漏");
    std::println("5. (进阶) 重载类的 operator new/delete，统计分配/释放次数");

    return 0;
}
