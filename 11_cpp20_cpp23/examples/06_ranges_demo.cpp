// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: Ranges 库 — views、管道操作符、range adaptors、ranges::to             ║
// ║  目标: 掌握 range 概念、views 组合、管道风格数据处理、ranges::to 转换        ║
// ║                                                                            ║
// ║  Ranges 核心思想:                                                            ║
// ║  传统 STL: sort(v.begin(), v.end())  — 迭代器对，繁琐                      ║
// ║  Ranges:   std::ranges::sort(v)       — 传容器，简洁                        ║
// ║  View 管道: v | filter(...) | transform(...) | take(...) — 懒求值流水线     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/ranges.hpp"
#include <print>
#include <ranges>     // C++20 ranges
#include <vector>
#include <string>
#include <algorithm>

int main() {
    lesson::print_header("C++20/23 Ranges 库");

    // ── 准备数据 ──
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    // ── Part 1: ranges 算法 ──
    lesson::print_subtitle("Part 1: Ranges 算法 — 直接传容器");
    // 传统方式: std::sort(numbers.begin(), numbers.end());
    std::ranges::sort(numbers, std::greater{});
    std::print("  降序排列: ");
    for (auto n : numbers) std::print("{} ", n);
    std::println("");

    // ── Part 2: views ──
    lesson::print_subtitle("Part 2: Views — 懒求值视图");

    // filter: 只保留偶数
    auto even_view = numbers | std::views::filter([](int n) { return n % 2 == 0; });
    std::print("  过滤偶数 (lazy): ");
    for (auto n : even_view) std::print("{} ", n);
    std::println("");

    // transform: 平方
    auto squared = numbers | std::views::transform([](int n) { return n * n; });
    std::print("  平方映射: ");
    for (auto n : squared) std::print("{} ", n);
    std::println("");

    // take + drop
    lesson::print_separator();
    std::print("  take(5): ");
    for (auto n : numbers | std::views::take(5)) std::print("{} ", n);
    std::println("");

    std::print("  drop(5): ");
    for (auto n : numbers | std::views::drop(5)) std::print("{} ", n);
    std::println("");

    // ── Part 3: 管道操作符组合 ──
    lesson::print_subtitle("Part 3: 管道操作符 | — 组合多条 view");

    // 需求: 从数据中取出"大于 3 的偶数，取前 3 个，然后平方"
    // 传统方式: 需要嵌套 if + 计数 + 临时变量
    // 管道方式: 声明式，读起来像需求描述
    auto pipeline = numbers
                  | std::views::filter([](int n) { return n > 3; })
                  | std::views::filter([](int n) { return n % 2 == 0; })
                  | std::views::take(3)
                  | std::views::transform([](int n) { return n * n; });

    std::print("  filter(>3) | filter(even) | take(3) | transform(^2): ");
    for (auto n : pipeline) std::print("{} ", n);
    std::println("");

    // ── Part 4: ranges::to — 物化为容器 (C++23) ──
    lesson::print_subtitle("Part 4: ranges::to<T>() — view 转为具体容器 (C++23)");

    // 将 view 的结果收集到 vector
    auto result_vec = numbers
                    | std::views::filter([](int n) { return n % 2 == 0; })
                    | std::views::transform([](int n) { return n * 10; })
                    | std::ranges::to<std::vector<int>>();  // C++23

    std::print("  偶数 * 10 = [");
    for (size_t i = 0; i < result_vec.size(); ++i)
        std::print("{}{}", result_vec[i], i + 1 < result_vec.size() ? ", " : "");
    std::println("]");

    // ── C++23: zip 和 enumerate ──
    lesson::print_subtitle("C++23 新增: views::zip 和 views::enumerate");

    std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
    std::vector<int>         scores = {95, 87, 92};

    std::println("  zip: 合并两个容器");
    for (auto [name, score] : std::views::zip(names, scores)) {
        std::println("    {} -> {}", name, score);
    }

    std::println("  enumerate: 带索引遍历");
    for (auto [idx, name] : std::views::enumerate(names)) {
        std::println("    [{}] {}", idx, name);
    }

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. view 是懒求值 — 修改底层数据会影响 view 结果");
    std::println("  2. 某些 view 不可多次遍历 (input range)，需用 ranges::to 提前物化");
    std::println("  3. 管道中的 filter 可能导致元素索引变化，注意迭代器失效");
    std::println("  4. zip 要求所有输入 range 等长 (或最短的那个决定终止)");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 用 views::iota(1) 生成无限序列，配合 take 和 filter 求前 20 个质数");
    std::println("  2. 将两个 vector 用 zip 合并，再用 transform 生成新数据，最后 ranges::to");
    std::println("  3. 尝试 views::join 将 vector<vector<int>> 扁平化");
}
