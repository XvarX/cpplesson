#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: <=> 三路比较 — strong/weak/partial_ordering、=default、自定义<=>     ║
// ║  目标: 理解三种比较类别，学会用 =default 和自定义 <=> 生成所有比较运算符     ║
// ║                                                                            ║
// ║  三种比较类别 (comparison categories):                                       ║
// ║                                                                            ║
// ║  strong_ordering: 全序 — 任何两个值可比较，等价的值不可区分                 ║
// ║    → 整数比较: 5 == 5 且两个 5 无法区分                                     ║
// ║                                                                            ║
// ║  weak_ordering:   弱序 — 等价的值可能可以区分 (如字符串大小写不敏感排序)    ║
// ║    → "Hello" 和 "HELLO" 等价但不同                                          ║
// ║                                                                            ║
// ║  partial_ordering: 偏序 — 某些值之间无法比较 (如 NaN)                       ║
// ║    → float: NaN 与任何值比较结果都是 false                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <compare>   // C++20 三路比较
#include <string>
#include <cmath>

// ============================================================================
// Part 1: =default — 让编译器自动生成 <=> 及所有比较运算符
// ============================================================================
// C++20 最实用的特性之一: 写一行 auto operator<=>(const T&) const = default;
// 编译器自动生成: ==, !=, <, <=, >, >= 全部六个运算符。
// 比较顺序: 按成员声明顺序逐个比较 (字典序)。

struct Person {
    std::string name;
    int         age;

    // 一行搞定所有比较! 按 name 再 age 顺序比较
    auto operator<=>(const Person&) const = default;
    // 编译器自动生成: == != < <= > >= 全部六个!
};

// ============================================================================
// Part 2: 自定义 <=> — 不同的比较逻辑
// ============================================================================

// 示例: 一个忽略大小写的字符串比较 (返回 weak_ordering)
struct CaseInsensitiveString {
    std::string value;

    // 自定义比较: 只比较长度 → weak_ordering (长度相等的不同字符串"等价")
    std::weak_ordering operator<=>(const CaseInsensitiveString& other) const {
        if (value.size() < other.value.size()) return std::weak_ordering::less;
        if (value.size() > other.value.size()) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;  // 长度相同但内容可能不同
    }

    // 注意: =default 的 <=> 不会自动生成 ==，但自定义 <=> 也不会!
    // C++20 规则: 如果 <=> 是 =default，编译器自动生成 ==
    //            如果 <=> 是自定义的，必须单独提供 ==
    bool operator==(const CaseInsensitiveString& other) const = default;
};

// ── 浮点数包装: 展示 partial_ordering ──
struct FloatWrapper {
    double value;

    // NaN 的存在使得浮点数只能产生 partial_ordering
    std::partial_ordering operator<=>(const FloatWrapper& other) const {
        if (std::isnan(value) || std::isnan(other.value))
            return std::partial_ordering::unordered;  // 无法比较!
        if (value < other.value) return std::partial_ordering::less;
        if (value > other.value) return std::partial_ordering::greater;
        return std::partial_ordering::equivalent;
    }

    bool operator==(const FloatWrapper& other) const {
        if (std::isnan(value) || std::isnan(other.value))
            return false;  // NaN != NaN (IEEE 754 标准)
        return value == other.value;
    }
};

// ============================================================================
// Part 3: comparison categories 的转换关系
// ============================================================================
// 转换方向 (从强到弱):
//   strong_ordering → weak_ordering → partial_ordering
// 强类别可以隐式转换为弱类别 (反之不行)。
