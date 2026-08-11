// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 05 折叠表达式 演示                                           ║
// ║  学习目标:                                                                    ║
// ║    1. 理解折叠表达式是什么——参数包的简洁运算                                  ║
// ║    2. 掌握一元折叠（左/右）的语法和行为                                       ║
// ║    3. 掌握二元折叠（左/右）的语法和行为                                       ║
// ║    4. 学会在真实场景中替代递归展开和逗号展开                                  ║
// ║  前置: 04_variadic_template                                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/fold_expressions.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>

int main() {
    lesson::print_header("04_templates — 05 折叠表达式");

    // ── Part 2: 一元折叠 ──
    lesson::print_subtitle("Part 2: 一元折叠");
    {
        std::println("左折叠加: (... + args) = {}", left_sum(1, 2, 3, 4));    // 10
        std::println("右折叠加: (args + ...) = {}", right_sum(1, 2, 3, 4));    // 10
        // 注意：对于 + 运算，左右折叠结果相同（加法满足结合律）
        // 但对于不满足结合律的运算符（如 -），左右折叠结果不同！

        std::println("左折叠减: (... - args) = {}", ([]<typename... T>(T... a) {
            return (... - a);  // ((1-2)-3)-4 = -8
        })(1, 2, 3, 4));

        std::println("右折叠减: (args - ...) = {}", ([]<typename... T>(T... a) {
            return (a - ...);  // 1-(2-(3-4)) = -2
        })(1, 2, 3, 4));

        std::println("全部为真: {}", all_true(true, true, true));       // true
        std::println("全部为真: {}", all_true(true, false, true));      // false
        std::println("任一为真: {}", any_true(false, false, true));     // true

        std::println("依次打印:");
        print_all_fold("甲", "乙", "丙");
    }

    // ── Part 3: 二元折叠 ──
    lesson::print_subtitle("Part 3: 二元折叠（有初始值）");
    {
        std::println("safe_sum()           = {}", safe_sum());          // 0
        std::println("safe_sum(1,2,3)      = {}", safe_sum(1, 2, 3));   // 6
        std::println("safe_product()       = {}", safe_product());       // 1
        std::println("safe_product(2,3,4)  = {}", safe_product(2, 3, 4));// 24

        auto s = join_strings("Hello", ", ", "折叠", "表达式", "!");
        std::println("join: {}", s);
    }

    // ── Part 4: 实战对比 ──
    lesson::print_subtitle("Part 4: 实战 — 旧写法 vs 折叠表达式");
    {
        std::vector<int> v1, v2;

        push_all_old(v1, 1, 2, 3, 4, 5);
        push_all_new(v2, 1, 2, 3, 4, 5);

        std::println("旧写法(v1) 大小: {}, 元素: {}", v1.size(), v1);
        std::println("新写法(v2) 大小: {}, 元素: {}", v2.size(), v2);

        formatted_print(" | ", "姓名", 25, "工程师", 3.14);
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. 运算符不满足结合律时, 左/右折叠结果不同! (如 - / %)");
    std::println("2. 空参数包 + 一元折叠: && → true, || → false, 逗号 → void");
    std::println("3. 空参数包 + 一元折叠: + * | & 等运算符不合法, 用二元折叠");
    std::println("4. 字符串拼接用二元折叠: (std::string{} + ... + args)");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 用折叠表达式实现 max_of(Args... args)，返回所有参数中的最大值。
//    提示: 二元左折叠 (0 + ... + args) 的模式 → (max(a, b)) 的思路。
//    进阶：考虑空参数包的情况（提示：std::common_type_t）。
// 2. 用折叠表达式实现 push_to_map(map, key1, val1, key2, val2, ...)。
//    每次取两个参数：map[key] = val。提示：逗号折叠。
// 3. 验证：左折叠 ( ... - args ) 和右折叠 ( args - ... ) 对 {1,2,3,4} 的结果。
//    解释为什么不同（提示：画出括号的分组方式）。
