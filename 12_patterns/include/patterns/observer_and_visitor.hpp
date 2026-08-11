// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  observer_and_visitor.hpp — 观察者模式与访问者模式                         ║
// ║  事件通知 + 算法与数据结构分离                                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 观察者模式(Observer):
//   - 一对多依赖: 主题状态改变时通知所有观察者
//   - C++ 中用 std::function 或信号/槽实现
// 访问者模式(Visitor):
//   - 算法与数据结构分离: 不修改数据结构就能添加新操作
//   - C++ 现代化: std::variant + std::visit + overloaded 模式
// 命令模式(Command):
//   - 将请求封装为对象,支持撤销、队列等
// 适用场景:
//   - 观察者: GUI 事件、发布订阅、数据绑定
//   - 访问者: AST 遍历、序列化

#pragma once

#include <print>
#include <string>
#include <vector>
#include <variant>
#include <functional>
#include <memory>

namespace patterns {

// ═══════════════════════════════════════════════════════════════════════════════
// 第一部分: 观察者模式 — 信号/槽
// ═══════════════════════════════════════════════════════════════════════════════

/// 点击事件的数据结构
struct ClickEvent {
    std::string source;
    int         count = 0;
};

/// 泛型信号类: 支持多个槽(slot)连接,发出事件时通知所有槽
/// 模板参数 Args... 是事件参数类型
template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;

    /// 连接一个槽,返回连接ID(用于后续断开)
    int connect(Slot slot) {
        slots_.push_back(std::move(slot));
        return next_id_++;
    }

    /// 断开指定ID的槽连接
    void disconnect(int id) {
        if (id >= 0 && static_cast<size_t>(id) < slots_.size())
            slots_[id] = nullptr;  // 惰性清理
    }

    /// 发出事件,通知所有已连接的槽
    void emit(Args... args) {
        for (auto& slot : slots_)
            if (slot) slot(args...);
    }

private:
    std::vector<Slot> slots_;
    int next_id_ = 0;
};

/// 按钮类: 被点击时发出 ClickEvent 信号
class Button {
public:
    explicit Button(std::string id) : id_(std::move(id)) {}

    void click() {
        ++count_;
        std::println("  [Button \"{}\"] 点击(#{})", id_, count_);
        on_click.emit(ClickEvent{id_, count_});
    }

    Signal<const ClickEvent&> on_click;

private:
    std::string id_;
    int count_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// 第二部分: 访问者模式 — std::variant + std::visit
// ═══════════════════════════════════════════════════════════════════════════════

// ── AST 节点: 用 variant 代替继承 ─────────────────────────────────────────

struct NumNode { double value; };
struct AddNode {
    std::shared_ptr<std::variant<NumNode, AddNode, struct MulNode>> left, right;
};
struct MulNode {
    std::shared_ptr<std::variant<NumNode, AddNode, MulNode>> left, right;
};
using Expr = std::variant<NumNode, AddNode, MulNode>;

// ── 构造 AST 的辅助函数 ────────────────────────────────────────────────────

inline auto num(double v) { return std::make_shared<Expr>(NumNode{v}); }
inline auto add(auto l, auto r) {
    return std::make_shared<Expr>(AddNode{std::move(l), std::move(r)});
}
inline auto mul(auto l, auto r) {
    return std::make_shared<Expr>(MulNode{std::move(l), std::move(r)});
}

// ── "overloaded" 模式: 组合多个 Lambda 为过载集 ────────────────────────────
template <typename... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// ── 求值访问者 ──────────────────────────────────────────────────────────────
inline double eval(const Expr& node) {
    return std::visit(overloaded{
        [](const NumNode& n) { return n.value; },
        [](const AddNode& a) { return eval(*a.left) + eval(*a.right); },
        [](const MulNode& m) { return eval(*m.left) * eval(*m.right); },
    }, node);
}

// ── 打印访问者: 不修改数据结构,就能添加新操作! ─────────────────────────────
inline std::string to_str(const Expr& node) {
    return std::visit(overloaded{
        [](const NumNode& n) {
            return n.value == static_cast<int>(n.value)
                       ? std::to_string(static_cast<int>(n.value))
                       : std::to_string(n.value);
        },
        [](const AddNode& a) {
            return "(" + to_str(*a.left) + " + " + to_str(*a.right) + ")";
        },
        [](const MulNode& m) {
            return "(" + to_str(*m.left) + " * " + to_str(*m.right) + ")";
        },
    }, node);
}

// ── 节点计数访问者: 再添加一个操作,仍然不需要修改节点定义! ─────────────────
inline int count_nodes(const Expr& node) {
    return std::visit(overloaded{
        [](const NumNode&) { return 1; },
        [](const AddNode& a) {
            return 1 + count_nodes(*a.left) + count_nodes(*a.right);
        },
        [](const MulNode& m) {
            return 1 + count_nodes(*m.left) + count_nodes(*m.right);
        },
    }, node);
}

} // namespace patterns
