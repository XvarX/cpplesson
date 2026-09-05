#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/jthread_and_parallel.hpp — C++20 并行原语                        ║
// ║                                                                              ║
// ║  本模块对比 std::thread 与 C++20 新增的并行工具:                              ║
// ║                                                                              ║
// ║  std::jthread (C++20) — 自动 join 的线程:                                     ║
// ║    - 析构时自动调用 join() (如果 joinable) — 不会 std::terminate              ║
// ║    - 内置 stop_token 机制 — 支持协作式取消                                    ║
// ║    - 线程函数第一个参数如果是 stop_token, jthread 自动传入                    ║
// ║    - jthread = thread + RAII join + stop_token                                ║
// ║                                                                              ║
// ║  stop_token / stop_source (C++20) — 协作式线程取消:                            ║
// ║    stop_source     — 发出停止请求的一端 (jthread 内部持有)                    ║
// ║    stop_token      — 检查是否被请求停止的一端 (传递给线程函数)                ║
// ║    stop_callback   — 注册停止时的回调 (RAII 管理)                             ║
// ║    ⚠️ 协作式: 线程必须主动检查 stop_token, 不会被强制杀死                     ║
// ║                                                                              ║
// ║  std::latch (C++20) — 一次性倒计数门闩:                                       ║
// ║    - 倒计数器, 所有线程等待计数器归零后一起通过                                ║
// ║    - 一次性使用, 不能重置                                                     ║
// ║    - 同一个线程可以多次 count_down                                             ║
// ║    - 适用: "所有初始化完成后, 主线程才开始处理"                                ║
// ║                                                                              ║
// ║  std::barrier (C++20) — 多阶段同步点:                                         ║
// ║    - 可重用同步点, 所有线程到达后一起释放, 进入下一阶段                        ║
// ║    - 有"完成回调" — 每个阶段开始时调用                                         ║
// ║    - 适用: 并行算法中的迭代同步, 模拟/游戏中每帧同步                           ║
// ║                                                                              ║
// ║  std::counting_semaphore (C++20) — 资源访问限制:                               ║
// ║    - 维护内部计数器, acquire() 减 1, release() 加 1                           ║
// ║    - 适用: 限制并发连接数 / 打开文件数 / 线程池队列长度                        ║
// ║    - binary_semaphore (N=1): 可用于线程间信号传递 (不要求同一线程 lock/unlock) ║
// ║                                                                              ║
// ║  对比总结:                                                                   ║
// ║    latch    — 一次性, 不可重置                                                ║
// ║    barrier  — 可重用, 有阶段回调                                              ║
// ║    semaphore — 计数型, acquire/release 可以跨线程, 不需要配对                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <chrono>
#include <print>
#include <stop_token>

/// 一个检查 stop_token 的工作函数, 演示协作式取消的基本模式。
///
/// jthread 会自动传入 stop_token 给线程函数 (如果第一个参数是 stop_token)。
/// 线程应定期检查 stop_requested(), 并在收到停止请求后优雅退出。
///
/// @param stoken 由 jthread 自动传入的 stop_token
/// @param max_steps 最大执行步数 (如果没被取消)
/// @return 实际执行的步数
inline int cancellable_work(std::stop_token stoken, int max_steps = 20) {
    using namespace std::chrono_literals;
    int step = 0;
    while (step < max_steps) {
        // stop_requested() — 检查是否收到了停止请求
        if (stoken.stop_requested()) {
            std::println("  [工作线程] 收到停止请求, 在步骤 {} 退出", step);
            return step;
        }
        std::println("  [工作线程] 步骤 {} ...", ++step);
        std::this_thread::sleep_for(80ms);
    }
    std::println("  [工作线程] 自然完成所有 {} 步", max_steps);
    return step;
}
