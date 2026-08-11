// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/thread_basics.hpp — std::thread 基础                            ║
// ║  提供:                                                                       ║
// ║    - worker(): 作为线程入口的普通函数                                         ║
// ║    - Task:     作为线程入口的仿函数 (可调用对象)                               ║
// ║    - Processor: 演示成员函数作为线程入口的结构体                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <print>
#include <string>
#include <thread>
#include <chrono>

// ── 作为线程入口的普通函数 ──────────────────────────────────────────────────
/// 模拟一个执行若干次循环的工作线程。
/// @param id    线程标识
/// @param loops 循环次数
inline void worker(int id, int loops) {
    using namespace std::chrono_literals;
    for (int i = 1; i <= loops; ++i) {
        std::println("  [线程 {}] 第 {} 次执行", id, i);
        std::this_thread::sleep_for(100ms);   // 模拟工作
    }
}

// ── 作为线程入口的仿函数 (可调用对象) ────────────────────────────────────────
/// Task 是一个可调用对象 (函数对象 / 仿函数)。
/// 将它传递给 std::thread 的构造函数时，它会被拷贝到新线程栈上。
/// 这是 C++ 创建线程的三种方式之一（另两种: 函数指针、Lambda）。
struct Task {
    std::string name;

    /// operator() 定义了该对象被"调用"时的行为。
    /// 在 std::thread 内部，拷贝后的 Task 实例会调用此函数。
    void operator()() const {
        using namespace std::chrono_literals;
        for (int i = 1; i <= 3; ++i) {
            std::println("  [Task '{}'] 步骤 {}", name, i);
            std::this_thread::sleep_for(150ms);
        }
    }
};

// ── 演示成员函数作为线程入口的结构体 ────────────────────────────────────────
/// Processor 演示如何将成员函数作为线程入口运行。
/// 语法: std::thread(&ClassName::method, &instance, args...)
///       - 第一个参数: 成员函数指针
///       - 第二个参数: 实例地址 (或 std::ref 包装)
///       - 后续参数:   传给成员函数的实参
struct Processor {
    int id;

    void process(int count) const {
        using namespace std::chrono_literals;
        for (int i = 1; i <= count; ++i) {
            std::println("  [Processor#{}] 处理 {}/{}", id, i, count);
            std::this_thread::sleep_for(100ms);
        }
    }
};
