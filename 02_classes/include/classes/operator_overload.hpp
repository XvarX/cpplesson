#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 运算符重载 (operator_overload)                         ║
// ║  内容: 算术/比较/<=>/下标/调用/成员vs非成员                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <compare>   // C++20 三路比较
#include <string>
#include <vector>
#include <ostream>
#include <functional>

// ═══════════════════════════════════════════════════════════════════════════════
// Vec2 — 算术运算符重载 (成员复合赋值 + 非成员二元)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 重载 +、-、*、/ 等运算符让自定义类型像内置类型一样运算
// 为什么: 代码可读性: vec3 a = b + c; 比 vec3 a = b.add(c); 更直观
// 何时用: 类型具有自然的算术语义时 (向量、矩阵、复数、货币等)

class Vec2 {
public:
    Vec2(double x, double y) : x_(x), y_(y) {}

    // 复合赋值: 成员函数 (修改自身) → 返回引用
    Vec2& operator+=(const Vec2& rhs) {
        x_ += rhs.x_; y_ += rhs.y_;
        return *this;
    }
    Vec2& operator*=(double scalar) {
        x_ *= scalar; y_ *= scalar;
        return *this;
    }

    void print() const { std::println("({:.0f}, {:.0f})", x_, y_); }

    double x() const { return x_; }
    double y() const { return y_; }

private:
    double x_, y_;
};

// 二元运算符: 非成员函数 (支持左操作数隐式转换)
// 基于复合赋值实现 (减少代码重复)
inline Vec2 operator+(const Vec2& a, const Vec2& b) {
    Vec2 result = a;
    result += b;
    return result;
}
inline Vec2 operator*(const Vec2& v, double s) {
    Vec2 result = v;
    result *= s;
    return result;
}
inline Vec2 operator*(double s, const Vec2& v) { return v * s; } // 对称

// ═══════════════════════════════════════════════════════════════════════════════
// Version — <=> 三路比较 (C++20)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: operator<=> 返回 std::strong_ordering / weak_ordering / partial_ordering
// 为什么: 只需定义 <=> (和 ==)，编译器自动生成 <,<=,>,>=,!=
// 何时用: 类型需要排序/比较时，优先用 <=> 替代手写所有比较运算符

class Version {
public:
    Version(int major, int minor, int patch)
        : major_(major), minor_(minor), patch_(patch) {}

    // C++20: 只需定义 <=> 和 ==
    auto operator<=>(const Version&) const = default; // 编译器按声明顺序比较
    // 等价于手写: 先比 major, 再比 minor, 最后比 patch

    friend std::ostream& operator<<(std::ostream& os, const Version& v) {
        return os << v.major_ << '.' << v.minor_ << '.' << v.patch_;
    }

private:
    int major_, minor_, patch_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Matrix — operator[] (const 和 非 const 重载)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: operator[] 让对象像数组一样访问元素
// 为什么: 容器、矩阵等需要索引访问
// 何时用: 有索引语义的类

class Matrix {
public:
    Matrix() { data_.resize(4, 0.0); }

    // 非 const 版本: 可修改
    double& operator[](size_t idx) { return data_[idx]; }
    // const 版本: 只读访问
    const double& operator[](size_t idx) const { return data_[idx]; }

private:
    std::vector<double> data_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Multiplier — operator() 仿函数 (像函数一样调用对象)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: operator() 让对象像函数一样调用
// 为什么: 仿函数 (functor)、回调包装、lambda 底层就是它
// 何时用: 需要可调用对象、需要携带状态的函数

class Multiplier {
public:
    explicit Multiplier(int factor) : factor_(factor) {}
    int operator()(int x) const { return x * factor_; } // 像函数一样调用
private:
    int factor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Score — 成员 vs 非成员比较
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 运算符可定义为成员函数 (this 作为左操作数) 或非成员函数
// 为什么: 非成员支持左操作数隐式转换 (如 100 == score)
// 何时用:
//   = [] () ->           必须是成员
//   复合赋值 += -= *= /=  首选成员
//   二元算术 + - * /      首选非成员 (复用复合赋值)
//   比较 == != < <= > >=  首选非成员 (对称性)
//   << >> (IO)            必须是非成员 (因为左操作数是 ostream)

class Score {
public:
    explicit Score(int v) : value_(v) {}
    int value() const { return value_; }

    // 成员比较 (只支持 Score < Score)
    bool operator<(const Score& rhs) const { return value_ < rhs.value_; }

private:
    int value_;
};

// 非成员相等比较 (支持隐式转换, 如 100 == score)
inline bool operator==(const Score& a, const Score& b) {
    return a.value() == b.value();
}
