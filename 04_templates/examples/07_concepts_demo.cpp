// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 07 C++20 Concepts 演示                                       ║
// ║  学习目标:                                                                    ║
// ║    1. 理解 concept 是什么——对模板参数的编译期约束                             ║
// ║    2. 掌握 requires 子句和 requires 表达式的语法                             ║
// ║    3. 学会使用标准库预定义的 concept                                          ║
// ║    4. 学会定义自定义 concept                                                 ║
// ║    5. 掌握简写语法（auto 约束）替代传统 template 声明                        ║
// ║  前置: 06_type_traits_basics                                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/concepts.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>

int main() {
    lesson::print_header("04_templates — 07 C++20 Concepts");

    // ── Part 2: 标准库 concept ──
    lesson::print_subtitle("Part 2: 标准库预定义 concept");
    {
        std::println("integral_double(21)  = {}", integral_double(21));
        std::println("integral_triple(7)  = {}", integral_triple(7));
        // integral_double(3.14);  // 编译错误: double 不满足 integral
    }

    // ── Part 3: 自定义 concept ──
    lesson::print_subtitle("Part 3: 自定义 concept");
    {
        std::println("generic_square(5)    = {}", generic_square(5));
        std::println("generic_square(3.0)  = {}", generic_square(3.0));

        std::vector<int> v{1, 2, 3, 4};
        print_size_info(v);  // vector 有 .size()
        // print_size_info(42);  // 编译错误: int 不满足 HasSize
    }

    // ── Part 4: requires 表达式 ──
    lesson::print_subtitle("Part 4: requires 表达式综合示例");
    {
        std::println("std::vector<int> 满足 Iterable? {}",
                     Iterable<std::vector<int>>);
        std::println("int 满足 Iterable? {}",
                     Iterable<int>);
    }

    // ── Part 5: 简写语法 ──
    lesson::print_subtitle("Part 5: 简写语法 (auto 约束)");
    {
        std::println("传统写法: {}", traditional_max(30, 20));
        std::println("简写语法: {}", abbreviated_max(30, 20));

        multi_constrained(42, 3.14, "hello");
        multi_constrained(100, 2.718, std::string("world"));
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. concept 约束的是接口，不是实现细节");
    std::println("2. requires 表达式中的代码不会真正执行，只检查合法性");
    std::println("3. 简写语法的每个 auto 都是独立推导，类型可以不同！");
    std::println("4. concept 不能递归引用自己（concept A 依赖 concept B，B 又依赖 A）");
    std::println("5. SFINAE 仍然需要理解：大量旧代码和库还在用，concept 是新标准");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 定义一个 concept Addable，要求 T 支持 + 运算且结果能转为 T。
//    然后写一个函数 template<Addable T> T add(T a, T b) { return a + b; }。
//    测试 int、double、std::string（std::string 的 + 是拼接，结果能转为 string）。
// 2. 定义一个 concept Container，要求 T 有 begin()、end()、size()。
//    写一个函数打印容器中所有元素。
// 3. 将练习 2 的函数改写为简写语法（auto 约束），比较两种写法的可读性。
// 4. 对比 concept、enable_if、if constexpr 的报错信息质量：
//    故意传入不满足约束的类型，观察编译器报错的长短和清晰度。
