#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 友元与嵌套类 (friend_nested)                            ║
// ║  内容: 友元函数/友元类/嵌套类/前向声明/局部类                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <string_view>
#include <cmath>
#include <ostream>
#include <utility>
#include <memory>

// ── 前向声明 ──
class Account;  // 前向声明: 告诉编译器 Account 是一个类

// ═══════════════════════════════════════════════════════════════════════════════
// Point — 友元函数演示
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: friend 声明允许外部函数或类访问私有/保护成员
// 为什么: ① 运算符重载 (如 operator<<) 需要访问私有成员
//         ② 紧密协作的类/函数之间避免暴露过多公共接口
// 何时用: 非成员运算符重载、工厂函数、紧密耦合的工具函数

class Point {
public:
    Point(double x, double y) : x_(x), y_(y) {}

    void print() const { std::println("({:.0f}, {:.0f})", x_, y_); }

    // 声明三个友元函数
    friend double distance(const Point& a, const Point& b);  // 自由函数
    friend std::ostream& operator<<(std::ostream& os, const Point& p);  // 运算符
    friend Point midpoint(const Point& a, const Point& b);   // 工厂函数

private:
    double x_, y_;
};

// 友元可以访问 private 成员 x_, y_
inline double distance(const Point& a, const Point& b) {
    double dx = a.x_ - b.x_;
    double dy = a.y_ - b.y_;
    return std::sqrt(dx * dx + dy * dy);
}

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << '(' << p.x_ << ", " << p.y_ << ')';  // 直接访问私有成员
}

inline Point midpoint(const Point& a, const Point& b) {
    return Point((a.x_ + b.x_) / 2, (a.y_ + b.y_) / 2);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Account / Bank / AuditLogger — 友元类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 声明 class B 为 class A 的友元 → B 的所有成员函数可访问 A 的私有成员
// 为什么: 两个类紧密耦合时 (如 LinkedList 和 Node)，避免暴露实现细节
// 何时用: 链表节点/迭代器、设计模式中的工厂、测试类访问内部状态

class Account {
public:
    Account(std::string owner, double balance)
        : owner_(std::move(owner)), balance_(balance) {}

    // 声明 Bank 为友元类 — Bank 可以访问 Account 的私有成员
    friend class Bank;

    // 声明 AuditLogger 为友元 — 审计日志需要读取敏感数据
    friend class AuditLogger;

private:
    std::string owner_;
    double balance_;

    // 只有友元 Bank 和 AuditLogger 能调用
    void adjust_balance(double delta) { balance_ += delta; }
};

class Bank {
public:
    // Bank 可以直接操作 Account 的私有成员
    static void transfer(Account& from, Account& to, double amount) {
        if (from.balance_ >= amount) {
            from.adjust_balance(-amount);  // 访问私有成员
            to.adjust_balance(+amount);    // 访问私有方法
            std::println("转账成功: {:.0f} 元", amount);
        }
    }

    static double get_balance(const Account& acc) {
        return acc.balance_;  // 友元可读私有成员
    }
};

class AuditLogger {
public:
    static void log(const Account& acc) {
        std::println("  [审计] 账户: {}, 余额: {:.0f}", acc.owner_, acc.balance_);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Tree + 嵌套 Node / Iterator — 类中定义类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 一个类定义在另一个类的内部 (类似 namespace 的类版本)
// 为什么: ① 表达"从属"关系 (Node 属于 Tree)
//         ② 隐藏实现细节 (iterator 作为嵌套类)
// 何时用: 迭代器、节点、内部实现辅助类、Pimpl 惯用法

class Tree {
public:
    explicit Tree(int root_value) : root_(new Node(root_value)) {}
    ~Tree() { delete root_; }

    // 嵌套类: 自动成为 Tree 的友元
    class Node {
    public:
        explicit Node(int v) : value(v) {}
        int value;
        Node* left  = nullptr;
        Node* right = nullptr;
    };

    Node* root() { return root_; }
    void insert(int value) { insert_impl(root_, value); }

    // 嵌套的迭代器类
    class Iterator {
    public:
        explicit Iterator(Node* node) : current_(node) {}
        int operator*() const { return current_->value; }
        bool has_next() const { return current_ != nullptr; }
        void next() { current_ = nullptr; } // 简化版本
    private:
        Node* current_;
    };

    Iterator begin() { return Iterator(root_); }

private:
    Node* root_;

    void insert_impl(Node* node, int value) {
        if (value < node->value) {
            if (node->left) insert_impl(node->left, value);
            else node->left = new Node(value);
        } else {
            if (node->right) insert_impl(node->right, value);
            else node->right = new Node(value);
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Engine / Car — 前向声明解决循环依赖
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 前向声明 → 只声明类名不定义
// 为什么: 打破循环依赖, 减少编译时间
// 何时用: 两个类互相引用对方时

class Engine;  // 前向声明
class Car {
public:
    void set_engine(Engine* e) { engine_ = e; }
    std::string describe() const;
private:
    Engine* engine_ = nullptr;  // 只用到指针, 前向声明足够
};

class Engine {
public:
    explicit Engine(int hp) : horsepower_(hp) {}
    int horsepower() const { return horsepower_; }
private:
    int horsepower_;
};

// Car::describe 定义在后面 (需要完整的 Engine 定义)
inline std::string Car::describe() const {
    if (engine_)
        return "汽车, 引擎马力: " + std::to_string(engine_->horsepower());
    return "汽车, 无引擎";
}
