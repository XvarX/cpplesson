// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_fundamentals 示例 — const / constexpr / constinit / static / extern /      ║
// ║                         inline                                                ║
// ║  目标: 掌握编译期与运行期常量、存储期、链接性，写出正确的不可变代码           ║
// ║  前置: 01_types_and_literals (基本类型)                                       ║
// ║  对应库头文件: fundamentals/const_static.hpp                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <array>
#include "shared/lesson_utils.hpp"
#include "fundamentals/const_static.hpp"

int main() {
    lesson::print_header("02 const / constexpr / constinit / static / extern / inline");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 1: const — 运行期"不可变"承诺
    // ═══════════════════════════════════════════════════════════════════════════════
    // const 是什么: 一个限定符，表示"我不会修改这个对象"。
    // 为什么存在: 避免意外修改、帮助编译器优化、文档化意图。
    // 什么时候用: 任何你确定不会改的变量，尤其是函数参数和引用。
    // 注意: const 修饰的是运行期常量，值可以在运行时才确定。
    lesson::print_subtitle("Part 1: const 限定符");

    const int max_players = 100;         // 绑定到变量名，不能修改
    // max_players = 200;                // ❌ 编译错误! const 变量不可赋值

    const double pi = 3.141592653589793;
    const std::string app_name = "C++学习器";

    std::println("最大玩家数: {}", max_players);
    std::println("π: {}", pi);
    std::println("应用名: {}", app_name);

    // const 修饰指针的三种写法 (从右往左读):
    int value = 42;
    const int* p1 = &value;        // 指向 const int 的指针: 不能通过 p1 改 value
    int const* p2 = &value;        // 等价写法: p1 和 p2 完全一样
    int* const p3 = &value;        // const 指针: p3 不能指向别处，但可以改 *p3
    const int* const p4 = &value;  // 都不能改

    // *p1 = 10;  // ❌ 编译错误: p1 指向 const int
    // p3 = &max_players;  // ❌ p3 是 const 指针，不能重新绑定

    std::println("*p3 = {} (可以修改，因为 p3 指向非 const int)", *p3);
    *p3 = 100;  // ✅ 可以
    std::println("修改后 *p3 = {}", *p3);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 2: constexpr — 编译期常量，保证在编译时求值
    // ═══════════════════════════════════════════════════════════════════════════════
    // constexpr 是什么: 比 const 更强，保证表达式在编译期就能算出结果。
    // 为什么存在: 性能优化 (编译期算好 → 运行时零开销)、模板元编程、数组大小。
    // 什么时候用: 1) 编译期就知道值的常量  2) 需要作为模板参数  3) 数组大小
    // C++11 的 constexpr 限制很多，C++14/17/20 逐步放宽，允许循环、分支。
    lesson::print_subtitle("Part 2: constexpr 编译期常量");

    constexpr int buffer_size = 4096;              // 编译期常量
    constexpr double e = 2.718281828459045;        // 编译期浮点常量

    // 使用库中的 constexpr 函数 square (定义在 const_static.hpp)
    constexpr int squared = fundamentals::square(10);  // 10*10=100 在编译期算

    // 下面这行的数组大小必须编译期可知，用 constexpr 正合适
    int buffer[buffer_size / 8]{};                 // 512 元素的数组 — 标准 C++!

    std::println("缓冲区大小: {}", buffer_size);
    std::println("自然常数 e: {}", e);
    std::println("square(10)  编译期计算 = {}", squared);
    // 验证 square(10) 确实是编译期求值: 可以直接放进 static_assert
    static_assert(fundamentals::square(5) == 25, "constexpr 函数应该在编译期执行");

    // ── const 和 constexpr 的区别 ──
    int n = 10;                   // n 在运行时才确定
    const int cm = n * 2;         // ✅ 可以: const 不要求编译期已知
    // constexpr int cx = n * 2;  // ❌ 编译错误! n 不是编译期常量

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 3: constinit (C++20) — 保证在编译期初始化，但可以在运行期修改
    // ═══════════════════════════════════════════════════════════════════════════════
    // constinit 是什么: 保证变量在编译期就被初始化 (解决"静态初始化顺序问题")。
    // 为什么存在: 全局/static 变量的初始化顺序在跨翻译单元时是不确定的
    //              (static initialization order fiasco)，constinit 强制编译期初始化。
    // 什么时候用: 对初始化顺序敏感的全局/static 变量。
    // 注意: constinit 只约束初始化，初始化完成后可以修改 (除非还加了 const)。
    lesson::print_subtitle("Part 3: constinit (C++20)");

    // constinit 必须用编译期常量初始化
    constinit static int init_ok = fundamentals::square(5);  // ✅ square(5) 是 constexpr
    // constinit static int init_bad = n;  // ❌ n 不是编译期常量
    std::println("constinit 初始值: {}", init_ok);
    init_ok = 99;  // ✅ 可以修改! constinit 只管初始化，不管后续
    std::println("修改 constinit 变量: {}", init_ok);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 4: static — 存储期修饰符
    // ═══════════════════════════════════════════════════════════════════════════════
    // static 是什么: 改变变量/函数的存储期和链接性。
    //   - static 局部变量: 生命周期 = 程序运行全程，保留上次的值
    //   - static 全局变量: 内部链接 (只在本 .cpp 可见)
    //   - static 成员变量: 属于类而非对象 (后面讲)
    // 为什么存在: 避免全局变量污染、实现单例模式、缓存/记忆化。
    lesson::print_subtitle("Part 4: static 存储期");

    // 使用库中的 create_counter() (定义在 const_static.cpp)
    std::println("create_counter() 第1次调用: {}", fundamentals::create_counter());  // 1
    std::println("create_counter() 第2次调用: {}", fundamentals::create_counter());  // 2
    std::println("create_counter() 第3次调用: {}", fundamentals::create_counter());  // 3
    // static 局部变量的初始化是线程安全的 (C++11 起保证)

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 5: extern — 跨翻译单元的变量/函数声明
    // ═══════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 5: extern 声明");

    // 典型模式: 在头文件中声明 extern，在某个 .cpp 中定义
    // extern int global_counter;  ← 头文件
    // int global_counter = 0;     ← 某个 .cpp
    // 这里我们演示 inline 变量 (C++17)，它在头文件友好场景中取代了 extern

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 6: inline 变量 (C++17) — 头文件友好的全局变量
    // ═══════════════════════════════════════════════════════════════════════════════
    // inline 变量是什么: C++17 允许在头文件中定义变量，链接器保证只存在一份。
    // 为什么存在: 替代 extern + 单独 .cpp 的繁琐模式，尤其适合 header-only 库。
    // 什么时候用: 头文件中需要共享的常量、配置对象。
    lesson::print_subtitle("Part 6: inline 变量 (C++17)");

    std::println("inline_counter (修改前): {}", fundamentals::inline_counter);
    ++fundamentals::inline_counter;
    std::println("inline_counter (修改后): {}", fundamentals::inline_counter);
    // 如果另一个 .cpp 也 include 并修改 inline_counter，看到的是同一个值

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 7: constexpr / consteval 对比
    // ═══════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 7: constexpr vs consteval");

    // constexpr 函数: 编译期调用 → 编译期求值; 运行期调用 → 运行期求值
    int runtime_val = fundamentals::square(n);    // ✅ OK, n 是运行期变量

    // consteval (C++20): 强制编译期求值，绝不生成运行时代码
    constexpr int c5 = fundamentals::compile_square(5);  // ✅ 编译期求值
    // int bad = fundamentals::compile_square(n);         // ❌ 编译错误! n 不是编译期常量

    std::println("constexpr square(n) 运行期: {}", runtime_val);
    std::println("consteval compile_square(5) 编译期: {}", c5);

    // ── 常见陷阱 (Common Pitfalls) ────────────────────────────────────────────
    lesson::print_separator("常见陷阱");
    lesson::print_warn("陷阱1: const 不等于编译期常量 — 不要混用 const 和 constexpr");
    lesson::print_warn("陷阱2: constexpr 函数可以在运行期被调用 — 需要编译期保证用 consteval");
    lesson::print_warn("陷阱3: static 局部变量初始化在多线程环境可能造成竞争 (C++11 前)");
    lesson::print_warn("陷阱4: static initialization order fiasco — 用 constinit 解决");
    lesson::print_warn("陷阱5: extern 变量重复定义 — 改用了 inline 变量 (C++17+)");
    lesson::print_warn("陷阱6: const 指针从右往左读: int const* p = 指向 const int");
    lesson::print_warn("陷阱7: constinit 不能和 constexpr 混用，语义冲突");

    // ── 练习 ─────────────────────────────────────────────────────────────────
    lesson::print_separator("练习");
    std::println("1. 写一个 constexpr 函数 factorial(n)，用 static_assert 验证 factorial(5)==120");
    std::println("2. 把 create_counter() 改成线程安全的版本 (提示: std::atomic<int>)");
    std::println("3. 实验: 把一个 constexpr 函数的参数改成运行期变量，观察是否还能编译");
    std::println("4. 用 constinit 声明一个全局变量，试试不用编译期常量初始化会怎样");
    std::println("5. 总结 const / constexpr / consteval / constinit 四者的区别，画出对比表格");

    return 0;
}
