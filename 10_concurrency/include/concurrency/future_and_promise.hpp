#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/future_and_promise.hpp — std::future 与 std::promise           ║
// ║                                                                              ║
// ║  提供:                                                                       ║
// ║    - compute_sum():    模拟耗时计算, 演示 std::async 用法                     ║
// ║    - risky_division(): 模拟可能失败的任务, 演示 future 异常传播               ║
// ║                                                                              ║
// ║  核心概念速查:                                                               ║
// ║    std::async         — 最简单的异步执行, 返回 future                         ║
// ║      launch::async    — 保证在新线程异步执行                                  ║
// ║      launch::deferred — 延迟求值, get() 时才在当前线程同步执行                ║
// ║                                                                              ║
// ║    std::promise<T>    — 承诺方 (生产者), 通过 set_value/set_exception 传结果  ║
// ║    std::future<T>     — 接收方 (消费者), 通过 get() 等待并获取结果             ║
// ║                        ⚠️ get() 只能调用一次, 之后 future 状态变为 invalid     ║
// ║                                                                              ║
// ║    std::packaged_task — 将可调用对象包装成 future, 自动创建 promise-future 对  ║
// ║                        适用: 把已有函数 "future 化" (线程池任务提交)          ║
// ║                                                                              ║
// ║    std::shared_future  — 可共享的 future, get() 可多次调用                     ║
// ║                        适用: 多个线程都需要等待同一个异步结果                 ║
// ║                                                                              ║
// ║    wait_for(timeout)  — 带超时的等待, 返回 future_status:                    ║
// ║      ready     — 结果就绪                                                    ║
// ║      timeout   — 超时未就绪                                                  ║
// ║      deferred  — 任务延迟执行 (launch::deferred)                              ║
// ║                                                                              ║
// ║  异常传播: 如果异步任务抛出异常, future::get() 会重新抛出该异常                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <chrono>
#include <thread>
#include <stdexcept>

/// 模拟耗时计算 — 计算 from 到 to 的等差数列和。
/// 使用 std::async(std::launch::async, compute_sum, ...) 进行异步计算。
///
/// @param from 起始值
/// @param to   结束值
/// @return from 到 to 的累加和 (等差数列求和公式)
inline int compute_sum(int from, int to) {
    using namespace std::chrono_literals;
    std::println("  [计算线程] 开始计算 {} 到 {} 的和...", from, to);
    std::this_thread::sleep_for(300ms);   // 模拟耗时工作
    int sum = (from + to) * (to - from + 1) / 2;  // 等差数列求和公式
    std::println("  [计算线程] 计算完成: sum={}", sum);
    return sum;
}

/// 模拟可能失败的任务 — 整数除法。
/// 当除数为 0 时抛出异常, 演示 future 如何传播异常。
///
/// @param a 被除数
/// @param b 除数
/// @return a / b 的结果
/// @throws std::runtime_error 当 b == 0
inline int risky_division(int a, int b) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100ms);
    if (b == 0) {
        throw std::runtime_error("除数不能为零!");
    }
    return a / b;
}
