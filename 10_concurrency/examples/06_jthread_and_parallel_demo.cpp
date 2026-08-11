// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 06: jthread 与并行原语 (C++20)                              ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握 std::jthread — 自动 join 的线程 (告别 std::terminate)             ║
// ║    2. 理解 stop_token / stop_source — 协作式线程取消                         ║
// ║    3. 使用 std::latch — 一次性倒计数门闩                                     ║
// ║    4. 使用 std::barrier — 多阶段同步点                                       ║
// ║    5. 使用 std::counting_semaphore — 限制并发资源访问                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <latch>
#include <barrier>
#include <semaphore>
#include <stop_token>
#include <vector>
#include <chrono>
#include <atomic>
#include <functional>

#include "shared/lesson_utils.hpp"
#include "concurrency/jthread_and_parallel.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.6  jthread 与并行原语 (C++20)");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: std::jthread (C++20) — 自动 join 的线程
    // ═══════════════════════════════════════════════════════════════════════════
    // std::thread 的两个核心问题:
    //   ① 忘记 join/detach → 析构时 std::terminate (程序直接崩溃)
    //   ② 无法优雅地请求线程停止 — 只能通过共享标志 + 轮询
    //
    // std::jthread (joining thread) 解决了这两个问题:
    //   ① 析构时自动调用 join() (如果 joinable) — 不会崩溃
    //   ② 内置 stop_token 机制 — 支持协作式取消
    //
    // jthread = thread + RAII join + stop_token
    lesson::print_subtitle("Part 1: jthread — 自动 join, 告别 terminate");

    // ── 基本用法: 和 std::thread 一样, 但不需要手动 join ──
    {
        std::jthread jt([] {
            std::println("  [jthread] 正在工作...");
            std::this_thread::sleep_for(200ms);
            std::println("  [jthread] 工作完成");
        });
        std::println("  [主线程] jthread 创建完毕, 离开作用域时将自动 join");
        // jt 析构时自动 join — 不需要写 jt.join() !
    }
    std::println("  [主线程] jthread 已自动 join 完成");

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: stop_token / stop_source — 协作式线程取消
    // ═══════════════════════════════════════════════════════════════════════════
    // jthread 内置停止机制 — 不需要手动设置 shared_flag:
    //
    // 三件套:
    //   stop_source      — 发出停止请求的一端 (jthread 内部持有)
    //   stop_token       — 检查是否被请求停止的一端 (传递给线程函数)
    //   stop_callback    — 注册停止时的回调 (RAII)
    //
    // 关键概念: 协作式取消 — 线程必须主动检查 stop_token, 不会被强制杀死
    lesson::print_subtitle("Part 2: stop_token — 优雅停止线程");

    // ── 示例 1: jthread 自动传递 stop_token ──
    // 线程函数的第一个参数如果是 std::stop_token, jthread 自动传入
    {
        // 使用库中的 cancellable_work 函数
        std::jthread worker([](std::stop_token stoken) {
            int step = 0;
            while (step < 20) {
                // stop_requested() — 检查是否收到了停止请求
                if (stoken.stop_requested()) {
                    std::println("  [工作线程] 收到停止请求, 在步骤 {} 退出", step);
                    return;  // 协作式退出 — 线程自己决定何时退出
                }
                std::println("  [工作线程] 步骤 {} ...", ++step);
                std::this_thread::sleep_for(80ms);
            }
            std::println("  [工作线程] 自然完成");
        });

        std::this_thread::sleep_for(400ms);
        std::println("  [主线程] 请求工作线程停止...");

        // request_stop() — 发出停止请求 (通过 jthread 的 stop_source)
        // 也可以不在主线程调用: worker.get_stop_source().request_stop();
        worker.request_stop();

        // jthread 析构时:
        //   ① 先调用 request_stop()
        //   ② 再调用 join()
        // 所以即使不手动 request_stop, 析构时也会自动发出停止请求
    }
    std::println("  [主线程] worker 已退出");

    lesson::print_separator("stop_callback 演示");

    // ── 示例 2: stop_callback — 注册停止时的清理动作 ──
    {
        std::stop_source source;
        std::stop_token token = source.get_token();

        // stop_callback: 当 stop 被请求时自动调用, RAII 管理
        // 可以在回调中做清理工作: 关闭文件、释放资源、通知其他线程等
        std::stop_callback cleanup_cb(token, [] {
            std::println("  [回调] 执行清理工作: 关闭资源、保存状态...");
        });

        std::jthread worker([token] {
            int count = 0;
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(50ms);
                ++count;
            }
            std::println("  [工作线程] 共执行 {} 次迭代后退出", count);
        });

        std::this_thread::sleep_for(200ms);
        std::println("  [主线程] 发出停止请求 (回调将被触发)");
        source.request_stop();  // 触发 stop_callback
    }

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: std::latch (C++20) — 一次性倒计数门闩
    // ═══════════════════════════════════════════════════════════════════════════
    // latch 是一个倒计数器，所有线程等待计数器归零后一起通过。
    //
    // 使用场景:
    //   ① "所有初始化完成后, 主线程才开始处理"
    //   ② "等 N 个子任务完成后再汇总"
    //
    // 与 barrier 的区别:
    //   latch:  一次性使用, 不能重置, 同一个线程可以多次 count_down
    //   barrier: 可重用 (多阶段), 每个线程每阶段只能 arrive 一次
    lesson::print_subtitle("Part 3: std::latch — 一次性倒计数门闩");

    const int num_workers = 3;
    // 初始化 latch, 计数为 num_workers + 1 (额外 1 是主线程)
    std::latch ready_latch{num_workers + 1};
    std::latch done_latch{num_workers};

    auto latch_worker = [&](int id) {
        std::println("  [线程{}] 初始化中...", id);
        std::this_thread::sleep_for((id * 100) * 1ms);  // 每个线程耗时不同

        // count_down: 计数器减 1, 当到 0 时所有等待的线程被释放
        ready_latch.count_down();  // 告诉主线程 "我准备好了"

        // 等待所有线程都准备好 (包括主线程)
        ready_latch.wait();
        std::println("  [线程{}] 所有线程已就绪, 开始并行工作!", id);

        std::this_thread::sleep_for(200ms);  // 模拟工作
        std::println("  [线程{}] 工作完成", id);

        done_latch.count_down();   // 告诉主线程 "我做完了"
    };

    std::vector<std::thread> latch_threads;
    for (int i = 0; i < num_workers; ++i) {
        latch_threads.emplace_back(latch_worker, i + 1);
    }

    // 主线程也准备好
    std::println("  [主线程] 准备完成, 等待所有线程就绪...");
    ready_latch.count_down();
    ready_latch.wait();       // 阻塞直到计数器归零
    std::println("  [主线程] 出发!");

    // 等待所有线程完成工作
    done_latch.wait();
    std::println("  [主线程] 所有线程已完成");

    for (auto& t : latch_threads) t.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: std::barrier (C++20) — 多阶段同步点
    // ═══════════════════════════════════════════════════════════════════════════
    // barrier 是一个可重用同步点:
    //   每个线程到达后调用 arrive_and_wait() 阻塞
    //   当所有线程都到达时, 所有线程同时释放, 进入下一阶段
    //
    // 使用场景:
    //   ① 并行算法中的迭代同步 (如并行排序的每一轮)
    //   ② 模拟/游戏中每一帧的同步
    //
    // 与 latch 的区别:
    //   barrier 可重用, 有"完成回调" (每个阶段开始时调用)
    lesson::print_subtitle("Part 4: std::barrier — 多阶段同步点");

    const int barrier_threads = 3;
    int phase = 0;

    // barrier 构造函数: (线程数, 完成回调)
    // 完成回调在每个阶段开始时调用 (barrier 释放前)
    std::barrier sync_point(barrier_threads, [&phase]() noexcept {
        ++phase;
        std::println("  ═══ 阶段 {} 开始 ═══", phase);
    });

    auto barrier_worker = [&](int id) {
        for (int stage = 1; stage <= 3; ++stage) {
            // 模拟每个阶段不同的工作量
            std::this_thread::sleep_for((id * 50 + stage * 30) * 1ms);
            std::println("  [线程{}] 阶段 {} 工作中...", id, stage);

            // arrive_and_wait: 到达同步点并等待其他线程
            //   ① 减少到达计数
            //   ② 如果还有线程未到达, 阻塞等待
            //   ③ 所有线程到达后, 执行完成回调, 然后重置计数, 释放所有线程
            sync_point.arrive_and_wait();
            std::println("  [线程{}] 阶段 {} 完成, 进入下一阶段", id, stage);
        }
        std::println("  [线程{}] 所有阶段完成", id);
    };

    std::vector<std::thread> barrier_workers;
    for (int i = 0; i < barrier_threads; ++i) {
        barrier_workers.emplace_back(barrier_worker, i + 1);
    }

    for (auto& t : barrier_workers) t.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: std::counting_semaphore (C++20) — 资源访问限制
    // ═══════════════════════════════════════════════════════════════════════════
    // counting_semaphore<N> 维护一个内部计数器:
    //   acquire(): 计数器减 1, 如果为 0 则阻塞
    //   release(): 计数器加 1, 唤醒一个等待线程
    //
    // 使用场景:
    //   ① 限制并发数据库连接数
    //   ② 限制同时打开的文件数
    //   ③ 线程池的工作队列长度控制
    //
    // binary_semaphore (N=1): 等价于互斥量, 但可用于线程间信号传递
    //   (mutex 要求 lock/unlock 在同一线程, semaphore 不要求)
    lesson::print_subtitle("Part 5: counting_semaphore — 限制并发数");

    // 最多允许 2 个"资源"同时使用 (如 2 个数据库连接)
    std::counting_semaphore<2> resource_pool{2};
    std::atomic<int> active_users{0};
    std::mutex print_mtx;

    auto resource_user = [&](int id) {
        {
            std::lock_guard lock(print_mtx);
            std::println("  [用户{}] 请求资源... (当前活跃: {})", id, active_users.load());
        }

        // acquire: 尝试获取一个资源许可, 如果计数器为 0 则阻塞
        resource_pool.acquire();
        ++active_users;

        {
            std::lock_guard lock(print_mtx);
            std::println("  [用户{}] 获得资源, 开始使用 (活跃: {})", id, active_users.load());
        }

        std::this_thread::sleep_for(300ms);  // 使用资源

        --active_users;
        {
            std::lock_guard lock(print_mtx);
            std::println("  [用户{}] 释放资源 (活跃: {})", id, active_users.load());
        }

        // release: 归还资源许可, 唤醒一个等待的线程
        resource_pool.release();
    };

    // 5 个用户竞争 2 个资源
    std::vector<std::thread> users;
    for (int i = 1; i <= 5; ++i) {
        users.emplace_back(resource_user, i);
        std::this_thread::sleep_for(50ms);  // 错开请求时间
    }

    for (auto& t : users) t.join();

    // ── binary_semaphore 作为线程间信号 ──
    lesson::print_separator("binary_semaphore 信号演示");
    {
        std::binary_semaphore signal{0};  // 初始计数为 0 (红色)

        std::jthread receiver([&] {
            std::println("  [接收者] 等待信号...");
            signal.acquire();  // 阻塞直到有人 release
            std::println("  [接收者] 收到信号! 开始处理");
        });

        std::this_thread::sleep_for(300ms);
        std::println("  [发送者] 发出信号");
        signal.release();  // 唤醒接收者 (和 mutex 不同, 可以跨线程 release)
        // jthread 自动 join
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. jthread 析构时 join 是阻塞的 → 如果线程死循环, 析构也会卡住");
    std::println("  2. stop_token 是协作式的 → 如果线程不检查 stop_requested(), 取消无效");
    std::println("  3. latch count_down 次数超过初始值 → 未定义行为");
    std::println("  4. barrier 线程数必须匹配 → 多了/少了都会导致永久等待");
    std::println("  5. semaphore release 次数超过 acquire → 计数器膨胀, 失去限制作用");
    std::println("  6. 不检查 semaphore::try_acquire 的返回值 → 直接使用未获得的资源");
    std::println("  7. binary_semaphore 和 mutex 混淆 → mutex 有所有权概念, semaphore 没有");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 用 jthread + stop_token 实现可取消的并行文件搜索 (搜索到 N 个结果后停止)");
    std::println("  2. 用 latch 实现"所有 5 个下载任务完成后再合并结果"的模式");
    std::println("  3. 用 barrier 实现一个 3 线程的并行归并排序 (每轮同步前进入下一层)");
    std::println("  4. 用 counting_semaphore 实现固定大小的线程池 (工作队列 + 资源限制)");
    std::println("  5. 对比 jthread + stop_token 和手动 thread + atomic<bool> 的代码量差异");
    std::println("  6. 用 counting_semaphore 实现哲学家就餐问题的解决方案");

    return 0;
}
