// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/iterators.hpp — 迭代器                                                 ║
// ║  主题: 迭代器类别、iterator_traits、自定义迭代器、begin/end 重载            ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
#pragma once

#include "shared/lesson_utils.hpp"
#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <iterator>
#include <list>
#include <print>
#include <ranges>
#include <vector>

namespace stl_learn {

// ── 可复用的类型：Range — 带自定义迭代器的整数范围类 ──────────────────────────

/// @brief 简单的整数范围类，演示如何实现自定义迭代器使其支持 range-for 和 STL 算法
/// @details 表示 [begin, end) 的半开区间，如 Range(0, 5) 表示 {0, 1, 2, 3, 4}
class Range {
private:
    int _begin, _end;

public:
    Range(int b, int e) : _begin(b), _end(e) {}

    /// @brief 前向迭代器：满足 std::forward_iterator 概念
    class Iterator {
        int current;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = const int*;
        using reference = const int&;

        // 迭代器必须可默认构造 — std::ranges::end 要求哨兵满足 sentinel_for
        // (含 semiregular → default_initializable)，否则 Range 不满足 range 概念
        Iterator() = default;
        explicit Iterator(int v) : current(v) {}
        int operator*() const { return current; }
        Iterator& operator++() {
            ++current;
            return *this;
        }
        Iterator operator++(int) {
            auto tmp = *this;
            ++current;
            return tmp;
        }
        bool operator==(const Iterator& other) const = default;
    };

    Iterator begin() const { return Iterator(_begin); }
    Iterator end() const { return Iterator(_end); }  // 不包含 _end
};

// ── Part 1: 迭代器类别 (Iterator Categories) ──────────────────────────────────
// 是什么: 迭代器分 6 个类别，从功能弱到强排列
// 为什么: 算法的复杂度保证依赖迭代器类别; 泛型代码通过 category 做编译期分支
// 什么时候用: 编写泛型函数时根据 iterator_category 选择最优实现
void part1_categories();

// ── Part 2: iterator_traits — 提取迭代器属性 ─────────────────────────────────
// 是什么: 模板元编程工具，提取迭代器的 5 种属性: value_type, difference_type,
//         pointer, reference, iterator_category
// 为什么: 泛型代码需要知道迭代器指向的类型等信息才能正确操作
// 什么时候用: 编写泛型算法/容器时
void part2_traits();

// ── Part 3: 迭代器辅助: 插入迭代器、流迭代器、反向迭代器 ──────────────────────
// 是什么: STL 提供多种特殊迭代器适配器
// 为什么: 统一接口，让算法能用于各种场景
// 什么时候用: 需要将算法的输出插入容器、流、或反向遍历
void part3_adaptors();

// ── Part 4: 自定义迭代器 (简化版 range 迭代器) ───────────────────────────────
// 是什么: 为自己的容器/数据结构实现符合 STL 接口的迭代器
// 为什么: 自定义容器可以与 STL 算法和 range-for 无缝配合
// 什么时候用: 设计新的数据结构时
void part4_custom_iterator();

// ── Part 5: C++20 ranges — 现代迭代器抽象 ────────────────────────────────────
// 是什么: C++20 的 ranges 库将迭代器对 (begin/end) 抽象为 range 概念
// 为什么: 更安全(防悬垂)、更简洁(管道语法)、更好的错误信息
// 什么时候用: C++20 项目中优先使用 ranges 代替传统迭代器对
void part5_ranges();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void iterators_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void iterators_exercises();

} // namespace stl_learn
