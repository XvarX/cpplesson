#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  lambda_advanced.hpp — Lambda 进阶: 泛型、constexpr、C++20 特性             ║
// ║  提供: ThisCaptureWidget 类(演示 C++20 [=, this] / [=, *this] 捕获)        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>

namespace move_lambda {

// ═══════════════════════════════════════════════════════════════════════════════
// ThisCaptureWidget — 演示 C++20 显式 this 捕获语义
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  C++20 之前, [=] 隐式捕获 this, 容易误解为按值捕获了 *this
//        C++20: [=, this] 显式捕获 this 指针(和以前行为一样)
//               [=, *this] 按值捕获整个对象(C++17起支持)
// WHY:   让 this 捕获意图明确, 减少意外
// WHEN:  在成员函数中定义 Lambda 时, 明确表达你的意图

struct ThisCaptureWidget {
    int value = 42;

    // C++20 推荐写法: 显式 this
    auto getReader_explicit() {
        return [=, this] {   // ← 明确表示按值捕获变量 + this 指针
            return value;     // 隐式 this->value
        };
    }

    auto getReader_byCopy() {
        return [=, *this] {  // 按值捕获整个对象副本
            return value;     // 副本的 value
        };
    }
};

} // namespace move_lambda
