// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  crtp.hpp — CRTP (Curiously Recurring Template Pattern)                     ║
// ║  静态多态: 编译期绑定,零虚函数开销                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 核心概念:
//   - 派生类将自己作为模板参数传给基类: class Derived : public Base<Derived>
//   - 基类通过 static_cast<Derived*>(this) 调用派生类方法
//   - 所有绑定在编译期完成,没有虚函数表(vtable)开销
// 适用场景:
//   - 需要多态但性能要求极高的场景(嵌入式、游戏引擎)
//   - Mixin 模式: 为多个类添加公共功能
//   - `std::enable_shared_from_this<T>` 就是 CRTP 的经典应用
// 与虚函数对比:
//   - 虚函数: 运行时多态,有 vtable 开销,可以统一容器存储
//   - CRTP:   编译期多态,零开销,但每个派生类产生不同的基类类型

#pragma once

#include <print>
#include <string>

namespace patterns {

// ═══════════════════════════════════════════════════════════════════════════════
// 示例1: 基本 CRTP — 为派生类添加功能
// ═══════════════════════════════════════════════════════════════════════════════

template <typename Derived>
class Counter {
public:
    void increment() {
        // static_cast 安全: Derived 必然继承自 Counter<Derived>
        static_cast<Derived*>(this)->do_increment();
    }
    int get_count() const {
        return static_cast<const Derived*>(this)->do_get_count();
    }
};

class ClickCounter : public Counter<ClickCounter> {
public:
    void do_increment() { clicks_++; }
    int  do_get_count() const { return clicks_; }
private:
    int clicks_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// 示例2: CRTP 静态多态 — 形状类(无虚函数!)
// ═══════════════════════════════════════════════════════════════════════════════

template <typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }
    void print() const {
        std::println("  {} area = {:.2f}",
                     static_cast<const Derived*>(this)->name(), area());
    }
};

class Circle : public Shape<Circle> {
public:
    explicit Circle(double r) : radius_(r) {}
    double area_impl() const { return 3.14159 * radius_ * radius_; }
    std::string name() const { return "Circle"; }
private:
    double radius_;
};

class Rectangle : public Shape<Rectangle> {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}
    double area_impl() const { return width_ * height_; }
    std::string name() const { return "Rectangle"; }
private:
    double width_, height_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// 示例3: CRTP Mixin — 自动生成比较运算符
// ═══════════════════════════════════════════════════════════════════════════════

template <typename Derived>
class Comparable {
public:
    friend bool operator!=(const Derived& l, const Derived& r) { return !(l == r); }
    friend bool operator> (const Derived& l, const Derived& r) { return r < l;  }
    friend bool operator<=(const Derived& l, const Derived& r) { return !(r < l); }
    friend bool operator>=(const Derived& l, const Derived& r) { return !(l < r); }
};

// 只需定义 == 和 <, Comparable 自动提供其余4个运算符
struct Point2D : Comparable<Point2D> {
    int x, y;
    // 构造函数: 支持 Point2D{1, 2} 聚合初始化 (与 CRTP 基类兼容)
    Point2D() = default;
    Point2D(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const Point2D& o) const { return x == o.x && y == o.y; }
    bool operator< (const Point2D& o) const {
        return (x < o.x) || (x == o.x && y < o.y);
    }
};

} // namespace patterns
