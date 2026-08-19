// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 04: future 与 promise                                     ║
// ║  学习目标:                                                                  ║
// ║    1. 理解 promise-future 通信模型 — 单次结果的线程间传递                     ║
// ║    2. 掌握 std::async 的两种启动策略 (launch::async / launch::deferred)       ║
// ║    3. 了解 packaged_task 和 shared_future 的适用场景                          ║
// ║    4. 学会用 wait_for 实现超时等待                                           ║
// ║    5. 理解 future 如何传播异常                                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include <numeric>
#include <stdexcept>

#include "shared/lesson_utils.hpp"
#include "concurrency/future_and_promise.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.4  future 与 promise");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: std::async — 最简单的异步执行方式
    // ═══════════════════════════════════════════════════════════════════════════
    // std::async 接收一个可调用对象，返回一个 std::future<T>。
    // future 就像一张"期票"——现在没有值，但未来某个时刻会有。
    //
    // 启动策略:
    //   launch::async:      保证在新线程中异步执行 (立即)
    //   launch::deferred:   延迟求值 — 直到调用 get() 才在当前线程同步执行
    //   默认 (不指定):      等价于 async | deferred，由实现决定 (通常为 async)
    //
    // 区别:
    //   async   → 适合耗时计算，不阻塞调用线程
    //   deferred → 适合可能不需要的计算 (如果从不调用 get，任务绝不执行)
    lesson::print_subtitle("Part 1: std::async 异步执行");

    // ── launch::async: 在新线程异步执行 — 使用库中的 compute_sum ──
    std::println("  [主线程] 发起异步计算...");
    std::future<int> f1 = std::async(std::launch::async, compute_sum, 1, 100);
    std::println("  [主线程] async 已返回, 任务在后台执行中...");

    // 主线程可以做其他事情
    std::println("  [主线程] 做点别的... (不会被异步任务阻塞)");
    std::this_thread::sleep_for(100ms);

    // future::get() — 阻塞直到结果就绪，然后返回结果
    // ⚠️ get() 只能调用一次! 调用后 future 状态变为 invalid
    int result1 = f1.get();
    std::println("  [主线程] 异步结果: {}", result1);

    // ── launch::deferred: 延迟执行, 在调用 get() 时才执行 ──
    std::future<int> f2 = std::async(std::launch::deferred, compute_sum, 1, 50);
    std::println("  [主线程] deferred async 已创建, 但任务尚未执行");
    std::println("  [主线程] 现在调用 get(), 任务将在当前线程执行...");
    int result2 = f2.get();  // 此时才真正调用 compute_sum
    std::println("  [主线程] deferred 结果: {}", result2);

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: promise / future — 手动设置结果的管道
    // ═══════════════════════════════════════════════════════════════════════════
    // promise-future 是一对一通信管道:
    //   promise 端 — 生产者: 持有数据的"承诺方", 通过 set_value() 传递结果
    //   future 端  — 消费者: 持有数据的"接收方", 通过 get() 等待并获取结果
    //
    // 与 async 不同: promise 让你完全控制"何时"和"在哪个线程"设置结果
    // 适用场景: 将异步操作的结果封装为 future, 方便调用方等待
    lesson::print_subtitle("Part 2: promise-future 手动管道");

    std::promise<std::string> prom;      // promise: 承诺会给出一个 string
    std::future<std::string> fut = prom.get_future();  // future: 等待这个承诺兑现

    // 在另一个线程中兑现承诺
    std::thread prom_thread([&prom] {
        std::println("  [承诺线程] 开始工作...");
        std::this_thread::sleep_for(500ms);

        try {
            // set_value() 设置结果，future 端的 get() 将解除阻塞
            prom.set_value("承诺已兑现 — 这是计算结果!");
            std::println("  [承诺线程] 结果已发送");
        } catch (...) {
            // 如果发生异常，通过 set_exception 传递给 future
            prom.set_exception(std::current_exception());
        }
    });

    std::println("  [主线程] 等待承诺兑现...");
    // future::get() 会阻塞直到 promise 调用 set_value 或 set_exception
    std::string msg = fut.get();
    std::println("  [主线程] 收到: {}", msg);

    prom_thread.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: packaged_task — 将可调用对象包装成 future
    // ═══════════════════════════════════════════════════════════════════════════
    // packaged_task 包装一个可调用对象，自动创建 promise-future 对。
    // 调用 packaged_task 时，内部自动调用 set_value() 传递返回值。
    //
    // 使用场景:
    //   ① 需要把已有函数"future 化" (如线程池的任务提交)
    //   ② 把任务对象和数据流解耦 — 哪个线程执行由你决定
    lesson::print_subtitle("Part 3: std::packaged_task");

    // 包装库中的 risky_division 函数
    std::packaged_task<int(int, int)> task(risky_division);

    // 获取与 task 关联的 future
    std::future<int> task_result = task.get_future();

    // 把 task 交给线程执行
    std::thread task_thread(std::move(task), 100, 7);
    //                         ↑ packaged_task 只能移动，不可复制

    std::println("  [主线程] packaged_task 已提交");
    int div_result = task_result.get();
    std::println("  [主线程] 计算结果: 100 / 7 = {}", div_result);

    task_thread.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: shared_future — 多个消费者共享同一个结果
    // ═══════════════════════════════════════════════════════════════════════════
    // future        — 独占: get() 只能调用一次 (移动语义, 内部状态被消耗)
    // shared_future — 共享: get() 可以多次调用 (返回 const 引用, 不消耗状态)
    //
    // 使用场景: 多个线程都需要等待同一个异步结果 (如广播通知)
    // 创建方式: future.share() 或直接从 promise::get_future() 转换
    lesson::print_subtitle("Part 4: std::shared_future — 多消费者共享结果");

    std::promise<int> shared_prom;
    std::shared_future<int> shared_fut = shared_prom.get_future().share();

    const int num_readers = 3;
    std::vector<std::thread> readers;
    for (int i = 0; i < num_readers; ++i) {
        readers.emplace_back([shared_fut, i] {        // shared_future 可拷贝!
            std::println("  [读者{}] 等待结果...", i);
            int val = shared_fut.get();               // 每个读者都能获取
            std::println("  [读者{}] 收到结果: {}", i, val);
        });
    }

    std::this_thread::sleep_for(100ms);
    shared_prom.set_value(42);  // 所有等待的读者同时收到结果

    for (auto& t : readers) t.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: wait_for 超时等待 + 异常传播
    // ═══════════════════════════════════════════════════════════════════════════
    // get() 会无限阻塞 — 不适合所有场景。
    // wait_for(timeout) 返回一个 std::future_status:
    //   ready:    结果就绪
    //   timeout:  超时未就绪
    //   deferred: 任务延迟执行 (launch::deferred)
    //
    // 异常传播: 如果异步任务抛出异常，get() 会重新抛出该异常 (std::exception_ptr)
    lesson::print_subtitle("Part 5: wait_for 超时等待 + 异常传播");

    // ── 超时等待示例 ──
    std::future<int> slow = std::async(std::launch::async, [] {
        std::this_thread::sleep_for(500ms);
        return 999;
    });

    std::println("  [主线程] 轮询等待结果 (超时 100ms)...");

    bool got_result = false;
    while (!got_result) {
        auto status = slow.wait_for(100ms);
        if (status == std::future_status::ready) {
            std::println("  [主线程] 结果就绪: {}", slow.get());
            got_result = true;
        } else if (status == std::future_status::timeout) {
            std::println("  [主线程] 还没好, 继续等待...");
        }
    }

    // ── 异常传播示例 ──
    lesson::print_separator("异常传播演示");

    // 使用库中的 risky_division — 传入除数为 0
    std::future<int> bad = std::async(std::launch::async, risky_division, 10, 0);
    std::println("  [主线程] 等待一个会失败的任务...");

    try {
        int bad_result = bad.get();  // 这里会重新抛出 std::runtime_error
        std::println("  结果: {}", bad_result);  // 不会执行到这里
    } catch (const std::exception& e) {
        std::println("  [主线程] 捕获异常: {}", e.what());
        std::println("  [主线程] 异常通过 future 成功传播!");
    }

    // ── 同时启动多个异步任务 ──
    lesson::print_separator("并行计算示例");

    std::future<long long> f_a = std::async(std::launch::async, [] {
        long long sum = 0;
        for (int i = 1; i <= 1'000'000; ++i) sum += i;
        return sum;
    });
    std::future<long long> f_b = std::async(std::launch::async, [] {
        long long sum = 0;
        for (int i = 1; i <= 500'000; ++i) sum += i * 2;
        return sum;
    });

    std::println("  任务 A 结果: {}", f_a.get());
    std::println("  任务 B 结果: {}", f_b.get());

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. future::get() 只能调用一次 → 第二次调用行为未定义");
    std::println("  2. async 返回的 future 析构时阻塞 → 赋值给变量, 别当临时对象丢弃");
    std::println("  3. promise 未 set_value/set_exception → future::get() 永远阻塞");
    std::println("  4. promise 析构时未 set_value → 抛出 broken_promise 异常给 future");
    std::println("  5. shared_future 不会自动同步 → 多线程同时 get() 需要外部同步 (C++11)");
    std::println("  6. launch::deferred 任务在非预期线程执行 → wait_for 不能超时 (返回 deferred)");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 用 async 并行下载 3 个“URL”(模拟: 不同的 sleep + 返回字符串), 汇总结果");
    std::println("  2. 用 promise-future 实现一个简单的超时机制 (wait_for + 条件变量取消)");
    std::println("  3. 实现一个“任务分发器”: packaged_task 被 push 到队列, 线程池线程 pop 执行");
    std::println("  4. 写一个函数, 同时启动 10 个 async 任务, 按完成顺序收集结果 (用 shared_future)");
    std::println("  5. 研究 future::then (C++ 提案) — 如果标准库没有, 自己实现一个简化版");

    return 0;
}
