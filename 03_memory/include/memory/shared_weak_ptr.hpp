#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: memory/shared_weak_ptr.hpp — 共享所有权与弱引用 (库头文件)       ║
// ║  提供: Named、Node (循环引用演示)、SafeNode (修复版)、Dog (enable_shared_from_this) ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <memory>       // shared_ptr, weak_ptr, make_shared, enable_shared_from_this
#include <string>

/// @brief 一个有名称的类，用于追踪生命周期
/// 构造/析构时打印消息，方便观察 shared_ptr 的引用计数管理
struct Named {
    std::string name;
    Named(std::string n) : name(std::move(n)) {
        std::println("  构造 Named(\"{}\")", name);
    }
    ~Named() { std::println("  析构 Named(\"{}\")", name); }
    void greet() const { std::println("  Hello, 我是 {}", name); }
};

// ═══════════════════════════════════════════════════════════════════════════════
// 场景演示: 循环引用问题
// ═══════════════════════════════════════════════════════════════════════════════
// 父节点持有子节点的 shared_ptr，子节点持有父节点的 shared_ptr
// → 两者互相引用，引用计数永远不为 0 → 内存泄漏

/// @brief 有缺陷的节点: 双向 shared_ptr 导致循环引用 → 内存泄漏!
struct Node {
    std::string label;
    std::shared_ptr<Node> parent;   // 强引用 ← 这是问题所在!
    std::shared_ptr<Node> child;    // 强引用

    Node(std::string lbl) : label(std::move(lbl)) {
        std::println("  构造 Node(\"{}\")", label);
    }
    ~Node() { std::println("  析构 Node(\"{}\")", label); }
};

/// @brief 修复版节点: 用 weak_ptr 打破循环
/// 子节点对父节点使用 weak_ptr (弱引用)，不增加引用计数
struct SafeNode {
    std::string label;
    std::weak_ptr<SafeNode> parent;   // ✅ 弱引用 — 不增加引用计数
    std::shared_ptr<SafeNode> child;  // 强引用

    SafeNode(std::string lbl) : label(std::move(lbl)) {
        std::println("  构造 SafeNode(\"{}\")", label);
    }
    ~SafeNode() { std::println("  析构 SafeNode(\"{}\")", label); }
};

// ═══════════════════════════════════════════════════════════════════════════════
// enable_shared_from_this: 在类内部安全获取自身的 shared_ptr
// ═══════════════════════════════════════════════════════════════════════════════

/// @brief 继承 enable_shared_from_this 的类, 安全地从 this 获取 shared_ptr
/// 直接写 shared_ptr<T>(this) 会创建新的控制块 → 双重释放!
/// 必须在对象已被 shared_ptr 管理后才能调用 shared_from_this()
struct Dog : std::enable_shared_from_this<Dog> {
    std::string name;
    Dog(std::string n) : name(std::move(n)) {
        std::println("  创建了一只狗: {}", name);
    }
    ~Dog() { std::println("  {} 去了汪星", name); }

    /// @brief 返回一个 shared_ptr<Dog> 指向 this，用于注册回调等场景
    std::shared_ptr<Dog> get_shared() {
        return shared_from_this();  // 安全! 不会创建新的控制块
    }

    void bark() const {
        std::println("  {}: 汪汪!", name);
    }
};
