#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/atomic_utils.hpp — std::atomic 与无锁编程                       ║
// ║                                                                              ║
// ║  提供:                                                                       ║
// ║    - SpinLock:         用 atomic_flag 实现的自旋锁                           ║
// ║    - LockFreeCounter:  用 CAS 实现的无锁计数器                                ║
// ║                                                                              ║
// ║  内存序速查 (由弱到强):                                                       ║
// ║    relaxed   — 无同步保证, 只保证原子性 (计数器适用)                          ║
// ║    consume   — 不推荐使用 (几乎不用)                                          ║
// ║    acquire   — 此操作之后的读写不会被重排到此操作之前                          ║
// ║    release   — 此操作之前的读写不会被重排到此操作之后                          ║
// ║    acq_rel   — acquire + release 合体                                        ║
// ║    seq_cst   — 全局一致的总顺序, 最安全但最慢 (默认)                           ║
// ║                                                                              ║
// ║  经验法则: 默认用 seq_cst (安全), 确认没问题后用 acquire/release,              ║
// ║           计数器用 relaxed                                                    ║
// ║                                                                              ║
// ║  CAS (Compare-And-Swap) 是无锁编程的基石:                                     ║
// ║    compare_exchange_strong — 保证只有值真正不同时才返回 false                  ║
// ║    compare_exchange_weak   — 允许伪失败, 性能更高, 需配合 while 循环           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <atomic>
#include <print>

// ── 自旋锁: 用 atomic_flag 实现轻量级互斥 ─────────────────────────────────────
/// SpinLock 用 std::atomic_flag 实现自旋锁。
///
/// atomic_flag 是最简单的原子类型 — 始终无锁 (lock-free)。
/// 只有 test_and_set 和 clear 两个操作, 天然适合做自旋锁。
///
/// 适用场景: 临界区极短 (几行代码) — 避免线程切换开销
/// ⚠️ 不适合: I/O 操作、复杂计算等长时间临界区 (应改用 mutex)
class SpinLock {
public:
    SpinLock() = default;

    /// 获取自旋锁。
    /// test_and_set 原子地将 flag_ 设为 true 并返回旧值。
    /// 如果旧值是 true (已被锁定), 自旋等待。
    /// memory_order_acquire: 后续读写在锁获取后可见。
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // 自旋 — 适合临界区极短的场景 (几行代码)
            // ⚠️ 长时间自旋浪费 CPU, 不适合 I/O 或复杂计算
        }
    }

    /// 释放自旋锁。
    /// memory_order_release: 所有写入在锁释放前对后续获取者可见。
    void unlock() {
        flag_.clear(std::memory_order_release);
    }

    /// 尝试获取锁, 失败立即返回 (非阻塞)。
    /// @return true 表示成功获取锁, false 表示锁已被持有
    bool try_lock() {
        return !flag_.test_and_set(std::memory_order_acquire);
    }

    // 不可复制/移动
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;  // 初始化为 false (未锁定)
};

// ── 无锁计数器 (用 compare_exchange_weak) ──────────────────────────────────────
/// LockFreeCounter 使用 CAS 操作实现无锁计数器。
///
/// 演示了两种原子递增方式:
///   - increment_simple(): 使用 fetch_add (最简单)
///   - increment_cas():    使用 compare_exchange_weak 手动实现 CAS 循环
///
/// CAS 伪代码:
///   do {
///     expected = value;
///     desired  = expected + 1;
///   } while (!CAS(&value, expected, desired));
class LockFreeCounter {
public:
    LockFreeCounter() = default;

    /// fetch_add 是最简单的原子加法 — 性能最优, 应优先使用
    void increment_simple() {
        value_.fetch_add(1, std::memory_order_relaxed);
    }

    /// 用 compare_exchange_weak 手动实现加法 — 演示 CAS 循环
    ///
    /// compare_exchange_weak:
    ///   如果 value_ == expected, 则 value_ = expected + 1, 返回 true
    ///   否则 expected = value_ (更新为当前值), 返回 false
    ///
    /// weak vs strong:
    ///   weak:    可能"伪失败" (spurious failure) — 即使值匹配也可能返回 false
    ///            性能更高 (在某些平台上), 需要配合 while 循环使用
    ///   strong:  保证只有值真正不同时才返回 false, 逻辑更简单, 性能稍低
    void increment_cas() {
        long long expected = value_.load(std::memory_order_relaxed);
        while (!value_.compare_exchange_weak(expected, expected + 1,
                                             std::memory_order_release,
                                             std::memory_order_relaxed)) {
            // CAS 失败后 expected 已被更新为实际值, 直接重试即可
        }
    }

    /// 获取当前计数值 (带 acquire 语义, 保证能看到其他线程的写入)
    long long get() const {
        return value_.load(std::memory_order_acquire);
    }

    /// 重置计数器为 0
    void reset() {
        value_.store(0, std::memory_order_release);
    }

private:
    std::atomic<long long> value_{0};
};
