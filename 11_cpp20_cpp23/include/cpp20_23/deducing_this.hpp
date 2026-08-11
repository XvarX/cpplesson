#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: Deducing This (C++23) — 显式对象参数                                 ║
// ║  目标: 掌握显式对象参数语法，简化 CRTP，实现递归 lambda，值类别感知          ║
// ║                                                                            ║
// ║  Deducing This 的核心语法:                                                  ║
// ║    成员函数第一个参数用 self 标记 (如 self&&, const self&)                   ║
// ║    编译器根据调用对象的实际类型推导 self 的类型                              ║
// ║                                                                            ║
// ║  WHY:                                                                       ║
// ║  1. 简化 CRTP — 不需要模板参数 T 和 static_cast<Derived*>                  ║
// ║  2. 递归 lambda — lambda 可以在自己体内调用自己                             ║
// ║  3. 值类别感知 — self&& 可区分左值/右值，避免重复代码                       ║
// ║  4. self 返回类型 — 返回与调用对象相同类型的引用/值                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <string>
#include <string_view>
#include <type_traits>

// ============================================================================
// Part 1: 简化 CRTP — 告别 static_cast<Derived*>
// ============================================================================
// CRTP 模式: 基类通过模板参数知道派生类类型，实现编译期多态。
// C++17: 需要 template<typename Derived> + static_cast<Derived*>(this)
// C++23: 用 deducing this 替代，代码简洁且类型安全。

// ── 旧式 CRTP (C++17) ──
// template<typename Derived>
// struct OldCRTP {
//     void do_something() {
//         static_cast<Derived*>(this)->impl();  // 丑陋的类型转换
//     }
// };

// ── 新式 deducing this (C++23) — 不需要模板参数! ──
struct Builder {
    std::string data;

    // 显式对象参数: self 的类型由编译器推导
    // 返回 auto& 使得链式调用时保留派生类类型
    template<typename Self>
    auto& append(this Self&& self, std::string_view text) {
        self.data.append(text);
        return self;  // 保留值类别!
    }

    template<typename Self>
    auto& clear(this Self&& self) {
        self.data.clear();
        return self;
    }

    const std::string& get() const { return data; }
};

// ============================================================================
// Part 3: 值类别感知 (Value Category Overloading)
// ============================================================================

struct ValueInspector {
    std::string name;

    // 左值版本: 只读访问
    void inspect(this const ValueInspector& self);

    // 右值版本: 可以"窃取"资源
    void inspect(this ValueInspector&& self);

    // 非 const 左值版本: 可修改
    void modify(this ValueInspector& self, std::string_view new_name);
};

// ============================================================================
// Part 4: self 返回类型 — 返回与调用者相同的类型
// ============================================================================

struct Cloneable {
    int value;

    // 左值调用 → 返回拷贝; 右值调用 → 返回移动的值
    auto clone(this auto&& self) {
        using T = std::remove_cvref_t<decltype(self)>;
        return T{self.value};  // 调用拷贝或移动构造，取决于 self 的类别
    }
};
