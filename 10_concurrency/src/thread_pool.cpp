// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/thread_pool.cpp — ThreadPool 非模板成员实现                       ║
// ║                                                                              ║
// ║  本文件实现 ThreadPool 的非模板成员函数:                                      ║
// ║    - 构造/析构: 线程创建与优雅关闭                                            ║
// ║    - worker_loop: 工作线程主循环 (条件变量 + stop_token + 信号量)             ║
// ║    - resize: 动态调整线程数                                                    ║
// ║    - worker_count / pending_count / is_bounded: 状态查询                      ║
// ║                                                                              ║
// ║  注意: submit() 是模板函数, 实现在头文件中。                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "concurrency/thread_pool.hpp"

#include <print>
#include <stdexcept>

namespace concurrency {

// ═══════════════════════════════════════════════════════════════════════════════
// 构造 / 析构
// ═══════════════════════════════════════════════════════════════════════════════

ThreadPool::ThreadPool(size_t num_threads, size_t max_queue_size)
    : task_slots_(0)               // 信号量初始计数为 0, 稍后在有界模式下 release
    , max_queue_size_(max_queue_size) {

    // ── 参数验证 ──────────────────────────────────────────────────────────
    if (max_queue_size > static_cast<size_t>(kMaxQueueSlots)) {
        throw std::invalid_argument(
            "max_queue_size 超过信号量上限 kMaxQueueSlots");
    }

    // ── 确保至少 1 个工作线程 ──────────────────────────────────────────────
    if (num_threads == 0) {
        num_threads = 1;
    }

    target_workers_.store(num_threads, std::memory_order_relaxed);

    // ── 初始化信号量 (有界队列模式) ────────────────────────────────────────
    // release(n) 将内部计数器增加 n; 初始为 0, release 后计数 = max_queue_size
    // 含义: 队列中初始有 max_queue_size 个空闲槽位
    if (max_queue_size_ > 0) {
        task_slots_.release(static_cast<std::ptrdiff_t>(max_queue_size_));
        std::println("  [线程池] 有界队列模式, 最大 {} 个待处理任务", max_queue_size_);
    } else {
        std::println("  [线程池] 无界队列模式 (不限流)");
    }

    std::println("  [线程池] 创建 {} 个工作线程", num_threads);

    // ── 创建工作线程 ──────────────────────────────────────────────────────
    // jthread 的线程函数第一个参数如果是 stop_token, jthread 自动传入
    // 这里用 lambda 捕获 this + worker_id, 再转发给 worker_loop
    workers_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this, i](std::stop_token stoken) {
            worker_loop(std::move(stoken), i);
        });
    }
}

ThreadPool::~ThreadPool() {
    std::println("  [线程池] 开始优雅关闭...");

    // ── 步骤 1: 发出停止请求 ──────────────────────────────────────────────
    // 所有从 stop_source_ 派生的 stop_token 的 stop_requested() 将返回 true
    stop_source_.request_stop();

    // ── 步骤 2: 唤醒所有工作线程 ──────────────────────────────────────────
    // 工作线程在条件变量上等待, 被唤醒后会检查 stop_requested(), 然后退出
    cv_.notify_all();

    // ── 步骤 3: jthread 自动 join ─────────────────────────────────────────
    // workers_ 的析构函数会销毁每个 jthread, jthread 析构时自动 join()
    // 因此当析构函数返回时, 所有工作线程已经安全退出
    //
    // ⚠️ 前提: 工作线程会检查 stop_requested() 并退出
    // 如果某个工作线程无视停止请求继续运行, 这里会永久阻塞

    std::println("  [线程池] 所有工作线程已退出, 线程池关闭完成");
}

// ═══════════════════════════════════════════════════════════════════════════════
// worker_loop — 工作线程主循环
// ═══════════════════════════════════════════════════════════════════════════════

void ThreadPool::worker_loop(std::stop_token stoken, size_t worker_id) {
    std::println("  [Worker-{}] 启动", worker_id);

    while (true) {
        std::function<void()> task;

        // ── 在互斥锁保护下等待任务 ──────────────────────────────────────
        {
            std::unique_lock lock(mutex_);

            // ═════════════════════════════════════════════════════════════
            // 条件变量谓词 wait — 三个退出条件:
            //   ① 队列非空 → 有任务可执行
            //   ② stop_requested → 线程池关闭, 停止等待
            //   ③ worker_id 超出目标线程数 → 缩容, 此线程退出
            //
            // cv.wait(lock, predicate) 等价于:
            //   while (!predicate()) { cv.wait(lock); }
            // 这样自动处理了虚假唤醒 (spurious wakeup)
            // ═════════════════════════════════════════════════════════════
            cv_.wait(lock, [&] {
                return !tasks_.empty()
                    || stoken.stop_requested()
                    || worker_id >= target_workers_.load(std::memory_order_relaxed);
            });

            // ── 退出条件检查 ──────────────────────────────────────────

            // 条件 1: 线程池正在关闭 且 队列为空
            // 为什么还要检查队列为空? — 即使收到停止请求, 也应先处理完剩余任务
            // (生产环境中可提供选项: 排空队列 vs 立即丢弃)
            if (stoken.stop_requested() && tasks_.empty()) {
                std::println("  [Worker-{}] 收到停止请求, 退出 (队列已空)", worker_id);
                return;
            }

            // 条件 2: 缩容 — 此线程的编号超出目标数
            if (worker_id >= target_workers_.load(std::memory_order_relaxed)) {
                std::println("  [Worker-{}] 线程池缩容, 退出", worker_id);
                return;
            }

            // 条件 3: 队列为空 (虚假唤醒 或 仅因缩容通知而醒来)
            if (tasks_.empty()) {
                continue;  // 回到循环开头再次等待
            }

            // ── 取出任务 ──────────────────────────────────────────────
            task = std::move(tasks_.front());
            tasks_.pop();
            pending_tasks_.fetch_sub(1, std::memory_order_relaxed);
        }
        // ═════════════════════════════════════════════════════════════════
        // 锁在此处释放 — 任务执行在锁外, 不会阻塞其他线程提交或取任务
        // ═════════════════════════════════════════════════════════════════

        // ── 归还信号量槽位 (有界队列模式) ─────────────────────────────────
        // 任务已取出, 队列中多了一个空位, release 通知等待中的 submit() 调用者
        if (max_queue_size_ > 0) {
            task_slots_.release();
        }

        // ── 执行任务 ────────────────────────────────────────────────────
        // 注意: 如果任务抛出异常, std::function<void()> 内部会将其传播到
        // packaged_task 的 promise, 最终由 future::get() 重新抛出。
        // 工作线程本身不会因为任务异常而崩溃。
        try {
            task();
        } catch (...) {
            // 理论上 std::function<void()> 不应该让异常逃逸到这里,
            // 因为 packaged_task 会捕获异常并通过 promise 传播。
            // 但作为防御性编程, 记录异常并继续。
            std::println("  [Worker-{}] 警告: 任务抛出未捕获的异常", worker_id);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// resize — 动态调整线程数
// ═══════════════════════════════════════════════════════════════════════════════

void ThreadPool::resize(size_t new_size) {
    if (new_size == 0) {
        new_size = 1;  // 至少保留 1 个工作线程
    }

    size_t old_size = target_workers_.load(std::memory_order_relaxed);

    if (new_size == old_size) {
        return;  // 没有变化
    }

    std::println("  [线程池] 调整线程数: {} → {}", old_size, new_size);

    // ── 更新目标线程数 (原子操作) ────────────────────────────────────────
    target_workers_.store(new_size, std::memory_order_release);

    if (new_size > old_size) {
        // ── 扩容: 创建新的工作线程 ─────────────────────────────────────
        // 注意: workers_.size() 可能与 old_size 不同 (如果之前缩容的线程还在退出中)
        // 这里以 workers_.size() 为准分配新 worker_id
        size_t current_count = workers_.size();
        workers_.reserve(new_size);
        for (size_t i = current_count; i < new_size; ++i) {
            workers_.emplace_back([this, i](std::stop_token stoken) {
                worker_loop(std::move(stoken), i);
            });
        }
        std::println("  [线程池] 已创建 {} 个新线程", new_size - current_count);
    } else {
        // ── 缩容: 唤醒所有线程, ID >= new_size 的将自动退出 ─────────────
        // 注意: 被裁减的线程如果正在执行任务, 会完成当前任务后才检查 ID 并退出
        // 这是"协作式"缩容, 不会强制中断正在运行的任务
        cv_.notify_all();
        std::println("  [线程池] 已通知 {} 个线程退出 (协作式缩容)", old_size - new_size);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 状态查询
// ═══════════════════════════════════════════════════════════════════════════════

size_t ThreadPool::worker_count() const noexcept {
    // target_workers_ 是 resize 后的目标值
    // 注意: 缩容后, 部分线程可能还在执行最后的任务, 尚未退出
    return target_workers_.load(std::memory_order_relaxed);
}

size_t ThreadPool::pending_count() const noexcept {
    // pending_tasks_ 是原子计数器, 在入队时 +1, 出队时 -1
    return pending_tasks_.load(std::memory_order_relaxed);
}

bool ThreadPool::is_bounded() const noexcept {
    return max_queue_size_ > 0;
}

}  // namespace concurrency
