// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: 协程基础 — co_await/co_yield/co_return、promise_type、generator      ║
// ║  目标: 理解协程的核心组件，实现简易 generator<T>，掌握 awaiter 概念          ║
// ║                                                                            ║
// ║  学习方式: Generator<T> 是协程库的核心，已在 generator.hpp 中实现。          ║
// ║  下面的代码演示如何使用 co_yield 写出类似 Python yield 的协程函数。          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/generator.hpp"
#include <print>
#include <utility>

// ============================================================================
// 协程函数 — 使用 co_yield 的 generator
// ============================================================================
// 注意: 函数体中出现 co_await/co_yield/co_return 时，编译器将其识别为协程。

Generator<int> fibonacci(int limit) {
    int a = 0, b = 1;
    while (a <= limit) {
        co_yield a;                  // 产出当前值并挂起
        int next = a + b;
        a = b;
        b = next;
    }
}

Generator<int> range_gen(int start, int end) {
    for (int i = start; i < end; ++i)
        co_yield i;
}

int main() {
    lesson::print_header("C++20 协程基础");

    // ── Part 1 & 2: 使用 generator ──
    lesson::print_subtitle("Part 1 & 2: Generator<T> — co_yield 生成序列");

    std::println("  fibonacci(<= 100):");
    std::print("    ");
    for (auto v : fibonacci(100)) {
        std::print("{} ", v);
    }
    std::println("");

    std::println("  range_gen(5, 15):");
    std::print("    ");
    for (auto v : range_gen(5, 15)) {
        std::print("{} ", v);
    }
    std::println("");

    // ── Part 3: awaiter 机制 ──
    lesson::print_subtitle("Part 3: Awaiter 概念 — co_await 背后的三个方法");

    std::println("  await_ready()   — 返回 true 则不挂起，直接跳过");
    std::println("  await_suspend() — 挂起时调用，可注册回调/调度");
    std::println("  await_resume()  — 恢复时调用，返回 co_await 的结果");

    lesson::print_note("std::suspend_always 的 await_ready() 始终返回 false (总是挂起)");
    lesson::print_note("std::suspend_never 的 await_ready() 始终返回 true (从不挂起)");

    // ── 演示协程不可拷贝 ──
    lesson::print_subtitle("协程不可拷贝");
    auto gen = fibonacci(10);
    // auto gen2 = gen;            // 编译错误! 删除了拷贝构造
    auto gen2 = std::move(gen);    // OK: 移动语义
    std::println("  协程状态已移动，原 gen 失效: done() = {}", gen.done());

    std::print("  移动后的 gen2: ");
    for (auto v : gen2) std::print("{} ", v);
    std::println("");

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. 协程不可拷贝 — 复制 lambada/引用捕获需谨慎");
    std::println("  2. promise_type::get_return_object() 返回悬空引用是常见错误");
    std::println("  3. 忘记在析构函数中 destroy handle 会导致内存泄漏");
    std::println("  4. co_await 挂起时必须确保引用的对象生命周期长于协程");
    std::println("  5. 协程是\"半函数半对象\" — 分为创建/执行/销毁三个阶段");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 修改 Generator 使其支持 co_return 返回最终值");
    std::println("  2. 实现一个打印日志的 LogAwaiter，await_ready 返回 false");
    std::println("     在 await_suspend 中打印 \"协程已挂起\"，在 await_resume 中打印 \"协程已恢复\"");
    std::println("  3. 写一个协程，用 co_await 模拟等待异步结果 (用简单的计数器模拟)");
}
