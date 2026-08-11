#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  variadic_template.hpp — 变参模板 (Variadic Template)                       ║
// ║  可复用组件: show_arg_count, recursive_print, sum, concat,                  ║
// ║              print_all, print_all_fold                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <sstream>
#include <utility>

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 参数包基础
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】typename... Args 表示"零个或多个类型"的集合，称为"模板参数包"。
//          Args... args 表示"零个或多个函数参数"，称为"函数参数包"。
// 【为什么】如果没有变参模板，std::make_shared、std::tuple 等都需要无数的重载版本。
// 【什么时候用】封装任意数量、任意类型的参数——工厂函数、tuple、variant、转发等。

// 最简变参模板：打印任意数量参数的类型信息
template <typename... Args>
void show_arg_count(Args... args) {
    std::println("参数个数: {}", sizeof...(args));
    std::println("类型个数: {}", sizeof...(Args));
    // sizeof...(args) 和 sizeof...(Args) 结果总是一样，前者是语法糖
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 递归展开 — 参数包的最经典处理方式
// ═══════════════════════════════════════════════════════════════════════════════
// 【原理】每次取出第一个参数处理，剩下的参数递归调用自身。
//        递归终止条件：无参数的版本（参数包为空）。
// 【模式】三步走: (1) 定义终止函数 (2) 定义递归函数 (3) 在递归函数中调用自身

// 递归终止：没有参数时什么都不做
inline void recursive_print() {
    std::println("── 递归终止 ──");
}

// 递归步骤：打印第一个参数，剩余参数递归处理
template <typename T, typename... Rest>
void recursive_print(T first, Rest... rest) {
    std::print("{} ", first);
    // 展开：recursive_print(rest...) → 再次匹配这个模板或终止版本
    if constexpr (sizeof...(rest) > 0) {
        recursive_print(rest...);
    } else {
        recursive_print();  // 调用终止版本
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 递归展开高级用法 — 求和、连接
// ═══════════════════════════════════════════════════════════════════════════════

// 求和（递归）
template <typename T>
T sum(T v) {
    return v;                        // 终止：只有一个值
}

template <typename T, typename... Rest>
T sum(T first, Rest... rest) {
    return first + sum(rest...);     // 递归：头部 + 尾部之和
}

// 字符串连接
template <typename... Args>
std::string concat(Args&&... args) {
    std::ostringstream oss;
    // 逗号展开技巧（见 Part 4 详述）
    ((oss << std::forward<Args>(args)), ...);
    return oss.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 逗号展开技巧 — 不用递归就能展开参数包
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】利用初始化列表或折叠表达式，一次性消费整个参数包。
// 【为什么】递归展开有 N 层函数调用开销（虽然大多数被内联），逗号展开更直接。
// 【核心技巧】
//   (1) 初始化列表展开: int dummy[] = { (处理(args), 0)... };
//   (2) C++17 折叠表达式: (处理(args), ...); ← 最推荐（详见 05_fold_expressions）

// 逗号展开示例：将参数包中的每个元素操作一遍
template <typename... Args>
void print_all(Args&&... args) {
    // (std::println("{}", args), 0) 是一个逗号表达式，值为 0
    // ... 展开为初始化列表的每个元素
    // 相当于: int dummy[] = { (println("a"), 0), (println("b"), 0), ... };
    int dummy[] = { (std::println("  -> {}", std::forward<Args>(args)), 0)... };
    (void)dummy;  // 抑制未使用变量警告
}

// 更现代的写法：折叠表达式（C++17，详见 05_fold_expressions 课程）
template <typename... Args>
void print_all_fold(Args&&... args) {
    ((std::println("  >> {}", std::forward<Args>(args))), ...);
    // (expr, ...) = 一元右折叠，详见下一课
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 参数包必须放在模板参数列表的最后（如果有多个参数）
// 2. sizeof...(Args) 和 sizeof...(args) 是编译期常量
// 3. 递归展开需要有终止条件（无参或单参版本）
// 4. 递归展开会产生多份函数实例，参数太多时编译慢
