// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: std::span 和 std::source_location                                    ║
// ║  目标: 掌握 span 替代指针+长度的安全模式，source_location 替代宏的现代方案   ║
// ║                                                                            ║
// ║  学习方式: span_utils.hpp 中声明了所有辅助函数，实现在 src/span_utils.cpp    ║
// ║  下面的代码演示如何使用这些现代 C++20 工具。                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/span_utils.hpp"
#include <print>
#include <vector>
#include <array>
#include <string>

int main() {
    lesson::print_header("std::span & std::source_location");

    // ── Part 1: span 动态 extent ──
    lesson::print_subtitle("Part 1: std::span — 动态 extent");

    int c_array[] = {10, 20, 30};
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::array<int, 4> arr = {100, 200, 300, 400};

    std::println("  C数组 span:");
    print_span(c_array);

    std::println("  vector span:");
    print_span(vec);

    std::println("  array span:");
    print_span(arr);

    // subspan 演示
    lesson::print_separator();
    std::println("  subspan 修改前: ");
    print_span(vec);
    zero_first_half(vec);
    std::println("  zero_first_half 后: ");
    print_span(vec);

    // ── Part 2: span 静态 extent ──
    lesson::print_subtitle("Part 2: std::span — 静态 extent (编译期大小)");

    std::println("  sum_fixed(arr) = {}", sum_fixed<int{4}>(std::span<const int, 4>{arr}));
    // 注意: 静态 extent 的 span 只能从大小匹配的数组构造

    // ── Part 3: source_location ──
    lesson::print_subtitle("Part 3: std::source_location — 替代 __FILE__/__LINE__");

    log_message("这是一条测试日志");
    log_message("不同位置的日志");

    my_assert(2 + 2 == 4, "2+2==4");
    my_assert(2 + 2 == 5, "2+2==5");

    lesson::print_note("source_location 的优势: 作为默认参数，调用者无需手动传参");
    lesson::print_note("对比宏: 宏是文本替换，source_location 是类型安全的 C++ 对象");

    // ── Part 4: 实践模式 ──
    lesson::print_subtitle("Part 4: span 实践 — 函数接受任何连续容器");

    std::vector<double> scores = {85.5, 92.0, 78.5, 88.0, 95.0};
    std::println("  平均分: {:.1f}", average(scores));

    std::println("  取尾部 3 个:");
    auto tail3 = tail(vec, 3);
    std::print("    ");
    for (auto v : tail3) std::print("{} ", v);
    std::println("");

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. span 不拥有数据 — 原数据销毁后 span 成为悬空引用");
    std::println("  2. 静态 extent span 只能从大小匹配的容器构造");
    std::println("  3. span<const T> 可接受任何 span<T>，反之不行 (const 安全)");
    std::println("  4. source_location::current() 必须在声明时设默认值，不能手动传 new");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 写一个函数用 span<int> 反转数组，不分配新内存");
    std::println("  2. 实现 log_error() 函数用 source_location 自动记录出错位置");
    std::println("  3. 用固定 extent 的 span 写一个矩阵行访问函数");
}
