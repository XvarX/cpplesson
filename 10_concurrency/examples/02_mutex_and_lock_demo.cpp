// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 02: 互斥量与锁管理                                       ║
// ║  学习目标:                                                                  ║
// ║    1. 理解数据竞争 (data race) 和互斥量 (mutex) 的作用                      ║
// ║    2. 掌握 lock_guard / unique_lock / scoped_lock (C++17)                   ║
// ║    3. 了解 timed_mutex / recursive_mutex / shared_mutex (C++17)             ║
// ║    4. 学会诊断和避免死锁                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <chrono>
#include <functional>
#include <iomanip>

#include "shared/lesson_utils.hpp"
#include "concurrency/mutex_and_lock.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.2  互斥量与锁管理");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 数据竞争问题 — 为什么需要 mutex
    // ═══════════════════════════════════════════════════════════════════════════
    // 数据竞争: 多个线程同时访问同一块内存，且至少有一个是写操作，
    //          没有同步机制 → 结果不可预测 (undefined behavior)。
    //
    // std::mutex 是最基本的互斥量:
    //   lock()   — 获取锁；若已被别人持有，阻塞等待
    //   unlock() — 释放锁
    //   try_lock() — 尝试获取，失败立即返回 false (非阻塞)
    //
    // ⚠️ 永远不要手动 lock/unlock (容易漏 unlock，异常不安全)
    //    请用 RAII 锁包装器: lock_guard / unique_lock / scoped_lock
    lesson::print_subtitle("Part 1: 数据竞争与 mutex 基本用法");

    // ── 使用库中的 ThreadSafeCounter ──
    ThreadSafeCounter safe_counter;

    auto increment = [&](int n) {
        for (int i = 0; i < n; ++i) {
            safe_counter.increment();     // 线程安全的 ++
        }
    };

    std::thread t1(increment, 50'000);
    std::thread t2(increment, 50'000);
    t1.join();
    t2.join();

    std::println("  共享计数器最终值: {} (期望 100000)", safe_counter.get());

    // ── 手动 mutex 示例 (对比) ──
    lesson::print_separator("手动 mutex + lock_guard 示例");

    int raw_counter = 0;
    std::mutex raw_mutex;

    auto raw_increment = [&](int n) {
        for (int i = 0; i < n; ++i) {
            // 每次进入临界区前加锁 — 保证 counter++ 的原子性
            std::lock_guard<std::mutex> guard(raw_mutex);
            ++raw_counter;               // 临界区: 读-改-写
        }                                // guard 析构自动释放锁
    };

    std::thread rt1(raw_increment, 50'000);
    std::thread rt2(raw_increment, 50'000);
    rt1.join();
    rt2.join();

    std::println("  手动 mutex 计数器: {} (期望 100000)", raw_counter);

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 三种 RAII 锁包装器对比
    // ═══════════════════════════════════════════════════════════════════════════
    // lock_guard     (C++11): 最简单的 RAII 锁，构造时 lock，析构时 unlock
    //                        不可复制/移动，无法手动解锁
    // unique_lock    (C++11): 更灵活，支持延迟锁定、提前解锁、移动语义
    //                        可与 condition_variable 搭配使用
    // scoped_lock    (C++17): 同时锁定多个 mutex，防止死锁 (比 std::lock 更方便)
    lesson::print_subtitle("Part 2: lock_guard / unique_lock / scoped_lock");

    std::mutex mtx;
    int value = 0;

    // ── lock_guard: 简单粗暴，适合整个作用域内持有锁 ──
    {
        std::lock_guard<std::mutex> g(mtx);
        value += 1;
        std::println("  lock_guard: 持有锁中, value={}", value);
    }   // 离开作用域自动释放
    std::println("  lock_guard: 锁已释放");

    // ── unique_lock: 灵活控制 ──
    {
        std::unique_lock<std::mutex> ul(mtx, std::defer_lock); // 不立即加锁
        std::println("  unique_lock: 未加锁 (owns_lock={})",
                     ul.owns_lock() ? "true" : "false");

        ul.lock();    // 手动加锁
        value += 10;
        std::println("  unique_lock: 手动加锁后 value={}", value);
        ul.unlock();  // 提前解锁
        std::println("  unique_lock: 手动解锁后 (owns_lock={})",
                     ul.owns_lock() ? "true" : "false");

        ul.lock();    // 可重新加锁
        value += 5;
        std::println("  unique_lock: 重新加锁后 value={}", value);
    }   // 析构时若仍持有锁则解锁
    std::println("  unique_lock: 离开作用域, 最终 value={}", value);

    // ── scoped_lock (C++17): 多锁场景的王牌 ──
    {
        std::mutex mtx_a, mtx_b;
        // 同时锁定两个 mutex，使用死锁避免算法 (类似 std::lock)
        // 不会出现: 线程1锁A等B，线程2锁B等A 的死锁问题
        std::scoped_lock lock(mtx_a, mtx_b);
        std::println("  scoped_lock: 同时持有 mtx_a 和 mtx_b");
    }
    std::println("  scoped_lock: 两个锁均已释放");

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 特殊 mutex — timed_mutex / recursive_mutex / shared_mutex
    // ═══════════════════════════════════════════════════════════════════════════
    // timed_mutex:    支持 try_lock_for / try_lock_until，可设置超时
    // recursive_mutex: 同一线程可多次 lock (不阻塞自己)
    // shared_mutex:   读写锁 — 多个读者可并发，写者独占 (C++17)
    lesson::print_subtitle("Part 3: timed_mutex / recursive_mutex / shared_mutex");

    // ── timed_mutex: 带超时的锁 ──
    std::timed_mutex tmtx;
    if (tmtx.try_lock_for(100ms)) {     // 最多等待 100ms
        std::println("  timed_mutex: 成功获取锁");
        tmtx.unlock();
    } else {
        std::println("  timed_mutex: 超时未获取锁");
    }

    // ── shared_mutex: 读写锁 (C++17) ──
    // 场景: 配置表 — 读频繁，写罕见
    std::shared_mutex config_mutex;
    std::string config = "v1.0";

    // 多个读线程 (共享锁)
    auto reader = [&](int id) {
        std::shared_lock lock(config_mutex);  // 共享锁: 多个读者可同时持有
        std::println("  读者{} 读取配置: {}", id, config);
    };

    // 写线程 (独占锁)
    auto writer = [&]() {
        std::lock_guard lock(config_mutex);   // 独占锁: 写时禁止任何读取
        config = "v2.0";
        std::println("  写者更新配置: {}", config);
    };

    std::thread r1(reader, 1);
    std::thread r2(reader, 2);
    std::this_thread::sleep_for(10ms);
    std::thread w(writer);
    r1.join(); r2.join(); w.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 死锁的四个必要条件 & 避免策略
    // ═══════════════════════════════════════════════════════════════════════════
    // 死锁四条件 (破坏任一即可预防):
    //   ① 互斥:         资源不能被共享
    //   ② 持有并等待:   线程持有一个锁的同时等待另一个锁
    //   ③ 不可剥夺:     不能强行抢走线程持有的锁
    //   ④ 循环等待:     A→B 等锁形成环
    //
    // 避免策略:
    //   1. 统一锁的获取顺序 (破坏 ④ 循环等待)
    //   2. 使用 scoped_lock / std::lock 一次性获取所有锁
    //   3. 使用 try_lock 带超时，失败则释放所有已持有的锁并重试
    lesson::print_subtitle("Part 4: 死锁避免");

    {
        std::mutex a, b;  // 两个资源

        // 场景: 如果不按固定顺序，可能出现死锁
        //   T1: lock(a) → lock(b)
        //   T2: lock(b) → lock(a)   ← 死锁!
        //
        // 解决: scoped_lock 内部使用 std::lock 算法
        //       同时尝试获取所有锁，避免死锁

        std::thread t_safe1([&] {
            std::scoped_lock lock(a, b);  // 安全地同时获取两个锁
            std::println("  线程 1: 持有 a 和 b");
        });

        std::thread t_safe2([&] {
            std::scoped_lock lock(b, a);  // 顺序不同也没关系!
            std::println("  线程 2: 持有 b 和 a");
        });

        t_safe1.join();
        t_safe2.join();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. 忘记释放锁 → 用 RAII 锁包装器，永远不要裸调 lock/unlock");
    std::println("  2. 锁粒度过大 → 不必要的耗时操作 (I/O、计算) 放在临界区外面");
    std::println("  3. 锁粒度过小 → 一次操作分多段，中间状态暴露给其他线程");
    std::println("  4. 持有锁时调用外部代码 → 可能导致意外加锁 (死锁) 或性能骤降");
    std::println("  5. recursive_mutex 藏 bug → 设计缺陷的信号，考虑重构为无递归加锁");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 写一个线程安全的计数器类，支持 add() 和 get()");
    std::println("  2. 用 shared_mutex 实现一个线程安全的缓存 (读多写少场景)");
    std::println("  3. 故意制造一个死锁，然后用 scoped_lock 修复它");
    std::println("  4. 用 timed_mutex 实现“获取锁最多等待 500ms，否则放弃”的逻辑");

    return 0;
}
