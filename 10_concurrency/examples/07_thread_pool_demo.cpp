// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 07: 线程池 (Thread Pool)                                    ║
// ║  学习目标:                                                                  ║
// ║    1. 理解线程池的设计原理: 任务队列 + 工作线程 + 优雅关闭                    ║
// ║    2. 掌握 std::packaged_task — 将可调用对象包装成 future 的生产者           ║
// ║    3. 理解 std::jthread + stop_token — 协作式线程管理的实现                  ║
// ║    4. 掌握 std::counting_semaphore — 有界队列的限流 (背压 back-pressure)     ║
// ║    5. 分析线程池的常见陷阱: future 生命周期、死锁、异常传播                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <future>
#include <cmath>

#include "shared/lesson_utils.hpp"
#include "concurrency/thread_pool.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.7  线程池 — 任务队列 + 工作线程 + 优雅关闭");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 线程池基础 — 提交任务并获取结果
    // ═══════════════════════════════════════════════════════════════════════════
    // ThreadPool 的核心接口是 submit():
    //   传入任意可调用对象 + 参数 → 返回 std::future<ReturnType>
    //   调用 future::get() 阻塞等待结果 (或获取异常)
    //
    // 内部机制:
    //   ① submit 用 packaged_task 包装任务 → 获取 future
    //   ② 任务被包装成 std::function<void()> 放入队列
    //   ③ 空闲的工作线程取出任务并执行
    //   ④ 执行完毕后, future 就绪, get() 可以获取结果
    lesson::print_subtitle("Part 1: 线程池基础 — submit + future");

    {
        // 创建一个 4 线程的线程池 (无界队列)
        concurrency::ThreadPool pool(4);

        // 提交一个简单的计算任务 — lambda 带参数和返回值
        auto f1 = pool.submit([](int a, int b) {
            std::println("    [任务1] 计算 {} + {}", a, b);
            std::this_thread::sleep_for(50ms);
            return a + b;
        }, 10, 20);

        // 可以同时提交多个任务, 它们会被分配给不同的工作线程并行执行
        auto f2 = pool.submit([](int n) {
            std::println("    [任务2] 计算 {} 的阶乘...", n);
            std::this_thread::sleep_for(80ms);
            long long result = 1;
            for (int i = 2; i <= n; ++i) result *= i;
            return result;
        }, 10);

        auto f3 = pool.submit([] {
            std::println("    [任务3] 执行中...");
            std::this_thread::sleep_for(30ms);
        });

        // future::get() 阻塞当前线程, 直到对应任务执行完毕并返回结果
        std::println("  结果1: {}", f1.get());  // → 30
        std::println("  结果2: 10! = {}", f2.get());  // → 3628800
        f3.get();  // void 类型的 future, 仅用于等待任务完成

        // pool 离开作用域 → 析构 → request_stop + notify_all + join
    }
    std::println("  [主线程] 线程池已安全销毁");

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: packaged_task 原理 — 手写一遍以理解内部机制
    // ═══════════════════════════════════════════════════════════════════════════
    // 线程池的 submit() 内部使用 packaged_task 桥接"任务调用"和"future 结果":
    //
    //   packaged_task<R(Args...)> 是可调用对象 + promise 的组合:
    //     ① task(args...) → 执行函数, 结果通过关联的 promise 传递
    //     ② task.get_future() → 获取绑定的 future, 用于异步等待结果
    //
    // 关键点: packaged_task 只可移动 (不可拷贝)
    //   线程池中需要把任务放入 std::queue<std::function<void()>>,
    //   而 std::function 要求可拷贝 → 必须用 shared_ptr 间接持有
    lesson::print_subtitle("Part 2: packaged_task 原理 — 手动演示");

    {
        // 步骤 1: 创建一个 packaged_task, 绑定计算函数
        std::packaged_task<int(int, int)> task([](int x, int y) {
            std::println("    [packaged_task] 计算 {} * {}", x, y);
            return x * y;
        });

        // 步骤 2: 从 packaged_task 获取 future (必须在 task 被移动前)
        std::future<int> result = task.get_future();

        // 步骤 3: 用 shared_ptr 持有 packaged_task (因为无法拷贝)
        auto task_ptr = std::make_shared<std::packaged_task<int(int, int)>>(
            std::move(task));

        // 步骤 4: 包装成 std::function<void()> (线程池的任务队列统一类型)
        std::function<void()> wrapped = [task_ptr]() {
            (*task_ptr)(6, 7);  // 调用 packaged_task → 执行计算 → 写结果到 promise
        };

        // 步骤 5: 在其他线程执行 wrapped
        std::jthread executor([&wrapped] {
            wrapped();
        });
        executor.join();

        // 步骤 6: 通过 future 获取结果
        std::println("    结果: {} (来自 future.get())", result.get());
    }

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: stop_token 优雅关闭 — 线程池析构流程演示
    // ═══════════════════════════════════════════════════════════════════════════
    // 线程池析构时执行优雅关闭:
    //   ① request_stop() — 通知所有线程"该下班了"
    //   ② notify_all()   — 唤醒所有在条件变量上等待的线程
    //   ③ 每个工作线程检查 stop_requested(), 处理完当前任务后退出
    //   ④ jthread 析构自动 join — 确保所有线程都已退出
    //
    // ⚠️ 关键: 这是协作式关闭 — 线程必须主动检查 stop_token,
    //    不会被强制杀死 (没有 pthread_cancel / TerminateThread)
    lesson::print_subtitle("Part 3: stop_token 优雅关闭 — 析构流程");

    {
        std::println("  [主线程] 创建线程池, 提交一些长时间任务...");

        {
            concurrency::ThreadPool pool(3);

            // 提交几个任务, 有些会耗时较长
            for (int i = 1; i <= 5; ++i) {
                pool.submit([i] {
                    std::println("    [任务{}] 开始执行...", i);
                    std::this_thread::sleep_for(i * 30ms);
                    std::println("    [任务{}] 完成", i);
                });
            }

            // 让部分任务先开始执行
            std::this_thread::sleep_for(50ms);

            std::println("  [主线程] 线程池即将离开作用域 → 开始优雅关闭");
            // pool 析构在这里执行:
            //   request_stop → notify_all → 工作线程完成当前任务后退出 → join
        }

        std::println("  [主线程] 线程池已完全关闭, 所有任务要么完成要么被丢弃");
    }

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: counting_semaphore 限流 — 有界队列防止任务堆积
    // ═══════════════════════════════════════════════════════════════════════════
    // 当 max_queue_size > 0 时, 线程池使用 counting_semaphore 实现背压:
    //   - 初始计数 = max_queue_size (队列中空闲槽位数)
    //   - submit() 调用 acquire(): 消耗一个槽位, 槽位不足则阻塞提交者
    //   - 工作线程取出任务后 release(): 归还一个槽位, 唤醒等待中的提交者
    //
    // 为什么需要限流?
    //   "生产者比消费者快" 时, 无界队列会无限增长 (内存耗尽)
    //   有界队列 + 信号量 让生产者自动减速 (背压 back-pressure)
    lesson::print_subtitle("Part 4: counting_semaphore 限流 — 有界队列");

    {
        // 创建线程池: 1 个慢速工作线程 + 最多 3 个待处理任务的队列
        // 生产者 (submit) 比消费者 (worker) 快 → 队列满时 submit 阻塞
        concurrency::ThreadPool pool(1, 3);
        std::println("  [配置] 1 个工作线程, 队列容量 3");

        std::println("  [主线程] 连续提交 6 个任务 (每个耗时 200ms)...");
        std::println("  [主线程] 预期: 第 4 个任务开始, submit 会阻塞等待空位");

        // 用一个独立线程来提交任务 (避免主线程被阻塞)
        std::jthread producer([&pool] {
            for (int i = 1; i <= 6; ++i) {
                std::println("  [生产者] 提交任务 {} ... (队列中: {} 个)",
                             i, pool.pending_count());
                auto start = std::chrono::steady_clock::now();

                pool.submit([i] {
                    std::println("    [任务{}] 执行中...", i);
                    std::this_thread::sleep_for(200ms);
                    std::println("    [任务{}] 完成", i);
                });

                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start);
                std::println("  [生产者] 任务 {} 提交成功 (submit 阻塞了 {}ms)",
                             i, elapsed.count());
            }
            std::println("  [生产者] 所有任务提交完毕");
        });

        producer.join();
        std::println("  [主线程] 线程池即将析构 (等待剩余任务完成)");
    }

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 实战 — 并行计算素数个数 (分治法 + 线程池)
    // ═══════════════════════════════════════════════════════════════════════════
    // 将一个大范围拆分成多个子范围, 每个子范围提交给线程池并行计算
    lesson::print_subtitle("Part 5: 实战 — 并行计算素数个数 (分治法)");

    {
        constexpr int N = 50000;        // 搜索范围上限
        constexpr int num_chunks = 8;   // 拆分成 8 个子任务

        // 判断一个数是否为素数的辅助函数
        auto is_prime = [](int n) -> bool {
            if (n < 2) return false;
            if (n == 2) return true;
            if (n % 2 == 0) return false;
            int limit = static_cast<int>(std::sqrt(n));
            for (int i = 3; i <= limit; i += 2) {
                if (n % i == 0) return false;
            }
            return true;
        };

        // 在 [start, end] 范围内统计素数个数的子任务
        auto count_primes_in_range = [&is_prime](int start, int end) -> int {
            int count = 0;
            for (int i = start; i < end; ++i) {
                if (is_prime(i)) ++count;
            }
            return count;
        };

        concurrency::ThreadPool pool(std::thread::hardware_concurrency());

        // 提交 8 个子任务, 每个负责一个子范围
        std::vector<std::future<int>> futures;
        int chunk_size = N / num_chunks;
        for (int i = 0; i < num_chunks; ++i) {
            int start = i * chunk_size;
            int end = (i == num_chunks - 1) ? N : start + chunk_size;
            futures.push_back(pool.submit(count_primes_in_range, start, end));
        }

        // 汇总所有子任务的结果
        int total_primes = 0;
        for (auto& fut : futures) {
            total_primes += fut.get();  // 阻塞等待每个子任务完成
        }

        std::println("  [结果] 1 ~ {} 范围内的素数个数: {}", N, total_primes);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. future::get() 只能调用一次 → 多次调用会抛 future_error");
    std::println("  2. 忽略 submit 返回的 future → 任务可能悄悄失败, 异常被吞掉");
    std::println("  3. 任务死循环不检查 stop_token → 析构时永久阻塞 (无法强制杀死)");
    std::println("  4. packaged_task 不可拷贝 → 必须用 shared_ptr 间接持有");
    std::println("  5. 有界队列 + 析构前的 submit → 如果析构后仍在 acquire, 可能死锁");
    std::println("  6. 捕获 this 指针的 lambda → 确保线程池生命周期长于提交的任务");
    std::println("  7. 锁内执行耗时任务 → 阻塞所有其他线程, 破坏并行性 (应在锁外执行)");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 实现 drain_and_stop(): 关闭前先排空队列中所有剩余任务");
    std::println("  2. 为 submit 添加超时参数: 队列满超过指定时间则返回 std::nullopt");
    std::println("  3. 改用 priority_queue 实现带优先级的线程池");
    std::println("  4. 分析未来线程池析构时, 哪些情况下 submit 会永久阻塞 acquire()");
    std::println("  5. 对比 std::async 与本线程池在以下场景的表现差异:");
    std::println("       - 并行矩阵乘法 (1000x1000 矩阵, 分块计算)");
    std::println("       - 高并发短任务 (10000 个 1ms 任务 vs 4 个 2500ms 任务)");
    std::println("  6. 为线程池添加统计功能: 已完成任务数、平均等待时间、最大队列长度");

    return 0;
}
