#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  forward_perfect.hpp — std::forward、万能引用、完美转发、emplace            ║
// ║  提供: process 重载、relay 模板、Widget 工厂、Point emplace 演示            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace move_lambda {

// ═══════════════════════════════════════════════════════════════════════════════
// process 重载 — 用于演示万能引用将参数转发到正确的重载版本
// ═══════════════════════════════════════════════════════════════════════════════

// 版本1: 接受左值引用
void process(int& x);

// 版本2: 接受 const 左值引用
void process(const int& x);

// 版本3: 接受右值引用
void process(int&& x);

// ═══════════════════════════════════════════════════════════════════════════════
// relay — 万能引用 + std::forward 实现完美转发
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  T&& 在模板推导上下文中是"万能引用":
//        - 传入左值 → T 推导为 T&,  最终变成 T&  && → T&  (左值引用)
//        - 传入右值 → T 推导为 T,   最终变成 T&&      (右值引用)
// WHY:   一个函数模板就能同时接受左值和右值, 且保持参数的值类别
// WHEN:  编写泛型包装器、工厂函数、转发层时

template<typename T>
void relay(T&& arg) {
    // std::forward<T>: 如果 arg 是左值 → 转发为左值引用
    //                 如果 arg 是右值 → 转发为右值引用
    process(std::forward<T>(arg));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Widget — 演示完美转发工厂函数的目标类型
// ═══════════════════════════════════════════════════════════════════════════════
struct Widget {
    std::string name;
    std::vector<int> data;

    // 普通构造
    Widget(std::string n, std::vector<int> d)
        : name(std::move(n)), data(std::move(d)) {
        std::println("  Widget 构造: \"{}\" ({}条数据)", name, data.size());
    }

    // 移动构造
    Widget(Widget&& other) noexcept
        : name(std::move(other.name)), data(std::move(other.data)) {
        std::println("  Widget 移动构造");
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// makeWidget — 万能工厂: 参数原样转发给 Widget 的构造函数
// ═══════════════════════════════════════════════════════════════════════════════
template<typename... Args>
std::unique_ptr<Widget> makeWidget(Args&&... args) {
    // std::forward<Args> 保持每个参数的左值/右值属性
    return std::make_unique<Widget>(std::forward<Args>(args)...);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Point — 带输出的结构体, 用于演示 emplace 原地构造 vs push_back
// ═══════════════════════════════════════════════════════════════════════════════
struct Point {
    int x, y;
    std::string label;

    Point(int x_, int y_, std::string l) : x(x_), y(y_), label(std::move(l)) {
        std::println("  Point({}, {}, \"{}\") 构造", x, y, label);
    }
    Point(const Point& p) : x(p.x), y(p.y), label(p.label) {
        std::println("  Point 拷贝");
    }
    Point(Point&& p) noexcept : x(p.x), y(p.y), label(std::move(p.label)) {
        std::println("  Point 移动");
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// bad_relay — 反模式: 用 std::move 代替 std::forward
// ═══════════════════════════════════════════════════════════════════════════════
// ❌ 把左值也转成了右值, 可能意外移动!
template<typename T>
void bad_relay(T&& arg) {
    process(std::move(arg));
}

} // namespace move_lambda
