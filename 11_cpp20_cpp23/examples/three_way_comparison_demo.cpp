// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: <=> 三路比较 — strong/weak/partial_ordering、=default、自定义<=>     ║
// ║  目标: 理解三种比较类别，学会用 =default 和自定义 <=> 生成所有比较运算符     ║
// ║                                                                            ║
// ║  学习方式: Person / CaseInsensitiveString / FloatWrapper 定义在头文件中。    ║
// ║  下面演示如何使用这些类型，以及三种比较类别的差别。                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/three_way_types.hpp"
#include <print>
#include <compare>
#include <string>
#include <limits>

int main() {
    lesson::print_header("C++20 <=> 三路比较");

    // ── Part 1: =default 自动生成比较运算符 ──
    lesson::print_subtitle("Part 1: =default — 一行代码生成全部比较运算符");

    Person alice{"Alice", 30};
    Person bob{"Bob", 25};
    Person alice2{"Alice", 30};

    // 编译器自动生成了所有比较运算符
    std::println("  alice == alice2: {}", alice == alice2);   // true
    std::println("  alice <  bob:    {}", alice < bob);       // true (Alice < Bob)
    std::println("  alice >  bob:    {}", alice > bob);       // false
    std::println("  alice != bob:    {}", alice != bob);      // true
    std::println("  alice <= alice2: {}", alice <= alice2);   // true

    // ── 使用 <=> 运算符本身 ──
    lesson::print_separator();
    auto cmp = alice <=> bob;
    if (cmp < 0)  std::println("  alice < bob (name 字典序)");
    if (cmp == 0) std::println("  alice == bob");
    if (cmp > 0)  std::println("  alice > bob");

    // ── Part 2: 自定义 <=> ──
    lesson::print_subtitle("Part 2: 自定义 <=> — 不同比较逻辑");

    // ── weak_ordering 示例 ──
    CaseInsensitiveString s1{"hello"};
    CaseInsensitiveString s2{"world"};
    CaseInsensitiveString s3{"hi"};

    auto r1 = s1 <=> s2;  // 长度 5 vs 5 → equivalent (但内容不同)
    auto r2 = s1 <=> s3;  // 长度 5 vs 2 → greater

    std::println("  'hello'(len5) vs 'world'(len5): {}", r1 == 0 ? "equivalent" : "not equivalent");
    std::println("  'hello'(len5) vs 'hi'(len2):     {}", (r2 > 0) ? "greater" : "not greater");
    std::println("  'hello' == 'world' (值比较): {}", s1 == s2);

    // ── partial_ordering 示例 ──
    lesson::print_separator();
    FloatWrapper f1{3.14};
    FloatWrapper f2{2.71};
    FloatWrapper nan{std::numeric_limits<double>::quiet_NaN()};

    auto r3 = f1 <=> f2;
    auto r4 = f1 <=> nan;

    std::println("  3.14 <=> 2.71: {}", r3 > 0 ? "greater" : "less");
    std::println("  3.14 <=> NaN:  {}", r4 == std::partial_ordering::unordered ? "unordered (无法比较)" : "comparable");

    // ── Part 3: 转换关系 ──
    lesson::print_subtitle("Part 3: 比较类别转换 — 从强到弱");

    std::strong_ordering  so = std::strong_ordering::equal;
    std::weak_ordering    wo = so;   // 强 → 弱: 隐式转换 OK
    std::partial_ordering po = wo;   // 弱 → 偏: 隐式转换 OK
    std::println("  strong → weak → partial 是合法的隐式转换");

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. 自定义 operator<=> 不会自动生成 operator==");
    std::println("     =default 时编译器才会同时生成 == 和 != (C++20)");
    std::println("  2. 不要把不同比较类别的 <=> 混用 — 注意类型安全");
    std::println("  3. NaN < 0 和 NaN >= 0 都是 false — partial_ordering 的必要性");
    std::println("  4. =default 按成员声明顺序比较 — 顺序影响性能(先比便宜的成员)");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 定义一个结构体有 3 个字段，用 =default 生成 <=>，验证比较顺序");
    std::println("  2. 为一个日期类实现自定义 operator<=>，先比年再比月再比日");
    std::println("  3. 尝试将 std::weak_ordering::equivalent 赋值给 std::strong_ordering，观察编译器错误");
}
