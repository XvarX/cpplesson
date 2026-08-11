#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: Concepts 进阶 — requires 表达式、concept 组合、约束偏序               ║
// ║  目标: 掌握四种 requires 表达式、concept 的逻辑组合、理解约束偏序规则        ║
// ║                                                                            ║
// ║  本文件提供:                                                                 ║
// ║    - 四种 requires 表达式的示例 concept                                       ║
// ║    - concept 的逻辑组合 (&& / ||)                                            ║
// ║    - 约束偏序的 describe() 重载                                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <concepts>
#include <functional>
#include <ostream>
#include <type_traits>

// ============================================================================
// Part 1: requires 表达式的四种形式
// ============================================================================
// requires 表达式是 concept 定义的核心工具，有四种形式:
//   ① 简单 requires  — 验证表达式是否合法
//   ② 类型 requires  — 验证某个类型是否存在
//   ③ 复合 requires  — 验证表达式合法且返回类型满足约束
//   ④ 嵌套 requires  — 在 requires 表达式内部再使用 requires 约束

// ── ① 简单 requires + ③ 复合 requires: 检查表达式能否编译且返回类型满足约束 ──
template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;  // ③ 复合 requires
};

// ── ② 类型 requires: 检查类型是否存在 ──
template<typename T>
concept HasValueType = requires {
    typename T::value_type;   // ② 类型 requires — 验证嵌套类型存在
};

// ── ④ 嵌套 requires: 对模板参数进一步约束 ──
template<typename T>
concept AddableContainer = requires(T a, T b) {
    typename T::value_type;
    a.size();
    requires std::same_as<typename T::value_type, decltype(a[0])>;  // ④ 嵌套 requires
};

// ============================================================================
// Part 2: concept 的逻辑组合 — && 和 ||
// ============================================================================
// concept 支持布尔逻辑组合，便于构建更精确的约束:
//   - concept1 && concept2  →  同时满足两个约束
//   - concept1 || concept2  →  满足其中之一即可

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 可打印的数值类型: 既是 Numeric，又支持 << 输出
template<typename T>
concept PrintableNumeric = Numeric<T> && requires(T val, std::ostream& os) {
    { os << val } -> std::convertible_to<std::ostream&>;
};

// ============================================================================
// Part 3: 约束的偏序 (Constraint Partial Ordering)
// ============================================================================
// 当多个重载都满足约束时，编译器选择"更严格"的那个（约束更强的版本）。
// 规则: 约束 A 比约束 B 更严格，当且仅当 A 蕴含 B（即满足 A 必然满足 B）。

// 通用版本 — 约束较弱
template<typename T>
    requires std::integral<T>
auto describe(T v) -> const char*;

// 特化版本 — 约束更强 (unsigned 是 integral 的子集)
template<typename T>
    requires std::integral<T> && std::unsigned_integral<T>
auto describe(T v) -> const char*;

// ============================================================================
// Part 4: requires 与 enable_if 的对比
// ============================================================================
// C++17 时代用 std::enable_if 实现 SFINAE，语法晦涩，错误信息恐怖。
// C++20 concept 直接表达意图，编译器给出清晰的约束失败信息。
//
// | 特性         | enable_if (C++17)        | concept (C++20)            |
// |--------------|--------------------------|----------------------------|
// | 可读性       | 模板元编程，难读          | 声明式，一目了然            |
// | 错误信息     | 模板实例化失败，几百行    | "约束未满足"，简洁明了       |
// | 重载选择     | 需要 tag dispatch         | 自动偏序，编译器选最优       |
// | IDE 支持     | 弱                       | 强，可即时提示               |

// ── describe() 的实现 ──
template<typename T>
    requires std::integral<T>
auto describe(T /*v*/) -> const char* { return "整数"; }

template<typename T>
    requires std::integral<T> && std::unsigned_integral<T>
auto describe(T /*v*/) -> const char* { return "无符号整数"; }
