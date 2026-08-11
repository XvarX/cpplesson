#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  lambda_basics.hpp — Lambda 基础: 语法、捕获、mutable、IILE                 ║
// ║  提供: Counter 类(演示 [this] vs [*this] 捕获)                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>

namespace move_lambda {

// ═══════════════════════════════════════════════════════════════════════════════
// Counter — 演示 [this] 和 [*this] 捕获的差异
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  [this] 按引用捕获, Lambda 持有 this 指针, 跟随原对象变化
//        [*this] 按值捕获 *this(C++17), Lambda 内是完整副本, 独立于原对象
// 注意:  [this] 的危险 — 如果原对象被销毁, this 悬垂!

class Counter {
    int count_ = 0;
public:
    // [this] 按引用捕获: Lambda 持有 this 指针
    // 危险: 如果 Counter 被销毁, this 悬垂!
    auto getIncrementer_byRef() {
        return [this] { std::println("  count={}", ++count_); };
    }

    // [*this] 按值捕获 *this(C++17)
    // Lambda 内是 Counter 的完整副本, 安全但拷贝开销
    auto getIncrementer_byCopy() {
        return [*this]() mutable {
            std::println("  count(副本)={}", ++count_);
        };
    }
};

} // namespace move_lambda
