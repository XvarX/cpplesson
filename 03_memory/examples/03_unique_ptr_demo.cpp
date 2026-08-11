// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: 03_unique_ptr_demo — 独占所有权智能指针                          ║
// ║  目标: 掌握 unique_ptr、make_unique、所有权转移、自定义删除器、数组版本       ║
// ║  前置: 理解裸指针 new/delete、移动语义基础                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <memory>       // unique_ptr, make_unique (C++14), default_delete
#include <string>
#include <vector>
#include <cstdio>       // FILE*, std::fclose (演示自定义删除器)
#include "shared/lesson_utils.hpp"
#include "memory/unique_ptr.hpp"

int main() {
    lesson::print_header("内存管理 (3): std::unique_ptr");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: unique_ptr 基础 — 独占所有权 + 自动释放
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: unique_ptr<T> 独占管理一个 T 类型对象，指针销毁时自动 delete
    // 为什么: 消除"忘记 delete"这类 bug，表达"这个对象只属于我"的语义
    // 何时用: 几乎总是! 只要不共享，就用 unique_ptr。零额外开销 (大小 = 裸指针)
    {
        lesson::print_subtitle("Part 1: unique_ptr 基础");

        // C++14 推荐写法: make_unique (异常安全 + 避免写两次类型名)
        auto p1 = std::make_unique<Resource>("数据库连接");
        p1->use();

        // unique_ptr 不能被复制 (保证独占性)
        // auto p2 = p1;  // ❌ 编译错误: unique_ptr 的拷贝构造 = delete

        // 但可以被移动 (转移所有权)
        auto p2 = std::move(p1);
        std::println("转移所有权后: p1 是否为 nullptr? {}", (p1 == nullptr));  // true
        p2->use();
        // p2 离开作用域 → 自动释放 "数据库连接"
        std::println("(离开作用域)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 所有权转移 — 工厂函数与参数传递
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: unique_ptr 可以通过 std::move 在函数间转移所有权
    // 为什么: 让调用者明确知道所有权转移了，而不是隐式共享导致混乱
    // 何时用: 工厂模式(创建并返回)、将资源交给另一个组件管理
    {
        lesson::print_subtitle("Part 2: 所有权转移模式");

        // 工厂函数返回 unique_ptr
        auto res = create_resource("临时缓冲区");
        res->use();

        // 将所有权传入函数 (按值传递 = 转移所有权)
        consume_resource(std::move(res));
        std::println("函数返回后: res 是否为 nullptr? {}", (res == nullptr));  // true

        // 也可以传递引用 (不转移所有权)
        // void borrow(unique_ptr<Resource>& ref) → 只借用，不获取所有权
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 自定义删除器 — 管理非内存资源
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: unique_ptr<T, Deleter> — 第二个模板参数指定如何释放资源
    // 为什么: 资源不一定是 new 出来的，可能是文件句柄、socket、COM 对象等
    // 何时用: 管理 FILE*、HANDLE、自定义内存池分配的资源等
    {
        lesson::print_subtitle("Part 3: 自定义删除器");

        // 场景 A: 管理 C 风格 FILE*
        std::FILE* raw_file = std::fopen("NUL", "w"); // 在 Windows 上用 NUL 模拟
        // NUL 在 Windows 上等价于 /dev/null
        if (raw_file) {
            // 用 unique_ptr + 自定义删除器包装 FILE*
            std::unique_ptr<std::FILE, FileCloser> file_ptr{raw_file};
            std::fputs("hello\n", file_ptr.get());
            std::println("写入文件完成");
            // file_ptr 析构时自动调用 FileCloser → fclose
        }

        // 场景 B: 用 lambda 作为删除器 (更轻量)
        {
            std::println("\nLambda 删除器示例:");
            auto deleter = [](Resource* r) {
                std::println("  自定义删除: 额外清理 {} 的日志...", r->name);
                delete r;
            };
            std::unique_ptr<Resource, decltype(deleter)> res{
                new Resource{"Lambda删除的资源"}, deleter
            };
            res->use();
            // 析构时调用 lambda → 打印日志 → delete
        }
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: unique_ptr<T[]> — 数组版本
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: unique_ptr<T[]> 管理动态数组，析构时调用 delete[]
    // 为什么: 比裸指针 new[]/delete[] 更安全，提供 operator[] 访问元素
    // 何时用: 需要动态数组且不想用 vector 的场景 (如固定大小、与 C API 交互)
    // 提示: 绝大多数情况下 std::vector 是更好的选择
    {
        lesson::print_subtitle("Part 4: unique_ptr<T[]> — 数组版本");

        // make_unique<int[]>(10) 创建 int[10]
        auto arr = std::make_unique<int[]>(10);

        // 初始化
        for (int i = 0; i < 10; ++i) {
            arr[i] = i * i;  // 支持 operator[]
        }

        // 输出
        std::print("数组内容: ");
        for (int i = 0; i < 10; ++i) {
            std::print("{} ", arr[i]);
        }
        std::println("");

        // arr 离开作用域 → 自动调用 delete[]，正确析构所有元素
        std::println("(离开作用域 → 自动 delete[])\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: unique_ptr 的内部实现原理 (简化版讲解)
    // ═══════════════════════════════════════════════════════════════════════════
    {
        lesson::print_subtitle("Part 5: unique_ptr 原理");

        std::println("unique_ptr 的核心设计:");
        std::println("  - 内部只有一个裸指针 (零额外空间开销)");
        std::println("  - 拷贝构造/拷贝赋值 = delete (禁止复制)");
        std::println("  - 移动构造/移动赋值 = 转移内部指针, 源指针置空");
        std::println("  - 析构函数 = 若指针非空则调用删除器");
        std::println("  - 对于无状态删除器 (std::default_delete), 优化到和裸指针一样大");
        std::println("  - C++23: std::make_unique_for_overwrite() 跳过值初始化, 提升性能\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");
    std::println("1. 不要用 unique_ptr 管理栈变量的地址 → 析构时会 delete 栈内存, 崩溃");
    std::println("2. 不要从 unique_ptr 中 get() 裸指针后又 delete → 双重释放");
    std::println("3. 不要两个 unique_ptr 管理同一指针 → 双重释放");
    std::println("4. get() 获取的裸指针在 unique_ptr 析构后变悬垂 → 不要保存 get() 的返回值");
    std::println("5. 循环引用中使用 unique_ptr → 不可能, unique_ptr 独占所有权 (用 weak_ptr 替代)");
    std::println("6. C++14 之前没有 make_unique → 用 new + unique_ptr<T>(new T) 替代");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");
    std::println("1. 写一个工厂函数 make_connection()，返回 unique_ptr<Connection>");
    std::println("2. 用 unique_ptr<FILE, decltype(&fclose)> 管理 fopen 打开的文件");
    std::println("3. 写一个类，包含 unique_ptr 成员，观察对象的生命周期");
    std::println("4. 对比 new[]/delete[] 和 unique_ptr<T[]> 的异常安全性");
    std::println("5. (进阶) 实现一个简单版的 unique_ptr (MyUniquePtr)，支持移动语义");
    std::println("6. (进阶) 研究 unique_ptr 的别名构造函数 (aliasing constructor) 的用途");

    return 0;
}
