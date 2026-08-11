// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/mutex_and_lock.hpp — 互斥量与锁管理                            ║
// ║  提供:                                                                       ║
// ║    - ThreadSafeCounter: 一个最简单的线程安全计数器 (mutex 保护)               ║
// ║    - 关于 lock_guard / unique_lock / scoped_lock / shared_mutex 的文档说明     ║
// ║                                                                              ║
// ║  锁类型速查表:                                                               ║
// ║    lock_guard  (C++11): 最简 RAII 锁, 构造时 lock, 析构时 unlock             ║
// ║                         不可复制/移动, 无法手动解锁                           ║
// ║    unique_lock (C++11): 灵活 RAII 锁, 支持延迟锁定/提前解锁/移动语义          ║
// ║                         必须与 condition_variable 搭配使用                    ║
// ║    scoped_lock (C++17): 同时锁定多个 mutex, 内部使用死锁避免算法              ║
// ║                         推荐替代 std::lock + lock_guard 组合                  ║
// ║    shared_lock (C++17): 共享锁 (读锁), 多个读者可同时持有                    ║
// ║                         与 std::shared_mutex 配合使用                          ║
// ║                                                                              ║
// ║  特殊 mutex:                                                                ║
// ║    timed_mutex:     支持 try_lock_for / try_lock_until, 可设置超时            ║
// ║    recursive_mutex: 同一线程可多次 lock (不阻塞自己), 注意: 通常为设计缺陷信号 ║
// ║    shared_mutex:    读写锁 (C++17) — 多读单写                                 ║
// ║                                                                              ║
// ║  死锁四条件 (破坏任一即可预防):                                               ║
// ║    ① 互斥 — 资源不能被共享                                                   ║
// ║    ② 持有并等待 — 持有一个锁的同时等待另一个锁                                ║
// ║    ③ 不可剥夺 — 不能强行抢走线程持有的锁                                     ║
// ║    ④ 循环等待 — A→B 等锁形成环                                               ║
// ║                                                                              ║
// ║  避免策略:                                                                   ║
// ║    1. 统一锁的获取顺序 (破坏 ④)                                              ║
// ║    2. 使用 scoped_lock / std::lock 一次性获取所有锁                           ║
// ║    3. 使用 try_lock 带超时, 失败则释放所有已持有的锁并重试                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <mutex>
#include <print>

/// 线程安全的计数器 — mutex 最基本的应用示例。
/// 演示了如何用 std::mutex + std::lock_guard 保护共享数据。
///
/// 关键设计要点:
///   - 每次访问共享数据前加锁, 确保"读-改-写"的原子性
///   - 使用 RAII 锁包装器 (lock_guard), 绝不手动 lock/unlock
///   - get() 返回的是快照值, 调用后值可能已变化 (除非外部持有锁)
class ThreadSafeCounter {
public:
    ThreadSafeCounter() = default;

    /// 线程安全地增加计数
    void add(int delta) {
        std::lock_guard<std::mutex> guard(mutex_);
        value_ += delta;
    }

    /// 线程安全地递增 1
    void increment() {
        std::lock_guard<std::mutex> guard(mutex_);
        ++value_;
    }

    /// 获取当前计数值的快照
    /// ⚠️ 返回值是瞬间快照 — 调用后实际值可能已被其他线程修改
    int get() const {
        std::lock_guard<std::mutex> guard(mutex_);
        return value_;
    }

private:
    mutable std::mutex mutex_;  // mutable: 允许在 const 函数中加锁
    int value_ = 0;
};
