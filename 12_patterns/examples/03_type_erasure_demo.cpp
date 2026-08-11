// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_type_erasure_demo.cpp — 类型擦除演示                                    ║
// ║  隐藏具体类型,用统一接口操作不同类型的对象                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 核心概念:
//   - 将不同类型"擦除"为同一个类型,通过统一接口操作
//   - 不同于继承: 不要求类型之间有继承关系(非侵入式)
//   - std::function 是最经典的类型擦除例子
// 实现原理:
//   - "外部多态": 类型擦除类 + 模板构造函数 + 虚函数桥接(Concept/Model)
//   - 值语义: 类型擦除后的对象可以像普通值一样拷贝、移动
// 适用场景:
//   - 统一容器存储无关类型 (如 std::vector<Drawable>)
//   - 回调/策略模式 (std::function)
//   - 插件系统

#include "patterns/type_erasure.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <memory>
#include <string>
#include <vector>
#include <functional>

auto main() -> int {
    lesson::print_header("03  类型擦除 — 隐藏具体类型");

    // ── 示例1: std::function ────────────────────────────────────────────────
    lesson::print_subtitle("1. std::function — 标准类型擦除");
    patterns::demo_std_function();

    // ── 示例2: AnyDrawable — 统一容器 ──────────────────────────────────────
    lesson::print_subtitle("2. 手写类型擦除 — AnyDrawable");
    std::vector<patterns::Drawable> shapes;
    shapes.push_back(patterns::Circle{5.0});
    shapes.push_back(patterns::Rectangle{4.0, 6.0});
    shapes.push_back(patterns::TextLabel{"Hello Type Erasure!"});

    for (const auto& s : shapes) s.draw();

    // ── 拷贝语义 ────────────────────────────────────────────────────────────
    lesson::print_subtitle("2b. 拷贝语义");
    patterns::Drawable a = patterns::Circle{3.0};
    patterns::Drawable b = a;  // 深拷贝底层的 Circle 对象
    std::print("  a: "); a.draw();
    std::print("  b: "); b.draw();

    // ── 示例3: 轻量级版本 ──────────────────────────────────────────────────
    lesson::print_subtitle("3. 轻量级类型擦除 (std::function)");
    patterns::LightDrawable ld1{patterns::Circle{2.5}};
    patterns::LightDrawable ld2{patterns::Rectangle{3.0, 4.0}};
    ld1.draw();
    ld2.draw();

    // ── 对比总结 ────────────────────────────────────────────────────────────
    lesson::print_subtitle("类型擦除 vs 继承 对比");
    std::println("┌──────────────────┬─────────────────────┬─────────────────────┐");
    std::println("│      特性        │   继承(虚函数)      │     类型擦除        │");
    std::println("├──────────────────┼─────────────────────┼─────────────────────┤");
    std::println("│ 侵入性           │ 侵入式(须继承)      │ 非侵入式            │");
    std::println("│ 类型要求         │ 必须是派生类        │ 满足\"接口\"即可       │");
    std::println("│ 容器存储         │ shared_ptr<Base>    │ vector<Drawable> 值 │");
    std::println("│ 运行时开销       │ vtable 间接调用     │ vtable + 堆分配     │");
    std::println("│ 增加新类型       │ 容易(新增派生类)    │ 容易(不需继承)      │");
    std::println("│ 增加新操作       │ 需改基类(影响大)    │ 需改 Concept(影响大) │");
    std::println("└──────────────────┴─────────────────────┴─────────────────────┘");

    // ── 常见陷阱 ────────────────────────────────────────────────────────────
    lesson::print_subtitle("常见陷阱");
    std::println("⚠️  忘记实现克隆 → 默认拷贝只浅拷贝指针(into double-free)");
    std::println("⚠️  堆分配开销 → 每次构造 Drawable 都 new Model<T>");
    std::println("   缓解: SBO (Small Buffer Optimization),类似 std::function 的实现");
    std::println("⚠️  Concept 虚函数越多,就越像传统继承 — 权衡接口粒度");

    lesson::print_subtitle("练习");
    std::println("1. 为 Drawable 添加 area() 方法(返回面积)");
    std::println("2. 实现一个 AnyPrintable 类型擦除类");
    std::println("3. 比较: 类型擦除 vs std::variant 实现统一容器");
    std::println("4. 给 LightDrawable 添加拷贝语义支持");

    return 0;
}
