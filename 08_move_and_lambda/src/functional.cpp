// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  functional.cpp — 自由函数与 Person 成员函数的实现                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "move_lambda/functional.hpp"

namespace move_lambda {

// ── free_func — 自由函数, 用于 std::function 演示 ─────────────────────────────
int free_func(int x) {
    return x * 2;
}

// ── print_three — 三参数打印, 用于 std::bind 参数绑定演示 ──────────────────────
void print_three(int a, int b, int c) {
    std::println("  print_three({}, {}, {})", a, b, c);
}

// ── Person::greet — 成员函数, 用于 std::mem_fn 演示 ────────────────────────────
void Person::greet() const {
    std::println("  你好, 我是 {}, {} 岁", name, age);
}

// ── modify — 修改引用参数, 用于 std::reference_wrapper 演示 ────────────────────
void modify(int& x) {
    x *= 2;
}

// ── show — 打印引用参数, 用于 std::reference_wrapper 演示 ──────────────────────
void show(const int& x) {
    std::print("{} ", x);
}

} // namespace move_lambda
