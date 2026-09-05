#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  type_traits_basics.hpp — <type_traits> 基础与 SFINAE                       ║
// ║  可复用组件: demonstrate_type_traits (声明), describe_sfinae (SFINAE),      ║
// ║              has_iterator (void_t), describe_if_constexpr (if constexpr)    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <type_traits>

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: <type_traits> 常用工具速览
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】<type_traits> 提供编译期类型查询和类型转换的模板。
//           分三大类：类型判断(_v)、类型转换(_t)、属性查询。
// 【为什么】在模板编程中，你经常需要知道"T 是不是整数？"，"去掉 const 后的类型是啥？"。
//          这些答案必须在编译期获得，运行时类型信息(RTTI)不够。
// 【速查表】
//   判断类: is_integral_v, is_floating_point_v, is_same_v<A,B>, is_base_of_v<A,B>
//           is_pointer_v, is_reference_v, is_const_v, is_class_v, is_enum_v
//           is_arithmetic_v, is_void_v, is_array_v, is_function_v
//   转换类: remove_const_t, remove_reference_t, remove_pointer_t
//           add_const_t, add_pointer_t, decay_t, common_type_t
//           underlying_type_t (枚举的底层类型), conditional_t<B,T,F>
//   属性类: alignment_of_v, rank_v (数组维度), extent_v (数组长度)

// 声明（实现在 src/type_traits_basics.cpp）
void demonstrate_type_traits();

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: SFINAE 与 std::enable_if
// ═══════════════════════════════════════════════════════════════════════════════
// 【SFINAE = Substitution Failure Is Not An Error】
// 【是什么】模板参数替换失败时，编译器不报错，只是把该重载从候选集中移除。
// 【为什么】允许你为不同类型提供不同的模板重载，编译器自动选最匹配的。
//          这是 C++17 之前实现"类型约束" 的主要手段。
// 【enable_if 原理】
//   enable_if<true, T>::type  → T        (条件成立，启用)
//   enable_if<false, T>::type → 替换失败  (条件不成立，SFINAE 移除)

// 传统 SFINAE 写法：只有整数类型才启用这个重载
template <typename T>
std::enable_if_t<std::is_integral_v<T>, std::string>
describe_sfinae(T val) {
    return std::format("整数: {}", val);
}

// 只有浮点类型才启用这个重载
template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::string>
describe_sfinae(T val) {
    return std::format("浮点: {:.2f}", val);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: void_t 技巧 — 检测类型是否拥有某个成员
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】void_t 是 C++17 引入的一个简单别名：
//          template<typename...> using void_t = void;
// 【技巧】利用 SFINAE：如果把 "T::member" 放在 void_t<decltype(T::member)> 中，
//        如果 T 没有 member，替换失败，走 fallback 版本。
// 【用途】编译期检测类型是否有特定成员函数、类型别名等。

// 通用版本（默认）
template <typename, typename = void>
struct has_iterator : std::false_type {};

// 特化版本：如果 T 有 iterator 类型别名，匹配这个
template <typename T>
struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};

template <typename T>
constexpr bool has_iterator_v = has_iterator<T>::value;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: if constexpr — C++17 更简洁的替代方案
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】编译期条件分支。条件为 false 的分支不会被编译。
// 【为什么】传统 SFINAE + enable_if 需要写多个重载，代码分散、可读性差。
//          if constexpr 把条件逻辑写在一个函数体里，清晰直观。
// 【对比】
//   SFINAE:  多个函数重载，每个用 enable_if 筛选
//   if constexpr: 一个函数体，用 if constexpr 分发
// 【局限】if constexpr 适合在函数体内部做分支；但某些场景仍需 SFINAE（类模板偏特化等）

template <typename T>
auto describe_if_constexpr(T val) {
    if constexpr (std::is_integral_v<T>) {
        return std::format("整数: {}", val);
    } else if constexpr (std::is_floating_point_v<T>) {
        return std::format("浮点: {:.2f}", val);
    } else if constexpr (std::is_same_v<T, std::string> ||
                         std::is_same_v<T, const char*>) {
        return std::format("字符串: \"{}\"", val);
    } else {
        return std::format("其他类型 (类型名: {})", typeid(T).name());
    }
}
