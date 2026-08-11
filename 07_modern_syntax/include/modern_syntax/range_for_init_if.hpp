#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  modern_syntax/range_for_init_if.hpp                                        ║
// ║  range-for / init-if / init-switch / [[likely]] [[unlikely]] —— 可复用类型 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <memory>

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Animal 类层次 —— 演示 init-if 配合 dynamic_cast 的多态安全转换           ║
// ║  实际应用中，这里可能是工厂函数返回的指针                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
struct Animal {
    virtual ~Animal() = default;
};

struct Dog : Animal {
    void bark() { std::println("汪汪!"); }
};

struct Cat : Animal {
    void meow() { std::println("喵喵!"); }
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  运算枚举 —— 演示 init-switch 模式                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
enum class Op { Add, Sub, Mul, Div };

} // namespace modern_syntax
