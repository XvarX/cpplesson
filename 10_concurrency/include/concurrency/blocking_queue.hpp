#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/blocking_queue.hpp — 线程安全的阻塞队列                         ║
// ║                                                                              ║
// ║  BlockingQueue<T> 是生产者-消费者模型的核心组件:                              ║
// ║    - 生产者往队列放数据 (push)                                                ║
// ║    - 消费者从队列取数据 (pop)                                                 ║
// ║    - 队列空时消费者阻塞等待 (条件变量)                                        ║
// ║    - 队列满时生产者阻塞等待 (条件变量 / 限流)                                 ║
// ║                                                                              ║
// ║  设计要点:                                                                   ║
// ║    - 使用 std::condition_variable 实现等待/通知                               ║
// ║    - 使用谓词版本的 wait() 防止虚假唤醒                                       ║
// ║    - notify 在解锁之后调用, 避免惊群效应                                      ║
// ║    - 模板化以适应任意数据类型                                                 ║
// ║                                                                              ║
// ║  关于条件变量:                                                               ║
// ║    cv.wait(lock, predicate) 等价于:                                          ║
// ║      while (!predicate()) { cv.wait(lock); }                                 ║
// ║    为什么需要 while 循环?                                                    ║
// ║      ① 虚假唤醒 (spurious wakeup): OS 可能在没有 notify 时唤醒线程           ║
// ║      ② 条件检查: 醒来后必须重新检查条件是否真正满足                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <condition_variable>
#include <mutex>
#include <queue>
#include <print>

/// 线程安全的阻塞队列。
/// 支持有界队列 (max_size) 实现生产者限流。
///
/// @tparam T 队列中存储的元素类型
template <typename T>
class BlockingQueue {
public:
    /// 构造阻塞队列
    /// @param max_size 队列最大容量, 默认 10
    explicit BlockingQueue(size_t max_size = 10) : max_size_(max_size) {}

    // ── 生产者: 放入元素 ──────────────────────────────────────────────────

    /// 向队列中放入一个元素。
    /// 如果队列已满, 当前线程阻塞直到有空位。
    ///
    /// @param item 要放入的元素 (使用移动语义, 避免拷贝)
    void push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);

        // ═════════════════════════════════════════════════════════════════
        // 谓词 wait: cv.wait(lock, predicate)
        // 等价于: while (!predicate()) { cv.wait(lock); }
        //
        // 为什么要用 predicate 版本的 wait？
        //   ① 虚假唤醒: 操作系统可能在没有 notify 的情况下唤醒线程
        //   ② 条件检查: 醒来后必须重新检查条件是否真正满足
        //   ③ 使用 predicate 版本, 标准库自动处理上述问题
        // ═════════════════════════════════════════════════════════════════
        cv_producer_.wait(lock, [this] {
            return queue_.size() < max_size_;  // 队列有空位才继续
        });

        queue_.push(std::move(item));
        std::println("  [生产者] 放入一个元素 (队列大小: {})", queue_.size());

        // notify_one vs notify_all:
        //   notify_one: 唤醒一个等待线程 — 适合"一个数据通知一个消费者"
        //   notify_all: 唤醒所有等待线程 — 适合"状态变化所有线程都需要重新判断"
        //   这里队列有新元素, 只需唤醒一个消费者即可
        lock.unlock();  // 提前解锁, 避免"惊群"效应 — 被唤醒的线程不需要等锁释放
        cv_consumer_.notify_one();
    }

    // ── 消费者: 取出元素 ──────────────────────────────────────────────────

    /// 从队列中取出一个元素。
    /// 如果队列为空, 当前线程阻塞直到有数据。
    ///
    /// @return 队列头部元素 (移动语义)
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_consumer_.wait(lock, [this] {
            return !queue_.empty();   // 队列非空才继续
        });

        T item = std::move(queue_.front());
        queue_.pop();
        std::println("  [消费者] 取出一个元素 (队列大小: {})", queue_.size());

        lock.unlock();
        cv_producer_.notify_one();    // 通知生产者队列有空位了
        return item;
    }

    /// 尝试放入元素 (非阻塞版本)。
    /// 注意: 这里使用 unique_lock 而非 lock_guard,
    /// 因为需要在 notify 前提前解锁 (推荐做法: 解锁后再 notify)。
    /// @param item 要放入的元素
    /// @return true 表示成功放入, false 表示队列已满
    bool try_push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.size() >= max_size_) {
            return false;
        }
        queue_.push(std::move(item));
        lock.unlock();   // 提前解锁 — 避免被唤醒的线程立即阻塞抢锁
        cv_consumer_.notify_one();
        return true;
    }

    /// 尝试取出元素 (非阻塞版本)。
    /// @param out 输出参数, 接收取出的元素
    /// @return true 表示成功取出, false 表示队列为空
    bool try_pop(T& out) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        out = std::move(queue_.front());
        queue_.pop();
        lock.unlock();   // 提前解锁
        cv_producer_.notify_one();
        return true;
    }

    /// 获取当前队列中的元素个数 (快照值)
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    /// 判断队列是否为空
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_producer_;   // 生产者等待队列"不满"
    std::condition_variable cv_consumer_;   // 消费者等待队列"不空"
    size_t max_size_;
};
