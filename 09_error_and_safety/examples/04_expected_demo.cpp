// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 09_error_and_safety — 错误处理与安全                                 ║
// ║  主题: 04_expected — std::expected<T,E> (C++23)                             ║
// ║  目标: 理解 std::expected、and_then/or_else/transform 单子操作、           ║
// ║        与异常/optional 的比较与选择                                        ║
// ║                                                                             ║
// ║  跟着敲: 掌握 C++23 函数式错误处理的强大能力                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <expected>      // C++23: std::expected
#include <optional>      // 对比用
#include <system_error>
#include <cmath>
#include <algorithm>
#include "error_safety/expected_utils.hpp"
#include "shared/lesson_utils.hpp"

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::expected<T,E> — 要么值，要么错误的类型                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part1_expected_basics() {
    lesson::print_separator("Part 1: std::expected 基础");

    // ── 构造与检查 ──
    // has_value() → 是否有值
    // operator bool() → 同上
    // operator* 或 value() → 获取值
    // error() → 获取错误

    auto result1 = parsePositiveInt("42");
    if (result1) {
        std::println("  ✅ 解析成功: {}", *result1);
        // 也可以: result1.value() (有值则返回，无值则抛 bad_expected_access)
    }

    auto result2 = parsePositiveInt("12abc34");
    if (!result2) {
        std::println("  ❌ 解析失败: {}", parseErrorMsg(result2.error()));
    }

    // 可以用 value_or 提供默认值
    auto result3 = parsePositiveInt("");
    int safeVal = result3.value_or(-1);  // 如果是错误，返回 -1
    std::println("  value_or 默认值: {}", safeVal);

    // ── 错误时的安全访问 ──
    auto result4 = queryUserName(42);
    // transform 配合打印: 只有成功时才执行 lambda
    auto name = result4.transform([](const std::string& n) {
        return "👤 " + n;
    });
    if (name) {
        std::println("  查询结果: {}", *name);
    }

    auto result5 = queryUserName(9999);  // 无效 ID
    // 可以直接检查错误
    if (!result5) {
        std::println("  查询失败: {}", result5.error().message());
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 单子操作 (Monadic Operations)                                     ║
// ║                                                                             ║
// ║  C++23 的 std::expected 提供了函数式风格的方法:                             ║
// ║                                                                             ║
// ║  .and_then(f)   — 如果有值，调用 f(值) 返回新的 expected；有错误则短路  ║
// ║  .or_else(f)    — 如果有错误，调用 f(错误) 返回新的 expected；有值则短路 ║
// ║  .transform(f)  — 如果有值，调用 f(值) 并包装结果；有错误则短路          ║
// ║  .transform_error(f) — 如果有错误，调用 f(错误) 并包装结果；有值则短路   ║
// ║  .value_or(v)   — 解包: 有值返回之，否则返回 v                           ║
// ║  .error_or(e)   — 解包: 有错误返回之，否则返回 e                         ║
// ║                                                                             ║
// ║  这些方法让你可以"链式"处理可能失败的操作，避免嵌套的 if-else。           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part2_monadic_operations() {
    lesson::print_separator("Part 2: 单子操作 — and_then / or_else / transform");

    // ── 示例1: and_then 链式处理 ──
    // 流水线: 字符串 → 解析 → 范围检查 → 加倍
    // 任何一步失败都会短路，错误直接传递到最后

    auto pipeline = [](std::string_view s) -> std::expected<int, ParseError> {
        return parsePositiveInt(s)             // 步骤1: 解析
            .and_then([](int v) {              // 步骤2: 范围检查
                return clampToRange(v, 1, 100);
            })
            .and_then([](int v) {              // 步骤3: 加倍
                return doubleIt(v);
            });
    };

    // 成功路径
    auto r1 = pipeline("42");
    if (r1) {
        std::println("  流水线(\"42\"): 成功 → 值 = {}", *r1);
    }

    // 中间失败: 解析失败
    auto r2 = pipeline("abc");
    if (!r2) {
        std::println("  流水线(\"abc\"): 失败 → {}", parseErrorMsg(r2.error()));
    }

    // 中间失败: 范围检查失败 (超出上限)
    auto r3 = pipeline("999");
    if (!r3) {
        std::println("  流水线(\"999\"): 失败 → {}", parseErrorMsg(r3.error()));
    }

    // ── 示例2: transform 用于值转换 ──
    auto r4 = parsePositiveInt("7")
        .transform([](int v) { return v * v; })       // 平方
        .transform([](int v) { return v + 1; });      // 再加1
    // 注意: transform 保持原错误类型 E 不变
    if (r4) {
        std::println("  transform 链: 7 → 7² → 49+1 = {}", *r4);
    }

    // ── 示例3: or_else 用于错误恢复 ──
    // 如果第一步失败，尝试替代方案
    auto r5 = parsePositiveInt("")  // 空输入 → 失败
        .or_else([](ParseError e) -> std::expected<int, ParseError> {
            // 错误恢复策略: 空输入默认为 0
            std::println("    [恢复] 空输入，使用默认值 0");
            return 0;
        })
        .transform([](int v) { return v + 100; });
    if (r5) {
        std::println("  or_else 恢复: {} + 100 = {}", 0, *r5);
    }

    // ── 示例4: transform_error 用于改进错误信息 ──
    auto r6 = parsePositiveInt("-5")
        .transform_error([](ParseError e) {
            // 将通用的 InvalidChar 转为更具体的错误
            if (e == ParseError::InvalidChar) {
                return ParseError::NegativeNotAllowed;
            }
            return e;
        });
    if (!r6) {
        std::println("  transform_error: {}", parseErrorMsg(r6.error()));
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: expected vs optional vs 异常 — 如何选择                           ║
// ║                                                                             ║
// ║  ┌───────────────────┬───────────┬─────────────┬───────────────┐           ║
// ║  │ 场景               │ optional  │ expected    │ 异常          │           ║
// ║  ├───────────────────┼───────────┼─────────────┼───────────────┤           ║
// ║  │ 可能没有值          │ ✅ 首选   │ ✅ 可用     │ ❌ 过重       │           ║
// ║  │ 需要失败原因        │ ❌        │ ✅ 首选     │ ✅ 可用       │           ║
// ║  │ 构造函数失败        │ ❌        │ ❌          │ ✅ 唯一选择   │           ║
// ║  │ 性能敏感的路径      │ ✅        │ ✅          │ ❌ 有开销     │           ║
// ║  │ 深层调用栈中的错误  │ ❌        │ ✅ (逐层传) │ ✅ (自动传)   │           ║
// ║  │ 不可恢复的错误      │ ❌        │ ❌          │ ✅ (terminate)│           ║
// ║  │ 函数式链式处理      │ ✅        │ ✅✅ 最佳   │ ❌ 不优雅     │           ║
// ║  └───────────────────┴───────────┴─────────────┴───────────────┘           ║
// ║                                                                             ║
// ║  经验法则:                                                                  ║
// ║  1. 只是"可能没有" → optional<T>                                          ║
// ║  2. "可能失败+需要原因" → expected<T,E>                                   ║
// ║  3. "意外错误+无法就地处理" → 异常                                        ║
// ║  4. "完全不应该失败" → 直接返回 T                                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part3_comparison() {
    lesson::print_separator("Part 3: expected vs optional vs 异常");

    // 演示三种风格在同一场景下的使用

    // optional 风格: 简洁，但无法区分"除零"和"溢出"
    auto opt = safeDivideOpt(10.0, 2.0);
    std::println("  [optional] 10/2 = {:.1f}", opt.value_or(-1.0));
    opt = safeDivideOpt(10.0, 0.0);
    std::println("  [optional] 10/0 = {}", opt ? std::format("{:.1f}", *opt) : "无值(原因未知)");

    // expected 风格: 能知道为什么失败
    auto exp = safeDivideExp(10.0, 2.0);
    std::println("  [expected] 10/2 = {:.1f}", exp.value_or(-1.0));
    exp = safeDivideExp(10.0, 0.0);
    if (!exp) {
        std::println("  [expected] 10/0 = 失败, 原因: {}",
            exp.error() == DivError::DivisionByZero ? "除零" : "溢出");
    }

    // 异常风格: 代码中不需要显式检查，但调用者必须 try-catch
    try {
        double r = safeDivideThrow(10.0, 5.0);
        std::println("  [异常] 10/5 = {:.1f}", r);
    } catch (const std::exception& e) {
        std::println("  [异常] 失败: {}", e.what());
    }

    try {
        double r = safeDivideThrow(1.0, 0.0);  // 这会抛异常
        std::println("  这行不会执行: {}", r);
    } catch (const std::domain_error& e) {
        std::println("  [异常] 捕获: {}", e.what());
    }

    // 总结: expected 的链式能力使错误处理显得优雅
    auto chain = safeDivideExp(100.0, 2.0)           // 100/2 = 50
        .and_then([](double v) {
            return safeDivideExp(v, 5.0);             // 50/5 = 10
        })
        .transform([](double v) { return v * 3; });  // 10*3 = 30

    if (chain) {
        std::println("  [expected 链] ((100/2)/5)*3 = {:.1f}", *chain);
    }

    // 链中的任何一步失败都会短路:
    auto failChain = safeDivideExp(100.0, 2.0)
        .and_then([](double v) {
            return safeDivideExp(v, 0.0);  // 除零! 后面的都不会执行
        })
        .transform([](double v) { return v * 3; });
    if (!failChain) {
        std::println("  [expected 链] 短路了: {}",
            failChain.error() == DivError::DivisionByZero ? "除零错误" : "其他错误");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                  ║
// ║                                                                             ║
// ║  1. 未检查 expected 就直接使用 value()                                     ║
// ║     → value() 在错误时抛 std::bad_expected_access，与使用 expected 的初衷  ║
// ║        (避免异常) 矛盾                                                      ║
// ║     正确方式: 总是先检查 has_value() 或使用 if(result)                    ║
// ║                                                                             ║
// ║  2. 滥用 and_then 导致深层嵌套难以调试                                      ║
// ║     → 链太长时，哪个 and_then 失败了不直观                                 ║
// ║     正确方式: 合理拆分，关键步骤加日志，或用 transform_error 改进错误信息 ║
// ║                                                                             ║
// ║  3. 用 expected 替代所有异常                                                ║
// ║     → 构造函数失败无法用 expected (构造函数的返回值就是对象本身)           ║
// ║     正确方式: 构造函数仍用异常，工厂函数可以用 expected                  ║
// ║                                                                             ║
// ║  4. expected<void,E> 的特殊性 (C++23)                                      ║
// ║     → expected<void,E> 是合法的，但没有 *value()，只有 void 的"成功"状态  ║
// ║     正确方式: 用于表示"操作成功与否，失败时有错误详情"                    ║
// ║                                                                             ║
// ║  5. E 类型的拷贝/移动开销                                                  ║
// ║     → 如果 E 很重 (如 std::string)，考虑用轻量级错误枚举 + 错误映射表    ║
// ║     正确方式: E 应该是轻量级类型 (int, enum, error_code)                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                      ║
// ║                                                                             ║
// ║  1. 实现一个 parseAndValidateConfig(jsonStr) 函数，返回                    ║
// ║     expected<Config, ConfigError>。使用 and_then 串联:                      ║
// ║     parseJson → validateFields → buildConfig                               ║
// ║     每步定义各自的错误枚举。                                                ║
// ║                                                                             ║
// ║  2. 对比实现同一个"安全整数除法"用三种方式:                                ║
// ║     - optional<int> (丢失除零之外的错误信息)                               ║
// ║     - expected<int, DivError> (推荐)                                      ║
// ║     - 抛出异常                                                             ║
// ║     写出它们的优缺点。                                                      ║
// ║                                                                             ║
// ║  3. 实现一个数据处理器:                                                    ║
// ║     rawData → validate(data) → normalize(data) → process(data)               ║
// ║     用 expected + and_then 串联，每一步都可能失败且有不同错误类型。         ║
// ║     提示: 如果错误类型不同，需要用 transform_error 统一。                  ║
// ║                                                                             ║
// ║  4. 用 expected<void, std::error_code> 实现一个"写入文件"操作:             ║
// ║     函数签名: expected<void, std::error_code>                               ║
// ║     成功时无返回值，失败时返回错误码。                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

int main() {
    lesson::print_header("04_expected — std::expected<T,E> (C++23)");

    part1_expected_basics();
    part2_monadic_operations();
    part3_comparison();

    std::println("\n✅ std::expected 学习完成!");
    return 0;
}
