// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  forward_perfect.cpp — process 重载函数的实现                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "move_lambda/forward_perfect.hpp"

namespace move_lambda {

// 版本1: 接受左值引用
void process(int& x) {
    std::println("  process(int&)  — 左值引用, x = {}", x);
}

// 版本2: 接受 const 左值引用
void process(const int& x) {
    std::println("  process(const int&) — const左值引用, x = {}", x);
}

// 版本3: 接受右值引用
void process(int&& x) {
    std::println("  process(int&&) — 右值引用, x = {}", x);
}

} // namespace move_lambda
