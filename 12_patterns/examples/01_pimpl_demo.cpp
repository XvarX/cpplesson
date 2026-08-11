// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_pimpl_demo.cpp — Pimpl 惯用法演示                                       ║
// ║  编译防火墙：隐藏实现细节，减少头文件依赖，加速编译                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 核心概念:
//   - 公开头文件只暴露接口，实现细节放在 .cpp 中
//   - 通过 unique_ptr 持有前向声明的实现类
//   - 修改实现类时不需要重新编译使用 Widget 的代码
//   - 保持 ABI 稳定：实现类的内存布局变化不影响调用方
// 适用场景:
//   - 库的公开 API 头文件
//   - 成员较多且经常变化的类
//   - 需要隐藏第三方库依赖时
// 常见陷阱:
//   - unique_ptr + 前向声明需要显式析构函数(放在 .cpp 中)
//   - 增加了间接访问开销(一次指针解引用)
//   - 实现类的所有成员函数调用都需要通过 pimpl 指针

#include "patterns/pimpl.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <memory>
#include <string>

auto main() -> int {
    lesson::print_header("01  Pimpl 惯用法 — 编译防火墙");

    // ── 基本使用 ──────────────────────────────────────────────────────────────
    lesson::print_subtitle("1. 创建和使用 Widget");
    patterns::Widget w("MyWidget");
    w.set_value(42);
    w.print_state();
    std::println("  get_value() = {}", w.get_value());
    std::println("  get_name()  = {}", w.get_name());

    // ── 移动语义 ──────────────────────────────────────────────────────────────
    lesson::print_subtitle("2. 移动语义");
    patterns::Widget w2("Second");
    w2.set_value(100);
    std::print("  w2: "); w2.print_state();

    patterns::Widget w3 = std::move(w2);  // w2 被移动,内部 pimpl_ 转移所有权
    std::print("  w3 (moved from w2): "); w3.print_state();
    // ⚠️ w2 现在处于"有效但未指定"状态,不应再使用

    // ── 多个 Widget 各自持有独立的 Impl ───────────────────────────────────────
    lesson::print_subtitle("3. 独立实例");
    patterns::Widget a("Alice");
    patterns::Widget b("Bob");
    a.set_value(10);
    b.set_value(20);
    std::print("  a: "); a.print_state();
    std::print("  b: "); b.print_state();

    // ═══════════════════════════════════════════════════════════════════
    // 编译防火墙效果演示(概念性):
    // 假设 Impl 中包含 #include <heavy_library.hpp>
    // 如果不用 Pimpl, 任何 #include "widget.hpp" 的文件都得解析 heavy_library.hpp
    // 用了 Pimpl, 只有 widget.cpp 需要解析它
    // ═══════════════════════════════════════════════════════════════════

    lesson::print_separator();

    // ── 常见陷阱说明 ──────────────────────────────────────────────────────────
    lesson::print_subtitle("常见陷阱");

    std::println("⚠️  陷阱1: 在头文件中使用 = default 析构函数");
    std::println("   class Widget {{ ~Widget() = default; ... }};  // 编译错误!");
    std::println("   解决: 在头文件中声明 ~Widget(); 在 .cpp 中定义 ~Widget() = default;");

    std::println("\n⚠️  陷阱2: 忘记实现移动构造/赋值");
    std::println("   编译器自动生成的移动操作会尝试访问不完整类型 Impl");

    std::println("\n⚠️  陷阱3: 性能开销");
    std::println("   每次访问成员都经过一次指针间接访问 (pimpl_->xxx)");
    std::println("   不适合极度性能敏感的 getter/setter");

    std::println("\n⚠️  陷阱4: 与虚函数的关系");
    std::println("   Pimpl 和虚函数解决不同问题: Pimpl 隐藏实现,虚函数支持多态");
    std::println("   两者可以结合使用");

    // ── 练习 ──────────────────────────────────────────────────────────────────
    lesson::print_subtitle("练习");
    std::println("1. 为 Widget 添加一个 set_value_range(int min, int max) 方法");
    std::println("2. 尝试在头文件中写 ~Widget() = default,观察编译错误");
    std::println("3. 思考: 如果 Widget 需要拷贝,应该如何实现?(提示: 深拷贝 Impl)");
    std::println("4. 用 shared_ptr 代替 unique_ptr 实现 Pimpl,对比差异");
    std::println("5. 测量: 修改 Impl 添加一个 std::vector<int> 成员,时间编译时间");

    return 0;
}
