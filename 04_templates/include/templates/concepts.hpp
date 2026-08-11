#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concepts.hpp — C++20 Concepts (概念)                                       ║
// ║  可复用组件: integral_double, integral_triple,                               ║
// ║              concept Doublable/Numeric/HasSize/Iterable/Printable,           ║
// ║              generic_square, print_size_info,                                ║
// ║              traditional_max, abbreviated_max, multi_constrained             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <concepts>
#include <type_traits>
#include <ostream>

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: C++20 Concept 概述
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】concept 是"类型的谓词"——一组编译期约束条件。
//          类似于：声明"模板参数 T 必须是可加的"、"必须是整数" 等。
// 【为什么】传统模板报错像天书（几页模板展开错误），concept 在"入口处"就拦截，
//          给出清晰简短的错误信息，类似"不满足 constraint: integral"。
// 【对比三种约束方式】
//   1. SFINAE + enable_if (C++11/14): 语法晦涩, 报错混乱
//   2. if constexpr (C++17):         只能在函数内部, 外部接口不清晰
//   3. concept (C++20):              ✅ 意图明确, 报错友好, 语法简洁
// 【什么时候用】任何能替代 enable_if 的场景, 尤其是公开 API 的接口约束。

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 标准库预定义 concept
// ═══════════════════════════════════════════════════════════════════════════════
// C++20 在 <concepts> 中提供了一批常用 concept，省去自己写的麻烦：
//   核心概念: same_as<T,U>, derived_from<D,B>, convertible_to<From,To>,
//            common_with<T,U>, assignable_from<L,R>, swappable<T>
//   算术概念: integral<T>, signed_integral<T>, unsigned_integral<T>,
//            floating_point<T>
//   对象概念: movable<T>, copyable<T>, semiregular<T>, regular<T>
//   调用概念: invocable<F,Args...>, predicate<F,Args...>

// 用法一：requires 子句（在模板参数列表后）
template <typename T>
    requires std::integral<T>   // ← 约束：T 必须是整数类型
T integral_double(T val) {
    return val * 2;
}

// 用法二：在模板参数列表中使用 concept
template <std::integral T>
T integral_triple(T val) {
    return val * 3;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 自定义 concept
// ═══════════════════════════════════════════════════════════════════════════════

// 3.1 简单 concept: 要求类型支持 * 2 运算
template <typename T>
concept Doublable = requires(T x) {
    { x * 2 } -> std::convertible_to<T>;  // x * 2 的结果可以转换为 T
};

// 3.2 组合 concept: 使用已有的 concept 拼出新 concept
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 3.3 检测成员函数的 concept: 要求类型有 .size() 返回整数
template <typename T>
concept HasSize = requires(const T& t) {
    { t.size() } -> std::convertible_to<std::size_t>;
};

// 3.4 检测迭代器的 concept
template <typename T>
concept Iterable = requires(T& t) {
    typename T::iterator;          // 有 iterator 类型别名
    t.begin();                     // 有 begin()
    t.end();                       // 有 end()
};

// 使用自定义 concept
template <Numeric T>
T generic_square(T val) {
    return val * val;
}

template <HasSize T>
void print_size_info(const T& container) {
    std::println("容器大小: {}", container.size());
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: requires 表达式 深入
// ═══════════════════════════════════════════════════════════════════════════════
// requires 表达式有四种要求 (requirement)：
//   1. 简单要求: requires(T x) { x + x; }        — 表达式合法即可
//   2. 类型要求: requires { typename T::value_type; }  — 存在该类型
//   3. 复合要求: { expr } -> concept;             — 表达式合法 + 返回类型满足 concept
//   4. 嵌套要求: requires { requires sizeof(T) > 4; } — 任意编译期常量表达式

// 综合示例
template <typename T>
concept Printable = requires(const T& val, std::ostream& os) {
    // 简单要求：表达式合法
    { os << val };
    // 简单要求：类型有 value_type（对于非容器，替换失败，SFINAE 兼容）
};

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: 简写语法 (Abbreviated Function Templates)
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】用 auto 加 concept 约束代替 template<typename> 声明。
// 【为什么】减少语法噪音，让接口更简洁易读。

// 传统写法
template <std::integral T>
T traditional_max(T a, T b) { return a > b ? a : b; }

// 简写写法（完全等价）
auto abbreviated_max(std::integral auto a, std::integral auto b) {
    return a > b ? a : b;
}

// 多个参数可以有不同的约束
void multi_constrained(std::integral auto n, std::floating_point auto x,
                       std::convertible_to<std::string> auto s) {
    std::println("整数: {}, 浮点: {}, 可转字符串: {}", n, x, s);
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. concept 约束的是接口，不是实现细节
// 2. requires 表达式中的代码不会真正执行，只检查合法性
// 3. 简写语法的每个 auto 都是独立推导，类型可以不同！
// 4. concept 不能递归引用自己（concept A 依赖 concept B，B 又依赖 A）
// 5. SFINAE 仍然需要理解：大量旧代码和库还在用，concept 是新标准
