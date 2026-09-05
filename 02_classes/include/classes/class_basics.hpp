#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类的声明与基础 (class_basics)                          ║
// ║  内容: class vs struct / 访问控制 / this 指针 / static / const              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <string_view>
#include <utility>

// ═══════════════════════════════════════════════════════════════════════════════
// Point — struct 定义的数据类 (默认 public)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: struct 也能定义类，默认 public 成员和 public 继承
// 为什么: 适合纯数据聚合 (POD)
// 何时用: 纯数据聚合用 struct；有行为和数据隐藏用 class

struct Point {
    double x, y; // struct: 默认 public
};

// ═══════════════════════════════════════════════════════════════════════════════
// Circle — class 定义的类 (默认 private)
// ═══════════════════════════════════════════════════════════════════════════════

class Circle {
    double radius_; // class: 默认 private!
public:
    void set_radius(double r) { radius_ = r; }
    double area() const { return 3.14159 * radius_ * radius_; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// BankAccount — public / private 访问控制演示
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 三种访问修饰符控制类成员的可见性
// 为什么: 封装是实现"最小暴露原则"的基础，隐藏内部细节
// 何时用: public: 对外接口; private: 内部实现; protected: 给子类访问

class BankAccount {
public:   // ── 外部可见 ──
    BankAccount(std::string owner, double initial)
        : owner_(std::move(owner)), balance_(initial) {}

    void deposit(double amount) { if (amount > 0) balance_ += amount; }
    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance_) { balance_ -= amount; return true; }
        return false;
    }
    double balance() const { return balance_; } // 只读访问器

private:  // ── 外部不可见 ──
    std::string owner_;
    double balance_;

    void log_transaction(const std::string& type, double amount) {
        // 内部日志记录，外部无需关心
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Counter — this 指针与链式调用
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: this 是成员函数内部隐含的指针，指向调用该函数的对象
// 为什么: 区分成员和参数、返回自身引用、链式调用
// 何时用: 参数名与成员同名时、链式调用、需要返回 *this

class Counter {
public:
    Counter& reset() { count_ = 0; return *this; }     // 返回 *this 实现链式
    Counter& increment() { ++count_; return *this; }
    Counter& add(int n) { count_ += n; return *this; }

    // 使用 this 显式区分成员与参数 (当名称相同时)
    Counter& set_count(int count) {
        this->count_ = count; // this->count_ 是成员, count 是参数
        return *this;
    }
    int value() const { return count_; }

private:
    int count_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// Student — static 成员 (属于类而非对象)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: static 成员变量在所有对象间共享，static 方法无需对象即可调用
// 为什么: 计数、缓存、单例、不需要访问实例状态的辅助函数
// 何时用: 需要类级别的共享状态或工具函数时

class Student {
public:
    Student(std::string name) : name_(std::move(name)) { ++total_count_; }
    ~Student() { --total_count_; }

    static int total() { return total_count_; }  // static 成员函数
    std::string_view name() const { return name_; }

private:
    std::string name_;
    static inline int total_count_ = 0;  // C++17 inline static 初始化
};

// ═══════════════════════════════════════════════════════════════════════════════
// Temperature — const 成员函数
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 函数签名后加 const，承诺不修改任何非 mutable 成员
// 为什么: 允许在 const 对象上调用；编译器强制检查，防止意外修改
// 何时用: 所有不修改对象状态的访问器(getter)都应标记为 const

class Temperature {
public:
    explicit Temperature(double celsius) : celsius_(celsius) {}

    double get_celsius() const { return celsius_; }     // ✅ const 访问器
    double get_fahrenheit() const { return celsius_ * 9 / 5 + 32; }

    void set_celsius(double v) { celsius_ = v; }        // 修改器不能是 const

    // mutable 成员即使在 const 函数中也能修改 (例如缓存)
    mutable int read_count = 0;
    double cached_fahrenheit() const {
        ++read_count;  // mutable 允许在 const 中修改
        return get_fahrenheit();
    }

private:
    double celsius_;
};
