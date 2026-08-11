// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  error_safety/expected_utils.hpp — std::expected<T,E> (C++23)               ║
// ║  可复用的类型: ParseError, DivError 枚举                                     ║
// ║  可复用的函数: parsePositiveInt, parseErrorMsg, queryUserName,              ║
// ║               clampToRange, doubleIt, safeDivideOpt, safeDivideExp,          ║
// ║               safeDivideThrow                                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <expected>      // C++23: std::expected
#include <optional>      // 对比用
#include <string>
#include <string_view>
#include <system_error>
#include <cmath>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  ParseError — 解析错误枚举（用于 expected<int, ParseError>）               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
enum class ParseError {
    EmptyInput,
    InvalidChar,
    Overflow,
    NegativeNotAllowed,
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  parsePositiveInt — 安全解析正整数，返回 expected<int, ParseError>         ║
// ║                                                                             ║
// ║  std::expected<T,E> 是什么？                                                ║
// ║  - C++23 引入的和类型 (sum type): 要么持有预期值 T，要么持有错误值 E     ║
// ║  - 类似于 Result<T,E> (Rust/Haskell)，是函数式错误处理的核心类型         ║
// ║  - 不涉及动态内存分配 (值语义，栈上存储)                                  ║
// ║                                                                             ║
// ║  为什么需要 expected？                                                      ║
// ║  1. 异常: 有运行时开销，不适合预期中会失败的场景                          ║
// ║  2. optional: 只能表示"有值/无值"，丢失了错误原因                         ║
// ║  3. error_code + 输出参数: 将正常返回和错误返回分到两个渠道，容易忽略错误 ║
// ║  4. expected: 强制调用者检查，且携带错误详情                             ║
// ║                                                                             ║
// ║  何时使用 expected？                                                        ║
// ║  - 函数可能失败且失败原因很重要 (比 optional 信息更多)                    ║
// ║  - 不想用异常 (性能、编码规范、或异常被禁用)                              ║
// ║  - 需要函数式的错误处理链 (and_then, or_else, transform)                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
std::expected<int, ParseError> parsePositiveInt(std::string_view s);

/// 终端用户友好的错误消息
std::string_view parseErrorMsg(ParseError e);

/// 模拟数据库查询 — 返回 expected<string, error_code>
std::expected<std::string, std::error_code> queryUserName(int userId);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  单子操作 (Monadic Operations) 辅助函数                                     ║
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

/// 检查数值是否在指定范围内
std::expected<int, ParseError> clampToRange(int v, int min, int max);

/// 将一个正整数加倍（检测溢出）
std::expected<int, ParseError> doubleIt(int v);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  DivError — 除法错误枚举                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
enum class DivError { DivisionByZero, Overflow };

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  三种不同风格的安全除法                                                    ║
// ║                                                                             ║
// ║  经验法则:                                                                  ║
// ║  1. 只是"可能没有" → optional<T>                                          ║
// ║  2. "可能失败+需要原因" → expected<T,E>                                   ║
// ║  3. "意外错误+无法就地处理" → 异常                                        ║
// ║  4. "完全不应该失败" → 直接返回 T                                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

/// 风格1: optional — "可能没有值"
std::optional<double> safeDivideOpt(double a, double b);

/// 风格2: expected — "可能失败，需要知道原因"
std::expected<double, DivError> safeDivideExp(double a, double b);

/// 风格3: 异常 — "除零是意外情况，不应发生"
double safeDivideThrow(double a, double b);
