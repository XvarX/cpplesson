// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 05: std::atomic 与内存序                                    ║
// ║  学习目标:                                                                  ║
// ║    1. 理解 std::atomic 的作用 — 无锁线程安全操作                              ║
// ║    2. 掌握原子操作: load / store / exchange / compare_exchange               ║
// ║    3. 理解六种内存序 (memory ordering) 及其性能差异                           ║
// ║    4. 了解 atomic_ref (C++20) 临时赋予普通变量原子性                         ║
// ║    5. 用 atomic_flag 实现自旋锁                                              ║
// ║    6. 理解 fence (内存屏障) 的概念                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <string>

#include "shared/lesson_utils.hpp"
#include "concurrency/atomic_utils.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.5  std::atomic 与内存序");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 为什么需要 atomic
    // ═══════════════════════════════════════════════════════════════════════════
    // 问题: 即使是最简单的 int++, 也不是原子的:
    //   ① 从内存加载值到寄存器
    //   ② 寄存器中加 1
    //   ③ 写回内存
    // 如果两个线程同时执行，可能丢失一次加法 (第 02 课中的 mutex 解决方案)。
    //
    // std::atomic 提供无锁 (lock-free) 的替代方案:
    //   ① 比 mutex 更轻量 (通常由 CPU 原子指令实现)
    //   ② 适合简单的数值操作 (计数器、标志位)
    //   ③ 复杂数据结构仍需 mutex
    lesson::print_subtitle("Part 1: 原子操作基础");

    // ── 基本类型: atomic<int>, atomic<bool>, atomic<指针> ──
    std::atomic<int> atomic_counter{0};

    auto incrementer = [&atomic_counter](int n) {
        for (int i = 0; i < n; ++i) {
            // fetch_add: 原子地加 1, 返回旧值 (也可用 ++atomic_counter)
            atomic_counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread t1(incrementer, 50'000);
    std::thread t2(incrementer, 50'000);
    t1.join();
    t2.join();

    std::println("  atomic 计数器结果: {} (期望 100000)", atomic_counter.load());

    // ── load / store / exchange ──
    std::atomic<int> flag{0};
    flag.store(42);                        // 原子写入
    int val = flag.load();                 // 原子读取
    std::println("  flag = {}", val);

    int old = flag.exchange(100);          // 原子交换: 写入 100, 返回旧值
    std::println("  exchange: old={}, new={}", old, flag.load());

    // is_lock_free() — 检查是否真正无锁 (总是 true 对于基础类型)
    std::println("  atomic<int> 无锁: {}", atomic_counter.is_lock_free() ? "是" : "否");

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: compare_exchange (CAS) — 原子比较交换
    // ═══════════════════════════════════════════════════════════════════════════
    // CAS (Compare-And-Swap) 是无锁编程的基石:
    //   "如果值等于我预期的，就把它改成新值；否则告诉我实际值是什么"
    //
    // compare_exchange_strong(expected, desired):
    //   原子地比较 *this 和 expected，相等则写入 desired
    //   不相等则把 *this 的值写入 expected
    //
    // compare_exchange_weak: 同上，但允许伪失败 (需循环使用)
    lesson::print_subtitle("Part 2: compare_exchange (CAS 操作)");

    std::atomic<int> cas_val{10};
    int expected = 10;
    int desired = 20;

    // strong: 期望值匹配，写入成功
    bool success = cas_val.compare_exchange_strong(expected, desired);
    std::println("  CAS strong: success={}, val={}, expected={}",
                 success ? "true" : "false", cas_val.load(), expected);

    // 期望值不匹配，失败且 expected 被更新
    expected = 5;
    desired = 30;
    success = cas_val.compare_exchange_strong(expected, desired);
    std::println("  CAS strong (不匹配): success={}, val={}, expected(更新后)={}",
                 success ? "true" : "false", cas_val.load(), expected);

    // ── 用 CAS 实现原子的"如果大于就更新" (fetch_max 的简化版) ──
    std::atomic<int> max_val{0};
    std::println("  max_val 初始值: {}", max_val.load());

    // 尝试将 max_val 更新为 42 (如果 42 更大)
    int current = max_val.load();
    while (current < 42 && !max_val.compare_exchange_weak(current, 42)) {
        // current 已被更新为实际值, 重新判断
    }
    std::println("  尝试更新为 42 后: max_val={}", max_val.load());

    // 尝试更新为 30 (应该失败, 因为 30 < 42)
    current = max_val.load();
    while (current < 30 && !max_val.compare_exchange_weak(current, 30)) {}
    std::println("  尝试更新为 30 后: max_val={} (不应变化)", max_val.load());

    // ── 使用库中的 LockFreeCounter 演示 CAS ──
    lesson::print_separator("LockFreeCounter 演示");
    LockFreeCounter lf_counter;

    lf_counter.increment_simple();
    lf_counter.increment_simple();
    std::println("  increment_simple ×2: {}", lf_counter.get());

    lf_counter.increment_cas();
    lf_counter.increment_cas();
    lf_counter.increment_cas();
    std::println("  increment_cas ×3: {}", lf_counter.get());

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 内存序 (Memory Ordering)
    // ═══════════════════════════════════════════════════════════════════════════
    // CPU 和编译器会对指令重排 (reorder) 以提高性能。
    // 在多线程环境中，重排可能导致另一个线程看到"不可能"的状态。
    //
    // C++ 提供 6 种内存序 (由弱到强):
    //   relaxed            — 无同步保证，只保证原子性 (计数器适用)
    //   consume (不推荐)   — 只同步依赖关系 (几乎不用)
    //   acquire            — 此操作之后的读写不会被重排到此操作之前
    //   release            — 此操作之前的读写不会被重排到此操作之后
    //   acq_rel            — acquire + release 合体
    //   seq_cst (默认)     — 全局一致的总顺序 (最强但最慢)
    //
    // 经验法则: 默认用 seq_cst (安全), 确认没问题后用 acquire/release, 计数器用 relaxed
    lesson::print_subtitle("Part 3: 内存序对比");

    // ── 发布订阅模式: 展示 acquire/release 的必要性 ──
    std::atomic<bool> data_ready{false};
    std::string message;  // 非原子数据, 但通过 atomic 标志同步

    std::thread publisher([&] {
        message = "重要消息";                       // ① 准备数据
        data_ready.store(true, std::memory_order_release);  // ② 发布 — release 保证 ① 对后续 acquire 可见
    });

    std::thread subscriber([&] {
        while (!data_ready.load(std::memory_order_acquire));  // ③ 等待 — acquire 保证看到 ①
        std::println("  订阅者收到: {}", message);             // ④ 安全读取 message
    });

    publisher.join();
    subscriber.join();

    // ── 性能对比: relaxed vs seq_cst ──
    lesson::print_separator("relaxed vs seq_cst 性能对比");

    std::atomic<long long> r_counter{0};
    std::atomic<long long> s_counter{0};

    auto relaxed_work = [&] {
        for (int i = 0; i < 1'000'000; ++i) {
            r_counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    auto seq_cst_work = [&] {
        for (int i = 0; i < 1'000'000; ++i) {
            s_counter.fetch_add(1, std::memory_order_seq_cst);
        }
    };

    std::println("  relaxed 版本:");
    std::thread rt1(relaxed_work), rt2(relaxed_work);
    rt1.join(); rt2.join();
    std::println("  relaxed 结果: {}", r_counter.load());

    std::println("  seq_cst 版本:");
    std::thread st1(seq_cst_work), st2(seq_cst_work);
    st1.join(); st2.join();
    std::println("  seq_cst 结果: {}", s_counter.load());
    std::println("  (通常 relaxed 在 x86 上性能接近 seq_cst, 但在 ARM 上差距明显)");

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 自旋锁 + atomic_ref (C++20)
    // ═══════════════════════════════════════════════════════════════════════════
    // 自旋锁: 适合临界区极短 (几行代码) 的场景，避免线程切换开销。
    // atomic_ref (C++20): 临时对普通变量进行原子操作，不要求变量本身是 atomic。
    lesson::print_subtitle("Part 4: 自旋锁 + atomic_ref");

    // ── 使用库中的 SpinLock ──
    SpinLock spinlock;
    int sp_counter = 0;  // 普通变量, 但受 spinlock 保护

    auto spin_worker = [&](int count) {
        for (int i = 0; i < count; ++i) {
            spinlock.lock();
            ++sp_counter;          // 临界区 — 只有很短的代码
            spinlock.unlock();
        }
    };

    std::thread sw1(spin_worker, 50'000);
    std::thread sw2(spin_worker, 50'000);
    sw1.join(); sw2.join();

    std::println("  自旋锁保护的计数器: {} (期望 100000)", sp_counter);

    // ── atomic_ref (C++20): 临时给普通变量加上原子操作 ──
    int plain_value = 0;
    {
        std::atomic_ref<int> ref(plain_value);  // 创建 atomic_ref 指向 plain_value
        ref.fetch_add(10);                      // 此时 ref 存在, 其他线程必须也通过 atomic_ref 访问
        // ⚠️ atomic_ref 期间不能有其他非原子的读写!
    }
    std::println("  atomic_ref 后 plain_value: {}", plain_value);

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: atomic_flag 与 fence (内存屏障)
    // ═══════════════════════════════════════════════════════════════════════════
    // atomic_flag: 最简原子类型，只有 test_and_set 和 clear，始终 lock-free
    //
    // std::atomic_thread_fence: 插入内存屏障 (memory barrier)
    //   阻止屏障两侧的指令被重排到另一侧
    //   通常不需要直接使用 — 优先用 atomic 操作的 acquire/release
    lesson::print_subtitle("Part 5: atomic_flag 与 fence");

    // ── atomic_flag 作为一次性开关 ──
    std::atomic_flag init_done = ATOMIC_FLAG_INIT;
    std::vector<int> data_cache;

    auto initialize = [&] {
        // test_and_set: 原子的"检查并设置", 只有第一个调用者看到 false
        if (!init_done.test_and_set()) {
            std::println("  [初始化线程] 执行一次性初始化...");
            for (int i = 0; i < 5; ++i) {
                data_cache.push_back(i * 10);
            }
            std::println("  [初始化线程] 初始化完成");
        } else {
            std::println("  [线程] 已初始化, 跳过");
        }
    };

    std::thread init_t1(initialize);
    std::thread init_t2(initialize);
    init_t1.join();
    init_t2.join();

    // ── fence 演示 ──
    // 等价于 release store + acquire load 的组合
    // 通常不需要直接使用 fence, 这里只做演示
    std::atomic<int> fence_val{0};
    std::atomic<bool> fence_ready{false};

    std::thread fence_writer([&] {
        fence_val.store(42, std::memory_order_relaxed);
        // release fence: 保证 fence 之前的写入对后续 acquire fence 可见
        std::atomic_thread_fence(std::memory_order_release);
        fence_ready.store(true, std::memory_order_relaxed);
    });

    std::thread fence_reader([&] {
        while (!fence_ready.load(std::memory_order_relaxed));
        // acquire fence: 保证此处之后能看到 release fence 之前的写入
        std::atomic_thread_fence(std::memory_order_acquire);
        std::println("  fence 同步结果: fence_val={}", fence_val.load(std::memory_order_relaxed));
    });

    fence_writer.join();
    fence_reader.join();

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. atomic 不等于 volatile → volatile 不保证原子性或内存序");
    std::println("  2. 混淆 weak 和 strong CAS → weak 必须配合循环, strong 适合单次判断");
    std::println("  3. memory_order_relaxed 用于同步 → 数据可能对不同线程可见时机不一致");
    std::println("  4. 自旋锁用于耗时临界区 → CPU 空转浪费, 应改用 mutex");
    std::println("  5. atomic_ref 生命周期内混合非原子访问 → 数据竞争 (undefined behavior)");
    std::println("  6. 默认 seq_cst 拖慢性能 → 确认同步关系后用 acquire/release");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 用 atomic 实现一个无锁的 LIFO 栈 (Treiber stack)");
    std::println("  2. 用 CAS 实现 fetch_max (原子地保持最大值)");
    std::println("  3. 在 ARM (或 ARM 模拟器) 上测试 relaxed vs seq_cst 的性能差异");
    std::println("  4. 用 atomic_flag 实现一个简单的读写自旋锁 (允许多读单写)");
    std::println("  5. 研究 double-checked locking 模式并用 atomic + mutex 正确实现");

    return 0;
}
