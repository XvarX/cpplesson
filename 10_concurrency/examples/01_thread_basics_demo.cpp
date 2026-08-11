// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  10_concurrency — 01: std::thread 基础                                      ║
// ║  学习目标:                                                                  ║
// ║    1. 用函数指针、成员函数、Lambda 三种方式创建线程                          ║
// ║    2. 理解 join / detach 的区别和正确用法                                    ║
// ║    3. 获取线程 ID 和查询硬件并发数                                           ║
// ║    4. 掌握线程生命周期管理 (RAII 惯用法)                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>

#include "shared/lesson_utils.hpp"
#include "concurrency/thread_basics.hpp"

using namespace std::chrono_literals;

int main() {
    lesson::print_header("10.1  std::thread 基础");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 创建线程的三种方式
    // ═══════════════════════════════════════════════════════════════════════════
    // std::thread 是 C++ 线程抽象，构造时传入一个"可调用对象" + 参数，
    // 构造函数内部启动新线程（立刻执行），不等你调用 start()。
    //
    // 三种常见可调用对象:
    //   ① 普通函数指针
    //   ② 函数对象 (仿函数 / struct with operator())
    //   ③ Lambda 表达式 (最常用)
    lesson::print_subtitle("Part 1: 三种方式创建线程");

    // 方式①: 函数指针 — 使用库中的 worker() 函数
    // 构造参数会按值拷贝到新线程栈上，因此无悬垂风险
    std::thread t1(worker, 1, 3);

    // 方式②: 仿函数 (函数对象) — 使用库中的 Task 结构体
    Task task{"下载器"};
    std::thread t2(task);               // task 被拷贝进线程

    // 方式③: Lambda (最灵活)
    std::thread t3([](std::string msg) {
        for (int i = 1; i <= 3; ++i) {
            std::println("  [Lambda] {} — 第 {} 遍", msg, i);
            std::this_thread::sleep_for(120ms);
        }
    }, "你好");

    // 你必须 join 或 detach 每个线程，否则析构时会 std::terminate !
    t1.join();
    t2.join();
    t3.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: join vs detach — 线程生命周期管理
    // ═══════════════════════════════════════════════════════════════════════════
    // join():  阻塞当前线程，等待目标线程完成。之后 thread 对象变为 "not-joinable"。
    // detach(): 分离线程，让它在后台独立运行。之后 thread 对象变为 "not-joinable"。
    //           ⚠️ 主线程退出后，detach 的线程可能来不及完成！慎用。
    //
    // 黄金法则: 在离开作用域前，必须确认每个 std::thread 为 joined 或 detached。
    //           C++20 的 std::jthread 在析构时自动 join，见 06_jthread_and_parallel。
    lesson::print_subtitle("Part 2: join vs detach");

    std::thread t_join([] {
        std::println("  这条消息在 t_join 线程中");
        std::this_thread::sleep_for(200ms);
    });
    t_join.join();                       // 主线程在此等待 t_join 完成
    std::println("  t_join 线程已完成 (主线程继续)");

    // detach 示例 (谨慎使用)
    std::thread t_detach([] {
        std::println("  detach 线程开始");
        std::this_thread::sleep_for(300ms);
        std::println("  detach 线程结束");
    });
    t_detach.detach();                   // 线程独立运行
    // 注意: 不能 join/detach 后再 join/detach — joinable() 会返回 false

    // 小睡一下确保 detach 线程能看到输出
    std::this_thread::sleep_for(500ms);
    std::println("  主线程完成");

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 线程 ID 和硬件并发数
    // ═══════════════════════════════════════════════════════════════════════════
    // std::this_thread::get_id() — 获取当前线程 ID (用于日志/调试)
    // std::thread::get_id()       — 获取指定线程对象的 ID
    // std::thread::hardware_concurrency() — 返回 CPU 可并行线程数 (估算值)
    lesson::print_subtitle("Part 3: 线程 ID 与硬件并发数");

    std::println("  主线程 ID:    {}", std::this_thread::get_id());
    std::println("  硬件并发数:   {} 个逻辑核心", std::thread::hardware_concurrency());

    std::thread t_id([] {
        std::println("  工作线程 ID:  {}", std::this_thread::get_id());
    });
    // t_id 还是 joinable 的
    std::println("  t_id 对象 ID: {} (joinable={})",
                 t_id.get_id(),
                 t_id.joinable() ? "true" : "false");
    t_id.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 成员函数作为线程入口
    // ═══════════════════════════════════════════════════════════════════════════
    // 语法: std::thread(&ClassName::method, &instance, args...)
    // 第一个参数是成员函数指针，第二个参数是实例地址（或引用包装器）
    lesson::print_subtitle("Part 4: 成员函数作为线程入口");

    // 使用库中的 Processor 结构体
    Processor proc{42};
    std::thread t_member(&Processor::process, &proc, 3);
    //                     成员函数指针       实例指针   参数
    t_member.join();

    lesson::print_separator();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 批量创建线程 (线程池雏形)
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 5: 批量创建线程 (RAII 管理)");

    const int N = 4;
    std::vector<std::thread> pool;       // 线程容器
    pool.reserve(N);

    for (int i = 0; i < N; ++i) {
        pool.emplace_back([i] {
            std::println("  线程池#{} 开始工作 (ID={})",
                         i, std::this_thread::get_id());
            std::this_thread::sleep_for(150ms);
            std::println("  线程池#{} 完成", i);
        });
    }

    // RAII: 用容器统一管理线程，批量 join
    for (auto& t : pool) {
        if (t.joinable()) t.join();      // 安全检查
    }
    std::println("  所有 {} 个线程已完成", pool.size());

    // ═══════════════════════════════════════════════════════════════════════════
    // ⚠️ 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("  1. 忘记 join/detach → thread 析构时调用 std::terminate，程序直接终止");
    std::println("  2. detach 后主线程退出 → 分离线程可能来不及完成任务");
    std::println("  3. 传递局部变量的引用 → 线程运行前引用可能已失效 (用拷贝或 shared_ptr)");
    std::println("  4. 多次 join/detach → 会抛 std::system_error 异常");
    std::println("  5. 线程函数抛出未捕获异常 → std::terminate (用 try-catch 包裹)");

    // ═══════════════════════════════════════════════════════════════════════════
    // 📝 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("  1. 用 std::thread 创建 3 个线程，每个打印不同的消息，观察执行顺序");
    std::println("  2. 把共享变量作为参数传给多个线程 (注意数据竞争，后续会讲如何保护)");
    std::println("  3. 实现一个 thread_guard 类 (RAII)，构造时接受 std::thread，析构时自动 join");
    std::println("  4. 尝试创建 hardware_concurrency() 个线程，每个计算一段素数");

    return 0;
}
