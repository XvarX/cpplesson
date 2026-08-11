// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  type_erasure.cpp — Drawable 类的非模板实现                                ║
// ║  拷贝构造/赋值等不依赖模板参数 T 的方法放在这里                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "patterns/type_erasure.hpp"
#include <print>
#include <functional>

namespace patterns {

// ── Drawable 拷贝构造 ────────────────────────────────────────────────────────
Drawable::Drawable(const Drawable& other)
    : pimpl_(other.pimpl_ ? other.pimpl_->clone() : nullptr)
{}

// ── Drawable 拷贝赋值 ────────────────────────────────────────────────────────
Drawable& Drawable::operator=(const Drawable& other) {
    if (this != &other)
        pimpl_ = other.pimpl_ ? other.pimpl_->clone() : nullptr;
    return *this;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 示例1: std::function 类型擦除演示
// ═══════════════════════════════════════════════════════════════════════════════

void demo_std_function() {
    // 三种完全不同的可调用类型,但都能放进同一个 std::function
    std::vector<std::function<int(int)>> ops;

    // (a) Lambda (无捕获 → 可转为函数指针)
    ops.push_back([](int x) { return x * x; });

    // (b) 有捕获的 Lambda — 类型与 (a) 完全不同
    int factor = 3;
    ops.push_back([factor](int x) { return x * factor; });

    // (c) 函数对象 — 又是一种不同的类型
    struct Adder { int n; int operator()(int x) const { return x + n; } };
    ops.push_back(Adder{10});

    // 统一调用 — 不需要知道它们的真实类型
    for (size_t i = 0; i < ops.size(); ++i)
        std::println("  op[{}](5) = {}", i, ops[i](5));
}

} // namespace patterns
