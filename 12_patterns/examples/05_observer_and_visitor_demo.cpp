// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  05_observer_and_visitor_demo.cpp — 观察者模式与访问者模式演示               ║
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

#include "patterns/observer_and_visitor.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>
#include <variant>
#include <functional>
#include <memory>

auto main() -> int {
    lesson::print_header("05  观察者模式与访问者模式");

    // ── 观察者: 信号/槽 ──────────────────────────────────────────────────
    lesson::print_subtitle("1. 观察者模式 — 信号/槽");
    patterns::Button btn("submit");

    auto c1 = btn.on_click.connect([](const patterns::ClickEvent& e) {
        std::println("  -> [Logger] 记录: {}", e.source);
    });
    btn.on_click.connect([](const patterns::ClickEvent& e) {
        std::println("  -> [Analytics] 统计: {} (#{})", e.source, e.count);
    });

    btn.click();
    btn.click();

    std::println("\n  断开 Logger 后:");
    btn.on_click.disconnect(c1);
    btn.click();  // 只有 Analytics 收到

    // ── 访问者: variant + visit ──────────────────────────────────────────
    lesson::print_subtitle("2. 访问者模式 — std::variant + std::visit");

    // 构建: (3 + 4) * (2 + 6)
    auto expr1 = patterns::mul(
        patterns::add(patterns::num(3), patterns::num(4)),
        patterns::add(patterns::num(2), patterns::num(6))
    );
    std::println("  表达式: {}", patterns::to_str(*expr1));
    std::println("  结果:   {}", patterns::eval(*expr1));
    std::println("  节点数: {}", patterns::count_nodes(*expr1));

    // ── 命令模式(简要): 撤销 ──────────────────────────────────────────────
    lesson::print_subtitle("3. 命令模式 — 撤销(简要)");
    int val = 0;
    std::vector<std::function<void()>> undos;

    std::println("  初始: {}", val);
    auto do_add = [&] { val += 10; std::println("  +10 → {}", val); };
    auto undo_add = [&] { val -= 10; std::println("  撤销 → {}", val); };
    do_add();
    undos.push_back(undo_add);
    undo_add();

    // ── 总结与练习 ────────────────────────────────────────────────────────
    lesson::print_subtitle("模式总结");
    std::println("  观察者: 一对多事件通知,解耦事件源和处理器");
    std::println("  访问者: 不修改数据结构,添加新操作 (std::visit)");
    std::println("  命令:   封装操作,支持撤销/重做/队列(可扩展为完整版本)");

    lesson::print_subtitle("常见陷阱");
    std::println("⚠️  观察者: 通知过程中修改观察者列表 → 迭代器失效");
    std::println("⚠️  观察者: 主题析构后观察者仍持有引用 → 悬空指针");
    std::println("⚠️  variant: 必须为每种类型提供处理函数,否则编译错误");
    std::println("⚠️  访问者: 添加新\"类型\"需修改所有访问者(固有权衡)");

    lesson::print_subtitle("练习");
    std::println("1. 为 Signal 添加线程安全支持(std::mutex)");
    std::println("2. 实现 EventBus: 按事件类型分发到对应处理器");
    std::println("3. 为 AST 添加 SubtractNode 和 DivideNode,更新所有访问者");
    std::println("4. 用观察者模式实现一个简单的 Model-View 数据绑定");
    std::println("5. 实现完整的 CmdHistory 类(支持 undo/redo)");

    return 0;
}
