// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 03: 条件变量 (std::condition_variable)                      ║
// ║  学习目标:                                                                  ║
// ║    1. 理解条件变量的作用 — 线程间"事件通知"机制                              ║
// ║    2. 掌握 wait / notify_one / notify_all 的用法                             ║
// ║    3. 理解虚假唤醒 (spurious wakeup) 和谓词 wait 的必要性                    ║
// ║    4. 实现生产者-消费者模型                                                   ║
// ║    5. 知道什么时候用 notify_one，什么时候用 notify_all                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <string>

#include "shared/lesson_utils.hpp"
#include "concurrency/blocking_queue.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.3  条件变量 (std::condition_variable)");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 条件变量基础 — 为什么需要它？
    // ═══════════════════════════════════════════════════════════════════════════
    // 场景: 线程 A 等待某个条件成立，线程 B 负责满足该条件。
    //
    // 低效方案 (忙等待 / busy-wait):
    //   while (!ready) { sleep(1ms); }   ← CPU 空转，浪费资源
    //
    // 条件变量方案:
    //   线程 A: cv.wait(lock, []{ return ready; });  ← 阻塞，不占 CPU
    //   线程 B: ready = true; cv.notify_one();       ← 主动唤醒 A
    //
    // 条件变量 = 互斥量 + 等待队列:
    //   wait() 内部: 释放锁 → 进入等待队列 → 被唤醒 → 重新获取锁 → 检查条件
    lesson::print_subtitle("Part 1: 条件变量基础");

    std::mutex mtx;
    std::condition_variable cv;
    bool ready_flag = false;
    std::string data;

    std::thread worker([&] {
        std::println("  [工作线程] 开始处理...");
        std::this_thread::sleep_for(200ms);   // 模拟耗时准备

        {
            std::lock_guard lock(mtx);
            data = "处理完毕的结果";
            ready_flag = true;
            std::println("  [工作线程] 数据准备完成");
        }
        // 解锁后通知 — 最佳实践:
        //   如果持有锁时 notify，被唤醒的线程会立即尝试获取锁但发现锁还被持有，
        //   又会被阻塞，造成不必要的上下文切换
        cv.notify_one();
    });

    // 主线程等待数据就绪
    {
        std::unique_lock lock(mtx);
        std::println("  [主线程] 等待数据...");

        // ═════════════════════════════════════════════════════════════════════
        // 使用带谓词的 wait — 推荐做法
        // ═════════════════════════════════════════════════════════════════════
        cv.wait(lock, [&ready_flag] { return ready_flag; });
        // 等价于手动写法:
        //   while (!ready_flag) { cv.wait(lock); }
        //
        // 为什么需要 while 循环？
        //   虚假唤醒 (spurious wakeup): 操作系统可能在没有 notify 的情况下
        //   唤醒等待的线程。这并非 C++ 标准库的 bug，而是底层 OS 调度机制的特性。
        //   因此必须在醒来后重新检查条件。

        std::println("  [主线程] 收到数据: {}", data);
    }

    worker.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 生产者-消费者模型 (含限流)
    // ═══════════════════════════════════════════════════════════════════════════
    // 经典多线程问题:
    //   生产者 — 生成数据放入缓冲区
    //   消费者 — 从缓冲区取出数据处理
    //   缓冲区 — 有容量限制，满时生产者等待，空时消费者等待
    //
    // 条件变量天然适合解决这种"等待某种状态"的问题
    lesson::print_subtitle("Part 2: 生产者-消费者模型");

    // 使用库中的 BlockingQueue<int>
    BlockingQueue<int> bq(3);  // 容量为 3 的阻塞队列

    std::thread producer([&] {
        for (int i = 1; i <= 6; ++i) {
            std::println("→ 生产者准备放入: {}", i);
            bq.push(i);
            std::this_thread::sleep_for(50ms);  // 模拟生产耗时
        }
        std::println("→ 生产者完成, 总计生产 6 个");
    });

    std::thread consumer([&] {
        for (int i = 1; i <= 6; ++i) {
            int val = bq.pop();
            std::println("← 消费者取出: {}", val);
            std::this_thread::sleep_for(120ms); // 消费速度比生产慢
        }
        std::println("← 消费者完成, 总计消费 6 个");
    });

    producer.join();
    consumer.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: notify_one vs notify_all
    // ═══════════════════════════════════════════════════════════════════════════
    // notify_one:  唤醒等待队列中的任意一个线程
    //   - 适用: 每个被唤醒的线程处理不同的工作 (如消费不同元素)
    //   - 效率高: 只有一个线程被唤醒后竞争锁
    //
    // notify_all:  唤醒等待队列中的所有线程
    //   - 适用: 状态变化影响所有等待线程 (如程序退出标志、配置更新)
    //   - 开销大: 所有线程被唤醒，但只有一个能获得锁 (惊群效应)
    lesson::print_subtitle("Part 3: notify_one vs notify_all");

    std::mutex demo_mtx;
    std::condition_variable demo_cv;
    int signal_count = 0;

    auto waiting_task = [&](int id) {
        std::unique_lock lock(demo_mtx);
        std::println("  [线程{}] 开始等待...", id);
        demo_cv.wait(lock, [&signal_count] { return signal_count > 0; });
        std::println("  [线程{}] 被唤醒! signal_count={}", id, signal_count);
    };

    // 创建 5 个等待线程
    std::vector<std::thread> waiters;
    for (int i = 1; i <= 5; ++i) {
        waiters.emplace_back(waiting_task, i);
        std::this_thread::sleep_for(20ms);  // 确保它们按顺序进入等待
    }

    std::this_thread::sleep_for(200ms);

    // notify_one: 只唤醒一个线程
    {
        std::lock_guard lock(demo_mtx);
        signal_count = 1;
    }
    demo_cv.notify_one();
    std::println("  [主线程] notify_one 已调用 (只唤醒一个)");

    std::this_thread::sleep_for(200ms);

    // notify_all: 唤醒所有剩余线程
    {
        std::lock_guard lock(demo_mtx);
        signal_count = 999;
    }
    demo_cv.notify_all();
    std::println("  [主线程] notify_all 已调用 (唤醒所有)");

    for (auto& t : waiters) t.join();

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. 忘记用 predicate wait → 虚假唤醒后可能操作无效数据");
    std::println("  2. 使用 notify_one 但需要通知所有线程 → 部分线程可能永远等不到通知");
    std::println("  3. 持有锁时调用 notify → 被唤醒线程立即阻塞抢锁, 造成无谓的上下文切换");
    std::println("  4. wait 必须用 unique_lock, 不能用 lock_guard → unlock/lock 在 wait 内部");
    std::println("  5. 条件变量 wait 醒来后锁是持有的 → 务必在修改共享数据后及时解锁");
    std::println("  6. notify 在 wait 之前调用 → 通知丢失, 等待线程一直阻塞 (lost wakeup)");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 实现一个“多生产者-单消费者”的 BlockingQueue");
    std::println("  2. 用条件变量实现一个简单的 CountDownLatch (倒计数门闩)");
    std::println("  3. 写一段代码演示“丢失唤醒”问题 (notify 发生在 wait 之前)");
    std::println("  4. 给 BlockingQueue 添加 try_push/try_pop 方法 (超时版本)");
    std::println("  5. 研究 pipe-or-notify: 什么场景下用管道替代条件变量更好？");

    return 0;
}
