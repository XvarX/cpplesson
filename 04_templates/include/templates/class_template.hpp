#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  class_template.hpp — 类模板                                                 ║
// ║  可复用组件: Box<T>, Calculator<T>, FixedStack<T,Size>, Constant<auto>,     ║
// ║              Buffer<T,Size>                                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <format>
#include <typeinfo>

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 类模板基础
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】跟函数模板类似，让类的成员类型变成参数。
// 【为什么】std::vector<int> 和 std::vector<double> 是同一个模板的不同实例。
//          不用为每种元素类型重写整个数据结构。
// 【什么时候用】容器、包装器、算法策略类——任何"结构相同、数据类型不同"的场景。

template <typename T>
class Box {
public:
    // 构造函数：接收一个 T 类型的值并存储
    explicit Box(T value) : m_value(value) {}

    T get() const { return m_value; }
    void set(T value) { m_value = value; }

    // 成员函数也可以用 auto 返回（C++14起允许）
    auto describe() const -> std::string {
        return std::format("Box 中的值是: {}", m_value);
    }

private:
    T m_value;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 类外定义成员函数
// ═══════════════════════════════════════════════════════════════════════════════
// 类模板的成员函数可以定义在类外，但需要：
//   1. 重复 template<typename T> 声明
//   2. 函数名前加上 类名<T>:: 限定
// 注意：定义和声明必须在同一个翻译单元（通常都在头文件），否则链接失败。

template <typename T>
class Calculator {
public:
    Calculator(T a, T b) : m_a(a), m_b(b) {}

    // 声明（类内）
    T add() const;
    T multiply() const;

private:
    T m_a, m_b;
};

// 定义（类外）— 每个成员函数前都要写 template<typename T>
template <typename T>
T Calculator<T>::add() const {
    return m_a + m_b;
}

template <typename T>
T Calculator<T>::multiply() const {
    return m_a * m_b;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 非类型模板参数 (NTTP, Non-Type Template Parameter)
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】模板参数不仅是类型，还可以是值（整数、枚举、指针、引用等）。
// 【为什么】std::array<int, 5> 在编译期就确定了大小，不需要堆分配。
//          值作为模板参数可以实现编译期多态和更强的类型安全。
// 【什么时候用】固定大小容器、编译期常量策略、维度参数。

template <typename T, int Size>
class FixedStack {
public:
    void push(T value) {
        if (m_top < Size) {
            m_data[m_top++] = value;
        }
    }

    T pop() {
        if (m_top > 0) {
            return m_data[--m_top];
        }
        return T{};  // 空栈返回默认值
    }

    int size() const { return m_top; }
    static constexpr int capacity() { return Size; }

private:
    T m_data[Size]{};  // 编译期确定大小的数组，零初始化
    int m_top = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: auto NTTP (C++17) — 让编译器推导非类型模板参数的类型
// ═══════════════════════════════════════════════════════════════════════════════
// 省去写 int、size_t 等类型，编译器自动推导
template <auto Value>
struct Constant {
    static constexpr auto value = Value;
    // decltype(Value) 自动获取类型
    using type = decltype(Value);
};

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: 默认模板参数
// ═══════════════════════════════════════════════════════════════════════════════
// 类模板也可以有默认参数，类似函数默认参数
// std::vector<T, Allocator = std::allocator<T>> 就是经典例子

template <typename T = int, int Size = 10>
class Buffer {
public:
    Buffer() { std::println("创建 Buffer<T={}, Size={}>", typeid(T).name(), Size); }

    T& at(int i) { return m_data[i]; }
    static constexpr int capacity() { return Size; }

private:
    T m_data[Size]{};
};
