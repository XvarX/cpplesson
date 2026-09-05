#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 虚函数与多态 (polymorphism)                             ║
// ║  内容: 虚函数/vtable/纯虚函数/抽象类/override/final/虚析构/dynamic_cast    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <vector>
#include <memory>

// ═══════════════════════════════════════════════════════════════════════════════
// NonVirtualBase / NonVirtualDerived — 非虚函数的静态绑定 (对比用)
// ═══════════════════════════════════════════════════════════════════════════════

class NonVirtualBase {
public:
    void speak() const { std::println("基类说话"); }
};

class NonVirtualDerived : public NonVirtualBase {
public:
    void speak() const { std::println("派生类说话"); } // 隐藏, 不是覆盖!
};

// ═══════════════════════════════════════════════════════════════════════════════
// Shape / Circle / Rectangle — 虚函数与多态
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: virtual 标记的函数通过 vtable 在运行时决定调用哪个版本
// 为什么: 让"基类指针/引用调用派生类方法"成为可能 — 多态的核心
// 何时用: 希望派生类可以覆盖 (override) 的行为

class Shape {
public:
    virtual ~Shape() = default;  // 虚析构 (见 Part 4)

    virtual double area() const { return 0.0; }  // 虚函数
    virtual std::string name() const { return "形状"; }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : radius_(r) {}

    double area() const override { return 3.14159 * radius_ * radius_; }
    std::string name() const override { return "圆形"; }

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}

    double area() const override { return width_ * height_; }
    std::string name() const override { return "矩形"; }

private:
    double width_, height_;
};

// 多态调用: 通过基类引用在运行时决定调用哪个版本
inline void print_shape_info(const Shape& shape) {
    std::println("{} 的面积: {:.2f}", shape.name(), shape.area());
}

// ═══════════════════════════════════════════════════════════════════════════════
// Animal / Cat / Duck — 纯虚函数与抽象类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 纯虚函数 ( = 0) 使类成为抽象类，不能实例化
// 为什么: 强制派生类实现特定接口 (类似于 Java 的 interface)
// 何时用: 定义接口规范，派生类必须实现

class AbstractAnimal {  // 抽象类: 有纯虚函数
public:
    virtual ~AbstractAnimal() = default;

    // 纯虚函数 = 0 → 派生类必须实现
    virtual std::string sound() const = 0;
    virtual std::string species() const = 0;

    // 非纯虚函数可以提供默认实现
    void describe() const {
        std::println("{} 的叫声是 {}", species(), sound()); // 多态调用!
    }
};

class Cat : public AbstractAnimal {
public:
    std::string sound()   const override { return "喵喵"; }
    std::string species() const override { return "猫"; }
};

class Duck : public AbstractAnimal {
public:
    std::string sound()   const override { return "嘎嘎"; }
    std::string species() const override { return "鸭子"; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Vehicle / SportsCar / SuperSportsCar — override 和 final
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: override 标记覆盖基类虚函数; final 阻止进一步覆盖
// 为什么: override → 编译期检查，防止签名写错; final → 锁定行为
// 何时用: 每个覆盖基类虚函数的函数都应加 override

class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual void start() { std::println("Vehicle 启动"); }
    virtual int max_speed() const { return 120; }
};

class SportsCar : public Vehicle {
public:
    void start() override { std::println("跑车轰鸣启动!"); }
    // void strat() override {} // ❌ 编译错误: 拼写错误被 override 捕获!

    int max_speed() const final { return 350; } // final: 子类不能再覆盖
};

class SuperSportsCar : public SportsCar {
public:
    void start() override { std::println("超级跑车启动!!"); }
    // int max_speed() const override {}  // ❌ 编译错误: 被 final 阻止
};

// ═══════════════════════════════════════════════════════════════════════════════
// BadBase / BadDerived / GoodBase / GoodDerived — 虚析构的必要性
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 基类析构函数标记 virtual，确保 delete 基类指针时调用正确的析构
// 为什么: 非虚析构通过基类指针 delete → 只调用基类析构 → 资源泄漏!
// 何时用: 任何可能被继承并多态使用的类，析构函数必须 virtual

class BadBase {
public:
    ~BadBase() { std::println("~BadBase"); } // 非虚析构!
};

class BadDerived : public BadBase {
public:
    ~BadDerived() { std::println("~BadDerived — 资源释放"); }
};

class GoodBase {
public:
    virtual ~GoodBase() { std::println("~GoodBase"); } // 虚析构!
};

class GoodDerived : public GoodBase {
    std::string* data_ = new std::string("数据");
public:
    ~GoodDerived() override {
        delete data_;
        std::println("~GoodDerived — 正确释放资源");
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Media / Audio / Video — dynamic_cast 安全的向下转型
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: dynamic_cast<T*> 尝试将基类指针/引用转为派生类
// 为什么: 安全的运行时类型检查，失败返回 nullptr (指针) 或抛异常 (引用)
// 何时用: 需要确认具体派生类型时 (但优先用虚函数而非类型判断)

class Media {
public:
    virtual ~Media() = default;
    virtual std::string type() const = 0;
};

class Audio : public Media {
public:
    std::string type() const override { return "音频"; }
    void play_audio() { std::println("播放音频..."); }
};

class Video : public Media {
public:
    std::string type() const override { return "视频"; }
    void play_video() { std::println("播放视频..."); }
};
