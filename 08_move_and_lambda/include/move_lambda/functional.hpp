#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  functional.hpp — std::function、bind、mem_fn、invoke、reference_wrapper    ║
// ║  提供: Functor、Person、Calculator 等可调用对象演示类型                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <functional>

namespace move_lambda {

// ═══════════════════════════════════════════════════════════════════════════════
// free_func — 自由函数, 用于演示 std::function 包装函数指针
// ═══════════════════════════════════════════════════════════════════════════════
int free_func(int x);

// ═══════════════════════════════════════════════════════════════════════════════
// Functor — 函数对象, 用于演示 std::function 包装函数对象
// ═══════════════════════════════════════════════════════════════════════════════
struct Functor {
    int multiplier;
    int operator()(int x) const { return x * multiplier; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// print_three — 三参数打印函数, 用于演示 std::bind 参数绑定与重排
// ═══════════════════════════════════════════════════════════════════════════════
void print_three(int a, int b, int c);

// ═══════════════════════════════════════════════════════════════════════════════
// Person — 人员结构体, 用于演示 std::mem_fn 包装成员函数
// ═══════════════════════════════════════════════════════════════════════════════
struct Person {
    std::string name;
    int age;

    void greet() const;
    int getAge() const { return age; }
    bool isAdult() const { return age >= 18; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Calculator — 计算器结构体, 用于演示 std::invoke 统一调用接口
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::invoke 能统一调用:
//        - 普通可调用对象: f(args...)
//        - 成员函数指针:   (obj.*pmf)(args...) 或 (ptr->*pmf)(args...)
//        - 成员数据指针:   obj.*pmd  或  ptr->*pmd
struct Calculator {
    int base;

    int add(int x) const { return base + x; }
    int multiply(int x) const { return base * x; }

    // 成员数据指针演示
    int value = 100;
};

// ═══════════════════════════════════════════════════════════════════════════════
// modify / show — 用于演示 std::reference_wrapper 配合 bind/thread
// ═══════════════════════════════════════════════════════════════════════════════
void modify(int& x);
void show(const int& x);

} // namespace move_lambda
