// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_fundamentals 示例 — 函数声明/定义、重载、默认参数、inline、[[nodiscard]]  ║
// ║  目标: 掌握 C++ 函数的各种声明方式、重载规则、默认参数和现代属性             ║
// ║  前置: 02_const_and_static, 04_pointers_and_refs                             ║
// ║  对应库头文件: fundamentals/functions_basics.hpp                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <string_view>
#include <cmath>        // std::abs (用于默认参数示例)
#include "shared/lesson_utils.hpp"
#include "fundamentals/functions_basics.hpp"

int main() {
    lesson::print_header("05 函数声明/定义、重载、默认参数、inline、[[nodiscard]]");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 1: 函数声明 vs 定义 — 分离接口与实现
    // ═══════════════════════════════════════════════════════════════════════════════
    // 声明 (declaration): 告诉编译器"存在这样一个函数"，通常在头文件中。
    // 定义 (definition):   提供函数体 { ... }，通常在 .cpp 文件中。
    // 为什么分离: 1) 编译加速 (改实现不需要重编译所有使用者)
    //             2) 接口与实现分离 (信息隐藏)
    //             3) 解决循环依赖
    // 在当前的工程化布局中:
    //   - 声明在 include/fundamentals/functions_basics.hpp
    //   - 实现在 src/functions_basics.cpp
    //   - 你正在写的这个文件 (examples) 使用这些库函数
    lesson::print_subtitle("Part 1: 声明 vs 定义");

    // 这些函数在 fundamentals 库中声明和定义
    fundamentals::print_info(42);          // 调用重载1
    fundamentals::print_info(3.14159);     // 调用重载2

    std::println("(这些函数声明在 functions_basics.hpp 中，实现在 src/functions_basics.cpp 中)");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 2: 函数重载 (Overloading) — 同名函数，参数不同
    // ═══════════════════════════════════════════════════════════════════════════════
    // 重载是什么: 同一作用域内允许多个同名函数，只要参数列表不同。
    // 为什么存在: 避免为不同参数类型起不同的名字 (如 print_int, print_double...)
    // 重载规则:
    //   1) 参数数量不同 → 重载
    //   2) 参数类型不同 → 重载
    //   3) 仅返回值类型不同 → ❌ 不是重载! 编译错误!
    //   4) const 成员函数 vs 非 const → 重载 (对成员函数)
    // 编译器通过"重载决议 (overload resolution)"选择最匹配的版本
    lesson::print_subtitle("Part 2: 函数重载");

    fundamentals::print_info(100);                         // 匹配 print_info(int)
    fundamentals::print_info(2.71828);                     // 匹配 print_info(double)
    fundamentals::print_info(std::string_view{"C++23"});   // 匹配 print_info(string_view)
    // fundamentals::print_info("hello");                  // ❌ 不明确: const char* 可以转 int/double/string_view
    // 需要显式指定: fundamentals::print_info(std::string_view{"hello"});

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 3: 默认参数 — 调用者可以省略的参数
    // ═══════════════════════════════════════════════════════════════════════════════
    // 默认参数是什么: 在声明中为参数提供默认值，调用时如果不传就用默认值。
    // 为什么存在: 减少重载数量、向后兼容 (新参数不影响旧调用)、简化常见用法。
    // 规则:
    //   - 默认参数必须从右往左连续 — 不能跳过前面的
    //   - 默认值只写在声明中 (定义中不重复)
    //   - 默认参数会影响重载决议
    lesson::print_subtitle("Part 3: 默认参数");

    // 使用 lambda 演示默认参数 (lambda 是本地定义，适合这里演示)
    auto greet = [](std::string_view name = "艾克斯", int times = 1) {
        for (int i = 0; i < times; ++i) {
            std::println("你好, {}!", name);
        }
    };

    greet();                          // 两个参数都用默认值
    greet("小明");                     // times 用默认值 1
    greet("小红", 3);                  // 全部显式指定

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 4: inline 函数 — 零开销抽象
    // ═══════════════════════════════════════════════════════════════════════════════
    // inline 是什么: 建议编译器把函数体直接嵌入调用处 (不生成 call 指令)。
    // 为什么存在: 消除小函数的调用开销 (压栈/跳转/返回)。
    // 现代编译器会自动内联，inline 关键字的主要作用是:
    //   允许函数定义出现在头文件中 (多个翻译单元包含也不会产生重复定义错误)
    // 什么时候用: header-only 库中的小函数、频繁调用的 getter/setter
    // 什么时候不用: 大函数 (代码膨胀)、递归函数、虚函数
    lesson::print_subtitle("Part 4: inline 函数");

    std::println("fast_max(10, 20) = {}", fundamentals::fast_max(10, 20));
    // 在调试器中，你可能看不到 fast_max 的调用帧
    // (取决于编译器是否接受 inline 建议)

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 5: [[nodiscard]] — "忽略我就警告你" (C++17)
    // ═══════════════════════════════════════════════════════════════════════════════
    // [[nodiscard]] 是什么: 属性 (attribute)，标记函数返回值"不应被丢弃"。
    // 为什么存在: 防止调用者忽略重要返回值 (如错误码、计算结果)。
    //             不能通过代码逻辑强制，但编译器会给你一个黄色波浪线。
    // 常见应用: 返回错误码的函数、纯计算函数 (忽略返回值意味着无意义的调用)
    lesson::print_subtitle("Part 5: [[nodiscard]]");

    int important = fundamentals::compute_important_value();  // ✅ 使用了返回值
    std::println("compute_important_value() = {}", important);

    bool valid = fundamentals::validate_input(10);             // ✅ 使用了返回值
    std::println("validate_input(10) = {}", valid);

    // fundamentals::compute_important_value();  // ⚠️ 编译器警告: 忽略了 [[nodiscard]] 返回值
    // 如果你真的想忽略, 可以: (void)fundamentals::compute_important_value();
    // 或者 C++17: std::ignore = fundamentals::compute_important_value();

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 6: 尾置返回类型 — auto + -> 的语法
    // ═══════════════════════════════════════════════════════════════════════════════
    // 尾置返回类型是什么: 把返回类型写在参数列表后面 (-> 引导)。
    // 为什么存在:
    //   1) 返回类型依赖于参数类型时 (模板/泛型)
    //   2) C++11 lambda 表达式中必须用 (C++14 起可省略)
    //   3) 有人觉得这种写法更一致: 函数名在左边, 一目了然
    lesson::print_subtitle("Part 6: 尾置返回类型");

    auto r1 = fundamentals::multiply(3, 4);        // int * int → int
    auto r2 = fundamentals::multiply(3.0, 4.0);    // double * double → double
    auto r3 = fundamentals::multiply(3, 4.5);      // int * double → double
    std::println("multiply(3,4)   = {}", r1);
    std::println("multiply(3.0,4.0) = {}", r2);
    std::println("multiply(3,4.5)   = {}", r3);

    // ── 简单场景也可以用尾置返回类型 ──
    auto add_vals = [](int a, int b) -> int { return a + b; };
    std::println("lambda add: {} + {} = {}", 5, 7, add_vals(5, 7));

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 7: constexpr 函数 — 编译期可执行
    // ═══════════════════════════════════════════════════════════════════════════════
    // 02_const_and_static 中已详细讲解，这里作为函数视角的复习
    lesson::print_subtitle("Part 7: constexpr 函数");

    constexpr int fact5 = fundamentals::factorial(5);   // 编译期计算 5!
    std::println("factorial(5) = {} (编译期计算)", fact5);
    static_assert(fundamentals::factorial(5) == 120, "编译期验证 factorial(5)");

    // ── 常见陷阱 (Common Pitfalls) ────────────────────────────────────────────
    lesson::print_separator("常见陷阱");
    lesson::print_warn("陷阱1: 仅返回值不同不是重载 — int f() 和 double f() 不能共存");
    lesson::print_warn("陷阱2: 默认参数必须从右往左 — void f(int a, int b=1, int c) 编译错误!");
    lesson::print_warn("陷阱3: 默认参数在声明和定义中都出现 — 推荐只在声明中写");
    lesson::print_warn("陷阱4: 函数重载 + 默认参数 → 二义性调用 (编译器不知道选哪个)");
    lesson::print_warn("陷阱5: inline 不是魔法 — 大函数内联会增大二进制体积 (代码膨胀)");
    lesson::print_warn("陷阱6: [[nodiscard]] 可以被显式忽略 — 需要代码审查配合");
    lesson::print_warn("陷阱7: 重载决议优先匹配非模板函数 (隐藏规则: 模板是最后的备选)");

    // ── 练习 ─────────────────────────────────────────────────────────────────
    lesson::print_separator("练习");
    std::println("1. 写 3 个重载的 max 函数: int, double, string_view");
    std::println("2. 写一个带默认参数的 greet 函数: void greet(string_view name, int times=1, string_view prefix=\"你好\")");
    std::println("3. 给除法函数标记 [[nodiscard]]，实验忽略返回值时的编译器警告");
    std::println("4. 用尾置返回类型实现模板函数 min(T, U)，返回类型用 decltype");
    std::println("5. 解释: 为什么 constexpr 函数不是隐式 [[nodiscard]]?");

    return 0;
}
