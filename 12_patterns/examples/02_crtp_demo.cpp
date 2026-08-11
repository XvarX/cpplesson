// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  02_crtp_demo.cpp — CRTP (Curiously Recurring Template Pattern)             ║
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

#include "patterns/crtp.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>

auto main() -> int {
    lesson::print_header("02  CRTP — 静态多态");

    // ── 示例1: 基本 CRTP ─────────────────────────────────────────────────────
    lesson::print_subtitle("1. 基本 CRTP — 自增计数器");
    patterns::ClickCounter cc;
    cc.increment(); cc.increment(); cc.increment();
    std::println("  ClickCounter: {}", cc.get_count());  // 3

    // ── 示例2: 静态多态 ─────────────────────────────────────────────────────
    lesson::print_subtitle("2. 静态多态 — 形状类");
    patterns::Circle    c(5.0);
    patterns::Rectangle r(4.0, 6.0);
    c.print();
    r.print();
    // ⚠️ CRTP 限制: Shape<Circle> 和 Shape<Rectangle> 是不同类型
    //    无法放入 std::vector<Shape> (解决方案: variant 或类型擦除)

    // ── 示例3: Mixin — 自动生成比较运算符 ──────────────────────────────────
    lesson::print_subtitle("3. Mixin — 自动比较运算符");
    patterns::Point2D p1{1, 2}, p2{1, 3}, p3{1, 2};
    std::println("  p1 == p2: {}", p1 == p2);  // false
    std::println("  p1 == p3: {}", p1 == p3);  // true
    std::println("  p1 != p2: {}", p1 != p2);  // true  (自动生成!)
    std::println("  p1 <  p2: {}", p1 <  p2);  // true
    std::println("  p1 >  p2: {}", p1 >  p2);  // false (自动生成!)
    std::println("  p1 <= p2: {}", p1 <= p2);  // true  (自动生成!)

    // ── CRTP vs 虚函数 对比 ─────────────────────────────────────────────────
    lesson::print_subtitle("CRTP vs 虚函数 对比");
    std::println("┌──────────────────┬─────────────────────┬─────────────────────┐");
    std::println("│      特性        │   虚函数(virtual)    │        CRTP         │");
    std::println("├──────────────────┼─────────────────────┼─────────────────────┤");
    std::println("│ 绑定时机         │ 运行时              │ 编译期              │");
    std::println("│ 开销             │ vtable + 间接调用   │ 零开销(可能内联)    │");
    std::println("│ 统一容器存储     │ 可以(基类指针)      │ 不能(不同类型)      │");
    std::println("│ 二进制体积       │ 较小(一份实现)      │ 较大(模板实例化)    │");
    std::println("└──────────────────┴─────────────────────┴─────────────────────┘");

    // ── 关于 C++23 deducing this ────────────────────────────────────────────
    std::println("\n💡 C++23 'deducing this' 可替代部分 CRTP 场景:");
    std::println("   struct Base {{ template<typename Self> void f(this Self&& s) {{ s... }} }};");
    std::println("   // 不需要 CRTP 继承! self 自动推导为实际类型");

    // ── 常见陷阱 ──────────────────────────────────────────────────────────────
    lesson::print_subtitle("常见陷阱");
    std::println("⚠️  忘记写 static_cast,直接用 this->impl() — 可能无限递归");
    std::println("⚠️  派生类写错模板参数: class A : Base<B> — 应为 Base<A>");
    std::println("⚠️  过度使用 CRTP 导致代码难读,简单场景用虚函数即可");

    lesson::print_subtitle("练习");
    std::println("1. 为 Shape CRTP 添加 perimeter() 方法");
    std::println("2. 实现 Cloneable CRTP mixin, 为派生类添加 clone() 方法");
    std::println("3. 用虚函数实现同样的 Shape,用 lesson::time_it 比较性能");
    std::println("4. 研究 C++23 deducing this,用新语法重写 Shape 示例");

    return 0;
}
