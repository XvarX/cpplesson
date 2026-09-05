#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency/thread_pool.hpp — 线程池 (Thread Pool)                           ║
// ║                                                                              ║
// ║  本模块实现一个工业级线程池, 综合运用以下 C++20/23 并发原语:                    ║
// ║                                                                              ║
// ║  任务队列 (Task Queue):                                                      ║
// ║    ① std::function<void()> — 类型擦除, 统一存储任意可调用对象                  ║
// ║    ② std::packaged_task<R()> — 将可调用对象包装成 std::future 的生产者        ║
// ║       packaged_task 本身不可拷贝, 需要用 shared_ptr 管理生命周期              ║
// ║       packaged_task.get_future() 返回 std::future<R>, 供调用者等待结果        ║
// ║    ③ submit() 使用 std::invoke_result_t 推导返回类型, 返回 std::future<R>    ║
// ║                                                                              ║
// ║  工作线程 (Worker Threads):                                                   ║
// ║    ① std::jthread (C++20) — 析构时自动 join, 避免 std::terminate             ║
// ║    ② 每个工作线程运行 worker_loop():                                          ║
// ║       - 阻塞等待任务 (condition_variable)                                     ║
// ║       - 取出任务并执行                                                       ║
// ║       - 循环直到收到停止请求                                                  ║
// ║                                                                              ║
// ║  优雅关闭 (Graceful Shutdown):                                                ║
// ║    ① std::stop_token (C++20) — 协作式线程取消                                ║
// ║       - stop_source::request_stop() 发出停止请求                              ║
// ║       - 工作线程通过 stop_token::stop_requested() 检查是否应该退出            ║
// ║       - 配合条件变量的谓词 wait, 同时等待"任务到达"和"停止请求"两个条件        ║
// ║    ② 析构顺序:                                                              ║
// ║       1. request_stop() — 通知所有线程准备退出                                ║
// ║       2. notify_all() — 唤醒所有在条件变量上等待的线程                        ║
// ║       3. workers_ 析构 → 每个 jthread 自动 join                              ║
// ║                                                                              ║
// ║  限流 (Flow Control):                                                        ║
// ║    ① std::counting_semaphore (C++20) — 资源计数信号量                        ║
// ║       - 初始化计数 = max_queue_size (有界队列的最大任务数)                    ║
// ║       - submit() 前 acquire(): 等一个空闲槽位                                 ║
// ║       - 任务被取出后 release(): 归还一个槽位                                  ║
// ║       - 当 max_queue_size == 0 时, 队列无界, 不使用信号量                     ║
// ║                                                                              ║
// ║  总结 — 为什么需要线程池?                                                    ║
// ║    - 避免频繁创建/销毁线程的开销 (线程是昂贵的 OS 资源)                       ║
// ║    - 限制并发线程数, 防止线程爆炸 (thundering herd)                           ║
// ║    - 解耦任务提交与任务执行 (生产者-消费者模式)                               ║
// ║    - 统一管理线程生命周期 (RAII + stop_token)                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <print>
#include <queue>
#include <semaphore>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <vector>

namespace concurrency {

/// 固定大小线程池 — 复用工作线程执行提交的任务。
///
/// ## 设计要点
///
/// ### 任务提交 (submit)
/// - 接受任意可调用对象 F 和参数 Args...
/// - 使用 std::packaged_task 将任务包装成 std::future 的生产者
/// - 返回 std::future<R> 供调用者异步获取结果
/// - 若设置了 max_queue_size, 队列满时 submit 会阻塞 (通过 counting_semaphore 限流)
///
/// ### 工作线程
/// - 每个工作线程运行 worker_loop(), 循环从任务队列取任务执行
/// - 空闲时阻塞在条件变量上, 不消耗 CPU
/// - 线程函数第一个参数为 std::stop_token, jthread 自动传入
///
/// ### 优雅关闭
/// - 析构时: request_stop() → notify_all() → jthread 自动 join
/// - 工作线程在处理完当前任务后检查 stop_requested() 并退出
/// - 队列中剩余的任务会被丢弃 (生产环境中应增加排空队列的选项)
///
/// ### 限流 (有界队列)
/// - counting_semaphore 维护"队列剩余槽位"计数
/// - 生产者 (submit): acquire() — 等待空位
/// - 消费者 (worker): release() — 释放空位
/// - 防止生产者提交过快导致队列无限增长 (背压 back-pressure)
///
/// @code
///   concurrency::ThreadPool pool(4, 100);  // 4 个工作线程, 最多 100 个待处理任务
///
///   // 提交任务, 获取 future
///   auto f1 = pool.submit([](int a, int b) { return a + b; }, 3, 5);
///   auto f2 = pool.submit([] { std::println("异步任务"); });
///
///   std::println("结果: {}", f1.get());  // 阻塞等待结果
///   f2.get();
/// @endcode
class ThreadPool {
public:
    /// 信号量最大计数 — 支持最多 65536 个待处理任务的有界队列。
    static constexpr std::ptrdiff_t kMaxQueueSlots = 65536;

    /// 构造线程池。
    ///
    /// @param num_threads   工作线程数 (默认: std::thread::hardware_concurrency())
    ///                      传 0 会退化为至少 1 个线程
    /// @param max_queue_size 任务队列最大容量, 0 表示无界 (默认 0, 不使用信号量限流)
    ///
    /// @throws std::invalid_argument 如果 max_queue_size > kMaxQueueSlots
    explicit ThreadPool(
        size_t num_threads = std::thread::hardware_concurrency(),
        size_t max_queue_size = 0);

    /// 析构 — 优雅关闭线程池。
    ///
    /// 关闭流程:
    ///   1. request_stop() — 通知所有工作线程准备退出
    ///   2. notify_all()   — 唤醒所有在条件变量上阻塞的线程
    ///   3. workers_ 析构  — 每个 std::jthread 自动调用 join()
    ///
    /// @warning 析构函数会阻塞直到所有工作线程退出 (join)。
    ///          如果某个任务陷入死循环且不检查 stop_token, 析构也会卡住。
    ~ThreadPool();

    // ── 不可拷贝, 不可移动 ──────────────────────────────────────────────────
    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&)                 = delete;
    ThreadPool& operator=(ThreadPool&&)      = delete;

    // ── 任务提交 ────────────────────────────────────────────────────────────

    /// 向线程池提交一个任务, 返回 std::future 用于获取执行结果。
    ///
    /// 这是线程池最核心的方法。内部流程:
    ///   1. 用 std::packaged_task 包装用户的可调用对象
    ///   2. 从 packaged_task 获取 std::future, 作为返回值
    ///   3. 将 packaged_task 包装成 std::function<void()> 放入任务队列
    ///   4. 如果有界队列, acquire 一个信号量槽位 (可能阻塞)
    ///   5. 通知一个工作线程来取任务
    ///
    /// @tparam F    可调用对象类型 (函数、lambda、函数对象等)
    /// @tparam Args 参数类型包
    /// @param f     可调用对象
    /// @param args  参数
    /// @return std::future<ReturnType> 用于获取任务返回值或异常
    ///
    /// @warning 如果线程池已停止 (stop_requested), 提交新任务的行为是未定义的
    ///          (生产环境中应抛出异常或返回 std::nullopt)
    ///
    /// @code
    ///   ThreadPool pool(4);
    ///   auto fut = pool.submit([](int x) { return x * x; }, 10);
    ///   std::println("{}", fut.get());  // 100
    /// @endcode
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    // ── 线程池管理 ──────────────────────────────────────────────────────────

    /// 动态调整工作线程数。
    ///
    /// - 增加线程: 立即创建新的 jthread 并启动 worker_loop()
    /// - 减少线程: 通知所有工作线程, ID >= new_size 的线程自动退出
    ///
    /// @param new_size 新的目标工作线程数
    ///
    /// @warning 减少线程时, 正在执行的任务不会被中断 (协作式退出)
    void resize(size_t new_size);

    /// 查询当前工作线程数 (仅快照值, 可能在查询后立即变化)。
    [[nodiscard]] size_t worker_count() const noexcept;

    /// 查询当前队列中等待执行的任务数 (仅快照值)。
    [[nodiscard]] size_t pending_count() const noexcept;

    /// 是否有界队列 (max_queue_size > 0)。
    [[nodiscard]] bool is_bounded() const noexcept;

private:
    /// 工作线程的主循环 — 每个 jthread 执行此函数。
    ///
    /// 循环逻辑:
    ///   1. 在条件变量上等待: 任务到达 或 停止请求 或 缩容信号
    ///   2. 如果收到停止请求且队列为空 → 退出
    ///   3. 如果 worker_id >= 目标线程数 → 退出 (缩容)
    ///   4. 从队列取一个任务, 释放信号量槽位 (有界模式)
    ///   5. 在锁外执行任务 (避免阻塞其他线程)
    ///
    /// @param stoken   由 jthread 自动传入的 stop_token
    /// @param worker_id 工作线程的唯一编号 (0-based)
    void worker_loop(std::stop_token stoken, size_t worker_id);

    // ── 数据成员 ────────────────────────────────────────────────────────────

    /// 工作线程容器 — 每个 std::jthread 代表一个活跃的工作线程。
    /// jthread 在析构时自动 join, 因此不需要手动管理。
    std::vector<std::jthread> workers_;

    /// 任务队列 — 存储 std::function<void()> 类型擦除的任务。
    /// 使用 std::function 而非模板是为了统一队列元素类型。
    std::queue<std::function<void()>> tasks_;

    /// 互斥锁 — 保护任务队列和条件变量的并发访问。
    mutable std::mutex mutex_;

    /// 条件变量 — 工作线程在此等待任务到达。
    /// notify_one(): 有新任务时通知一个工作线程
    /// notify_all(): 停止或缩容时通知所有工作线程
    std::condition_variable cv_;

    /// 任务槽位信号量 — 用于有界队列的限流 (背压 back-pressure)。
    ///
    /// 计数含义: 当前队列中可用的空闲槽位数。
    /// - submit() 前 acquire(): 消耗一个槽位 (槽位不足则阻塞, 实现背压)
    /// - 任务被取出后 release(): 归还一个槽位
    ///
    /// 初始计数为 max_queue_size (有界模式) 或未使用 (无界模式)。
    std::counting_semaphore<kMaxQueueSlots> task_slots_{0};

    /// 队列最大容量 — 0 表示无界 (不使用信号量限流)。
    size_t max_queue_size_;

    /// 当前待处理任务数的原子快照 — 用于 pending_count() 查询。
    /// 使用 memory_order_relaxed 因为仅用于监控/日志, 不参与同步。
    std::atomic<size_t> pending_tasks_{0};

    /// 目标工作线程数 — resize() 修改, worker_loop() 读取。
    /// 工作线程定期检查自己的 ID 是否 >= 此值来决定是否退出。
    std::atomic<size_t> target_workers_{0};

    /// 停止源 — 析构时调用 request_stop() 通知所有工作线程退出。
    /// 每个 worker 通过 jthread 的 stop_token (自动从 stop_source 派生) 接收停止信号。
    std::stop_source stop_source_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// 模板实现 (必须在头文件中)
// ═══════════════════════════════════════════════════════════════════════════════

template <typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {

    using ReturnType = std::invoke_result_t<F, Args...>;

    // ═════════════════════════════════════════════════════════════════════════
    // 步骤 1: 用 std::packaged_task 包装可调用对象
    //
    // packaged_task<R(Args...)> 将可调用对象与 future-promise 绑定:
    //   - 调用 packaged_task() → 执行任务, 结果通过 promise 传递给 future
    //   - packaged_task.get_future() → 获取绑定的 future
    //
    // ⚠️ packaged_task 不可拷贝, 只可移动 — 必须用 shared_ptr 管理
    //    因为我们要把它丢进 std::function<void()> (要求可拷贝)
    // ═════════════════════════════════════════════════════════════════════════

    // 使用 lambda + 捕获包展开 (C++20) 完美转发参数
    auto bound_callable = [f = std::forward<F>(f),
                           ...args = std::forward<Args>(args)]() mutable -> ReturnType {
        return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    };

    auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>>(
        std::move(bound_callable));

    // 提前获取 future — 必须在 packaged_task 被移动前调用
    std::future<ReturnType> result = task_ptr->get_future();

    // ═════════════════════════════════════════════════════════════════════════
    // 步骤 2: 信号量限流 (仅在有界模式下)
    //
    // counting_semaphore::acquire() — 如果计数器为 0, 当前线程阻塞
    // 这实现了"背压" (back-pressure): 当队列满时, 提交者自动减速
    // ═════════════════════════════════════════════════════════════════════════
    if (max_queue_size_ > 0) {
        task_slots_.acquire();
    }

    // ═════════════════════════════════════════════════════════════════════════
    // 步骤 3: 将 packaged_task 包装成 std::function<void()> 入队
    //
    // task_ptr 是 shared_ptr<packaged_task<ReturnType()>>,
    // lambda 捕获 task_ptr 的拷贝 (引用计数 +1), 保证 packaged_task
    // 在执行前不会被销毁
    // ═════════════════════════════════════════════════════════════════════════
    {
        std::lock_guard lock(mutex_);
        tasks_.emplace([task_ptr]() {
            (*task_ptr)();  // 执行 packaged_task → 结果写入 promise → future 就绪
        });
        pending_tasks_.fetch_add(1, std::memory_order_relaxed);
    }

    // ═════════════════════════════════════════════════════════════════════════
    // 步骤 4: 通知一个工作线程来取任务
    //
    // notify_one 只唤醒一个等待线程 — 避免惊群效应 (thundering herd)
    // 注意: 这里在锁外 notify, 减少被唤醒线程立即阻塞抢锁的概率
    // ═════════════════════════════════════════════════════════════════════════
    cv_.notify_one();

    return result;
}

}  // namespace concurrency
