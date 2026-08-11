#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  fold_expressions.hpp — 折叠表达式 (Fold Expressions, C++17)                ║
// ║  可复用组件: left_sum, right_sum, all_true, any_true, print_all_fold,      ║
// ║              safe_sum, safe_product, join_strings, push_all_old,            ║
// ║              push_all_new, formatted_print                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <vector>
#include <utility>

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 折叠表达式概述
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】对参数包的所有参数，用一个二元运算符"折叠"成单一结果。
// 【为什么】以前需要用递归展开参数包（代码冗长，编译慢），折叠表达式一行搞定。
// 【分类】
//   - 一元折叠: ( ... op pack ) 或 ( pack op ... )
//   - 二元折叠: ( init op ... op pack ) 或 ( pack op ... op init )
// 【什么时候用】任何需要对变参模板做聚合运算的场景——求和、逻辑判断、逗号序列等。

// 四种折叠形式的命名规则（以 + 为例）：
//   ( ... + args )    → 一元左折叠   = ((a1 + a2) + a3) + a4
//   ( args + ... )    → 一元右折叠   = a1 + (a2 + (a3 + a4))
//   ( 0 + ... + args )→ 二元左折叠   = (((0 + a1) + a2) + a3) + a4
//   ( args + ... + 0 )→ 二元右折叠   = a1 + (a2 + (a3 + (a4 + 0)))

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 一元折叠 — 四种常用场景
// ═══════════════════════════════════════════════════════════════════════════════

// 2.1 一元左折叠：求和
template <typename... Args>
auto left_sum(Args... args) {
    return (... + args);  // ((a1 + a2) + a3) + a4
}

// 2.2 一元右折叠：求和
template <typename... Args>
auto right_sum(Args... args) {
    return (args + ...);  // a1 + (a2 + (a3 + a4))
}

// 2.3 一元折叠 + 逻辑运算
template <typename... Args>
bool all_true(Args... args) {
    return (... && args);  // 所有参数都为 true 才返回 true
}

template <typename... Args>
bool any_true(Args... args) {
    return (... || args);  // 至少一个为 true 就返回 true
}

// 2.4 一元折叠 + 逗号运算符（依次执行，返回最后一个表达式的结果）
template <typename... Args>
void print_all_fold(Args&&... args) {
    (std::println("{}", std::forward<Args>(args)), ...);
    // 等价于: println(a1), println(a2), println(a3);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 二元折叠 — 有初始值的折叠
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】指定一个初始值参与折叠运算。
// 【为什么】(1) 空参数包时，一元折叠对某些运算符（如 +、*）不合法；
//          二元折叠有初始值可以优雅处理空包。
//          (2) 可以用初始值作为累积器。

// 3.1 安全的求和（空参数包返回初始值 0）
template <typename... Args>
auto safe_sum(Args... args) {
    return (0 + ... + args);  // 二元左折叠：空包时返回 0
}

// 3.2 安全的求积（空参数包返回 1）
template <typename... Args>
auto safe_product(Args... args) {
    return (1 * ... * args);
}

// 3.3 字符串拼接（二元折叠 + operator+）
template <typename... Args>
std::string join_strings(Args&&... args) {
    return (std::string{} + ... + std::forward<Args>(args));
    // 等价于: string{} + a1 + a2 + a3 + a4
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 实战 — 用折叠表达式替代旧写法
// ═══════════════════════════════════════════════════════════════════════════════

// 4.1 旧写法（递归展开）：向 vector 中插入多个元素
template <typename T>
void push_all_old(std::vector<T>&) {}  // 终止

template <typename T, typename... Rest>
void push_all_old(std::vector<T>& v, T first, Rest... rest) {
    v.push_back(first);
    push_all_old(v, rest...);
}

// 4.2 新写法（折叠表达式）：一行搞定
template <typename T, typename... Rest>
void push_all_new(std::vector<T>& v, Rest&&... rest) {
    (v.push_back(std::forward<Rest>(rest)), ...);
}

// 4.3 自定义输出：比逐元素打印更灵活
template <typename... Args>
void formatted_print(std::string_view sep, Args&&... args) {
    std::size_t i = 0;
    auto print_one = [&](const auto& arg) {
        if (i++ > 0) std::print("{}", sep);
        std::print("{}", arg);
    };
    (print_one(std::forward<Args>(args)), ...);
    std::println("");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 运算符不满足结合律时, 左/右折叠结果不同! (如 - / %)
// 2. 空参数包 + 一元折叠: && → true, || → false, 逗号 → void
// 3. 空参数包 + 一元折叠: + * | & 等运算符不合法, 用二元折叠
// 4. 字符串拼接用二元折叠: (std::string{} + ... + args)
