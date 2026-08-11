#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  modern_syntax/ctad.hpp                                                     ║
// ║  类模板参数推导(CTAD) / 推导指引 / 聚合CTAD (C++17/20) —— 可复用模板     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <vector>
#include <string>
#include <iterator>

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Wrapper<T> —— 简单的包装类模板                                            ║
// ║  构造函数从 T 构造 → 编译器能推导 T                                        ║
// ║  Wrapper w1(42);                // CTAD → Wrapper<int>                      ║
// ║  Wrapper w2(std::string("...")); // CTAD → Wrapper<string>                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T>
struct Wrapper {
    T value;
    // 构造函数: 从 T 构造 → 编译器能推导 T
    explicit Wrapper(T v) : value(v) {}
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  DynArray<T> —— 动态数组模板，演示推导指引的多种场景                        ║
// ║  包含:                                                                      ║
// ║    - initializer_list 构造函数 (隐式推导)                                   ║
// ║    - 迭代器范围构造函数 (需要推导指引)                                      ║
// ║    - C 风格字符串 → std::string 的推导指引                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T>
class DynArray {
public:
    std::vector<T> data;

    // 从 initializer_list 构造
    DynArray(std::initializer_list<T> il) : data(il) {}

    // 从迭代器范围构造 [first, last)
    // 问题: 编译器无法从 InputIt 推导出 T!
    template<typename InputIt>
    DynArray(InputIt first, InputIt last) : data(first, last) {}
};

// ★ 推导指引: "用迭代器构造时, T = 迭代器指向的元素的类型"
template<typename InputIt>
DynArray(InputIt, InputIt) -> DynArray<typename std::iterator_traits<InputIt>::value_type>;

// ★ 另一个推导指引: C 风格字符串 → DynArray<std::string>
DynArray(const char*) -> DynArray<std::string>;

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Point2D<T> —— 模板化的聚合类型 (无用户定义构造函数)                       ║
// ║  C++20 聚合 CTAD 需要推导指引 (或编译器自动合成)                            ║
// ║  对聚合类型, C++20 编译器有时能自动推导, 但明确写指引更安全                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T>
struct Point2D {
    T x{};
    T y{};
    std::string label{};
};

// C++20 聚合 CTAD 推导指引
template<typename T>
Point2D(T, T) -> Point2D<T>;

template<typename T>
Point2D(T, T, std::string) -> Point2D<T>;

} // namespace modern_syntax
