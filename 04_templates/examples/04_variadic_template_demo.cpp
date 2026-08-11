// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 04 变参模板 演示                                             ║
// ║  学习目标:                                                                    ║
// ║    1. 理解参数包 (Parameter Pack) 的概念                                     ║
// ║    2. 掌握 sizeof... 查询参数包大小                                          ║
// ║    3. 学会递归展开参数包的经典技巧                                           ║
// ║    4. 掌握逗号展开技巧（无需递归的展开方式）                                  ║
// ║  前置: 01_function_template, 02_class_template                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/variadic_template.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>

int main() {
    lesson::print_header("04_templates — 04 变参模板");

    // ── Part 1: 参数包基础 ──
    lesson::print_subtitle("Part 1: 参数包与 sizeof...");
    {
        show_arg_count();                          // 0 个参数
        show_arg_count(1, 2.0, "hello");           // 3 个参数
        show_arg_count(1, 2, 3, 4, 5);            // 5 个参数
    }

    // ── Part 2: 递归展开 ──
    lesson::print_subtitle("Part 2: 经典递归展开");
    {
        std::print("recursive_print: ");
        recursive_print(1, 2.0, std::string("三"), "四");
        std::println("");
    }

    // ── Part 3: 递归展开高级用法 ──
    lesson::print_subtitle("Part 3: 递归求和与连接");
    {
        std::println("sum(1, 2, 3, 4, 5) = {}", sum(1, 2, 3, 4, 5));
        std::println("sum(1.5, 2.5, 3.0)  = {}", sum(1.5, 2.5, 3.0));

        auto s = concat("Hello", ", ", "World", "!", " (", 2024, ")");
        std::println("concat 结果: {}", s);
    }

    // ── Part 4: 逗号展开 ──
    lesson::print_subtitle("Part 4: 逗号展开技巧");
    {
        std::println("初始化列表展开 (print_all):");
        print_all("苹果", "香蕉", 42, 3.14);

        std::println("折叠表达式展开 (print_all_fold):");
        print_all_fold("C++", 17, "折叠表达式", "更优雅");
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. 参数包必须放在模板参数列表的最后（如果有多个参数）");
    std::println("2. sizeof...(Args) 和 sizeof...(args) 是编译期常量");
    std::println("3. 递归展开需要有终止条件（无参或单参版本）");
    std::println("4. 递归展开会产生多份函数实例，参数太多时编译慢");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 写一个变参模板函数 product(T first, Rest... rest)，返回所有参数的乘积。
//    提示：递归终止条件是单参数版本（返回该参数本身）。
// 2. 用逗号展开技巧实现一个 for_each_arg(func, args...) 模板。
//    对每个参数调用 func(args)，例如 for_each_arg([](auto x){ println("{}",x); }, 1,2,3)。
// 3. 实现变参模板类 MyTuple<T...>（不需要完整实现，只需要存储和访问第一个元素）。
//    提示：用递归继承（MyTuple<Head, Tail...> 继承自 MyTuple<Tail...>）。
