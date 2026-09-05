// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: Concepts 进阶 — requires 表达式、concept 组合、约束偏序               ║
// ║  目标: 掌握四种 requires 表达式、concept 的逻辑组合、理解约束偏序规则        ║
// ║                                                                            ║
// ║  学习方式: 跟着敲下面的代码，理解 concept 如何取代传统 SFINAE                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/concepts_deep.hpp"
#include <print>
#include <string>
#include <vector>

int main() {
    lesson::print_header("C++20 Concepts 进阶");

    // ── Part 1 演示 ──
    lesson::print_subtitle("Part 1: requires 表达式的四种形式");
    std::println("  std::string 满足 Hashable: {}", Hashable<std::string>);
    std::println("  std::vector<int> 满足 HasValueType: {}", HasValueType<std::vector<int>>);
    std::println("  int 不满足 HasValueType: {}", HasValueType<int>);

    // ── Part 2 演示 ──
    lesson::print_subtitle("Part 2: concept 逻辑组合 (&& / ||)");
    std::println("  int 满足 Numeric: {}", Numeric<int>);
    std::println("  double 满足 Numeric: {}", Numeric<double>);
    std::println("  string 不满足 Numeric: {}", Numeric<std::string>);

    lesson::print_separator();

    auto print_if_numeric = []<typename T>(T val) {
        if constexpr (PrintableNumeric<T>) {
            std::println("  {} 是可打印的数值", val);
        } else {
            std::println("  不是可打印的数值类型");
        }
    };
    print_if_numeric(42);
    print_if_numeric(3.14);
    print_if_numeric(std::string("hello"));

    // ── Part 3 演示: 约束偏序 ──
    lesson::print_subtitle("Part 3: 约束偏序 — 编译器自动选择最匹配版本");
    int signed_val = -5;
    unsigned int unsigned_val = 42u;
    std::println("  describe({}) -> {}", signed_val, describe(signed_val));
    std::println("  describe({}u) -> {}", unsigned_val, describe(unsigned_val));
    // 编译器自动选择: signed int → "整数"; unsigned int → "无符号整数"
    // 因为 unsigned_integral 比 integral 约束更强

    // ── Part 4 演示: requires vs enable_if ──
    lesson::print_subtitle("Part 4: requires vs enable_if — 新时代的约束");
    lesson::print_note("concept 取代 enable_if: 代码可读性提升 10 倍，错误信息缩短 100 倍");

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. requires 表达式和 requires 子句的语法区别:");
    std::println("     template<typename T> requires std::integral<T>  ← requires 子句 (约束模板)");
    std::println("     requires(T a) {{ a + a; }}                        ← requires 表达式 (检测表达式)");
    std::println("  2. concept 组合时注意短路求值 — A||B 中若 A 满足则不再检查 B");
    std::println("  3. 嵌套 requires 必须写在 requires 表达式体内部");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 定义一个 Swappable concept，要求两个值能通过 std::swap 交换");
    std::println("  2. 用 concept 约束一个 find 函数，要求容器支持 begin/end 且元素可比较");
    std::println("  3. 尝试写两个重载函数，一个接受 integral，一个接受 floating_point");
    std::println("     观察编译器在选择时的偏序行为");
}
