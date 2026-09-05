#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  type_erasure.hpp — 类型擦除                                               ║
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

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <print>

namespace patterns {

// ═══════════════════════════════════════════════════════════════════════════════
// 示例1: std::function — 标准库中的类型擦除
// ═══════════════════════════════════════════════════════════════════════════════

/// 演示 std::function 如何将不同类型的可调用对象统一存储和调用
void demo_std_function();

// ═══════════════════════════════════════════════════════════════════════════════
// 示例2: 手写类型擦除 — AnyDrawable
// ═══════════════════════════════════════════════════════════════════════════════

class Drawable {
public:
    // 模板构造函数: 接受任何类型 T (不需要继承)
    template <typename T>
    Drawable(T obj)
        : pimpl_(std::make_unique<Model<T>>(std::move(obj))) {}

    // 值语义: 支持拷贝
    Drawable(const Drawable& other);
    Drawable& operator=(const Drawable& other);
    Drawable(Drawable&&) noexcept = default;
    Drawable& operator=(Drawable&&) noexcept = default;

    // 统一接口
    void draw() const { if (pimpl_) pimpl_->draw(); }

private:
    // ╔════════════════════════════════════════════════════════════════╗
    // ║  Concept/Model 模式 — 外部多态的经典实现                      ║
    // ╚════════════════════════════════════════════════════════════════╝
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        T data;
        explicit Model(T d) : data(std::move(d)) {}
        void draw() const override { data.draw(); }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model<T>>(data);
        }
    };

    std::unique_ptr<Concept> pimpl_;
};

// ── 各种 "可绘制" 类型,彼此没有继承关系 ─────────────────────────────────────
struct Circle {
    double r;
    void draw() const { std::println("  ⭕ Circle(r={:.1f}, area={:.2f})", r, 3.14*r*r); }
};
struct Rectangle {
    double w, h;
    void draw() const { std::println("  🔲 Rectangle({}x{}, area={:.1f})", w, h, w*h); }
};
struct TextLabel {
    std::string text;
    void draw() const { std::println("  📝 Text: \"{}\"", text); }
};

// ═══════════════════════════════════════════════════════════════════════════════
// 示例3: 轻量级类型擦除 — 仅用 std::function (适合单方法接口)
// ═══════════════════════════════════════════════════════════════════════════════

class LightDrawable {
public:
    template <typename T>
    explicit LightDrawable(T obj) {
        draw_fn_ = [obj = std::move(obj)]() { obj.draw(); };
    }
    void draw() const { if (draw_fn_) draw_fn_(); }
private:
    std::function<void()> draw_fn_;
};

} // namespace patterns
