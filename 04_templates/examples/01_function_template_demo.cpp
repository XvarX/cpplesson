// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 01 函数模板 演示                                             ║
// ║  学习目标:                                                                    ║
// ║    1. 理解函数模板的本质：用类型参数化函数                                     ║
// ║    2. 掌握隐式/显式实例化及其区别                                             ║
// ║    3. 理解模板实参推导规则                                                    ║
// ║    4. 学会使用 C++20 auto 模板参数简化写法                                    ║
// ║  前置: 03_advanced_typing (auto/decltype)                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/function_template.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>

int main() {
    lesson::print_header("04_templates — 01 函数模板");

    // ── Part 1: 基本使用 ──
    lesson::print_subtitle("Part 1: 基本函数模板");
    {
        std::println("my_max(10, 20)     = {}", my_max(10, 20));       // int
        std::println("my_max(3.14, 2.71) = {}", my_max(3.14, 2.71));   // double
    }

    // ── Part 2: 隐式 vs 显式实例化 ──
    lesson::print_subtitle("Part 2: 显式/隐式实例化");
    {
        // 隐式实例化：编译器自己推导 T = int
        std::println("隐式 my_max(5, 8)         = {}", my_max(5, 8));

        // 显式实例化：强制 T = double
        std::println("显式 my_max<double>(5, 8) = {}", my_max<double>(5, 8));

        // 混合类型用显式实例化解决
        // my_max(3, 5.0);  // 编译错误！
        std::println("显式解决: my_max<double>(3, 5.0) = {}",
                     my_max<double>(3, 5.0));
    }

    // ── Part 3: 类型推导 ──
    lesson::print_subtitle("Part 3: 类型推导与转发引用");
    {
        int x = 42;
        std::println("传入左值 x:");
        print_type_info(x);         // T = int&, 引用折叠后 → int&

        std::println("传入右值 42:");
        print_type_info(42);        // T = int, 参数是 int&&
    }

    // ── Part 4: C++20 auto 模板 ──
    lesson::print_subtitle("Part 4: C++20 auto 模板参数");
    {
        // auto_max 中 a 和 b 可以是不同类型
        std::println("auto_max(10, 20)       = {}", auto_max(10, 20));
        std::println("auto_max(3.14, 1)      = {}", auto_max(3.14, 1));
        // 注意：返回类型也是 auto，编译期推导
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. 模板定义必须在头文件中可见");
    std::println("2. 类型推导冲突 -> 显式指定模板实参");
    std::println("3. 引用折叠规则是 T&& 万能引用的基础");
    std::println("4. auto 模板参数简洁但丧失了 SFINAE 能力");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 写一个 print_twice(T val) 模板，打印 val 两次，换行分隔。
//    测试 int、string、double 三种类型。
// 2. 写一个 add(a, b) 函数模板，返回 a + b。
//    测试 my_max(3, 5.0) 会编译失败，然后用显式实例化解决。
// 3. 将 my_max 改为 C++20 auto 模板参数写法 (auto_max 已给出)，比较两者的差异。
