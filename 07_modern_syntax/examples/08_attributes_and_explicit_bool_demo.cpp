// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: [[no_unique_address]] / explicit(bool) / hardware_destructive_       ║
// ║        interference_size (C++17/20)                                         ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握 [[no_unique_address]] 优化空成员, 理解 EBO 与成员版 EBO 的区别   ║
// ║    2. 学会 explicit(bool) 实现条件 explicit 转换构造函数                   ║
// ║    3. 理解伪共享问题并使用 hardware_destructive_interference_size 规避      ║
// ║    4. 识别三个特性各自的适用场景和常见陷阱                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/attributes_and_explicit_bool.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <type_traits>
#include <new>
#include <atomic>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: [[no_unique_address]] —— 空成员零开销优化 (C++20)               ║
// ║  WHAT: 无状态的策略/标签类型作为成员时, 可以与其他成员共享存储空间          ║
// ║  WHY:  空类成员至少占 1 字节(C++ 对象必须区分地址), 浪费内存              ║
// ║        基类可以 EBO, 成员不行 —— 直到 C++20 补上了这个缺口                  ║
// ║  WHEN: 策略模式(policy); 空分配器/比较器/删除器; 模板元编程                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_no_unique_address() {
    lesson::print_header("Part 1: [[no_unique_address]] 空成员优化 (C++20)");

    using namespace modern_syntax;

    // ── 1.1 认识空类的大小 ──
    {
        lesson::print_subtitle("1.1  空类的大小: 至少 1 字节");
        std::println("sizeof(空类 LoggingPolicy) = {} 字节", sizeof(LoggingPolicy));
        std::println("sizeof(空类 NoOpPolicy)    = {} 字节", sizeof(NoOpPolicy));
        lesson::print_note("空类大小为 1 字节——C++ 要求每个对象有唯一地址");
    }

    // ── 1.2 [[no_unique_address]] 的效果 ──
    {
        lesson::print_subtitle("1.2  [[no_unique_address]] 的优化效果");

        // Processor 中 logger 标记了 [[no_unique_address]]
        // 空 logger 可以共享地址, 不额外占用空间
        Processor proc;
        std::println("sizeof(Processor)  = {} 字节", sizeof(proc));
        std::println("sizeof(int)         = {} 字节 (data_ 成员)", sizeof(int));

        // 预期: sizeof(Processor) == sizeof(int) + 可能的对齐填充
        // 如果 logger 没有 [[no_unique_address]], Processor 至少会多 1 字节
        lesson::print_note("空 logger 与 int data_ 共享地址, 总大小仅取决于 data_");
    }

    // ── 1.3 使用 Processor ──
    {
        lesson::print_subtitle("1.3  使用带策略的处理器");
        Processor proc;
        proc.data_ = 42;
        proc.process();   // 内部调用 logger.log()
        std::println("data = {}", proc.data_);
    }

    // ── 1.4 模板版本: 编译期选择策略 ──
    {
        lesson::print_subtitle("1.4  模板 GenericProcessor: 编译期策略切换");

        // 使用 LoggingPolicy (有日志)
        GenericProcessor<LoggingPolicy> gp1;
        gp1.run();

        // 使用 NoOpPolicy (无操作, 编译器会优化掉所有调用)
        GenericProcessor<NoOpPolicy> gp2;
        gp2.run();  // logger.log() 是空函数, 编译器内联后完全消失

        // 比较: 两种特化的大小
        std::println("sizeof(GenericProcessor<LoggingPolicy>) = {} 字节",
                     sizeof(gp1));
        std::println("sizeof(GenericProcessor<NoOpPolicy>)    = {} 字节",
                     sizeof(gp2));
        // 两者都只占 int 的大小, 因为空策略不占空间
        lesson::print_note("两个模板特化大小相同: 空策略被优化掉了!");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: explicit(bool) —— 条件 explicit (C++20)                          ║
// ║  WHAT: 构造函数前面的 explicit 可接受编译期布尔表达式                       ║
// ║        explicit(true)  = 禁止隐式转换;  explicit(false) = 允许隐式转换      ║
// ║  WHY:  模板包装器需要 "视情况" 决定是否允许隐式转换                          ║
// ║        经典模式: explicit(!is_convertible_v<U,T>)                           ║
// ║  WHEN: 类似 std::optional / std::variant 的包装器; 条件转换构造              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_explicit_bool() {
    lesson::print_header("Part 2: explicit(bool) 条件 explicit (C++20)");

    using namespace modern_syntax;

    // ── 2.1 SmartWrapper: 基于 is_convertible 的条件 explicit ──
    {
        lesson::print_subtitle("2.1  SmartWrapper: 同类型可隐式, 不同类型必须显式");

        // ★ 从 int 构造 SmartWrapper<int> → U=int, T=int → is_convertible_v<int,int>=true
        //   → explicit(false) → 允许隐式转换
        SmartWrapper<int> w1 = 42;              // OK: 隐式转换
        std::println("w1.get() = {}", w1.get());

        // 也可以显式构造
        SmartWrapper<int> w2(100);
        std::println("w2.get() = {}", w2.get());

        // ★ 从 double 构造 SmartWrapper<int> → U=double, T=int
        //   → is_convertible_v<double, int> = true (double可隐式转为int)
        //   → explicit(false) → 允许隐式转换
        SmartWrapper<int> w3 = 3.14;            // OK, 但窄化! 这就是条件 explicit 要小心的
        std::println("w3.get() = {} (double 隐式转为 int)", w3.get());

        // ★ 从 const char* 构造 SmartWrapper<string>
        //   → is_convertible_v<const char*, string> = true
        //   → 可以隐式转换
        SmartWrapper<std::string> ws = "你好 C++20!";  // OK: const char* → string 隐式
        std::println("ws.get() = '{}'", ws.get());
    }

    // ── 2.2 ValueHolder: 基于 is_same 的条件 explicit ──
    {
        lesson::print_subtitle("2.2  ValueHolder: 仅同类型可隐式, 其余必须显式");

        // 同类型 → explicit(false)
        ValueHolder<int> v1 = 100;
        std::println("v1.get() = {}", v1.get());

        // 不同类型 → explicit(true) → 必须显式构造
        // ValueHolder<int> v2 = 3.14;       // 编译错误! double 不是 int
        ValueHolder<int> v2(3.14);           // OK: 显式构造
        std::println("v2.get() = {}", v2.get());

        ValueHolder<std::string> vs("显式构造的字符串");
        std::println("vs.get() = '{}'", vs.get());

        lesson::print_note("is_same 比 is_convertible 更严格 —— 只有完全相同才允许隐式");
    }

    // ── 2.3 对比理解: explicit 语义 ──
    {
        lesson::print_subtitle("2.3  explicit 语义总结");

        std::println("explicit(true)   = 必须使用直接初始化语法: T obj(value);");
        std::println("explicit(false)  = 允许拷贝初始化语法:   T obj = value;");
        std::println("");
        std::println("经典模板模式:");
        std::println("  explicit(!std::is_convertible_v<U, T>)");
        std::println("  含义: 如果 U 能安全转为 T → 允许隐式; 否则禁止");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: std::hardware_destructive_interference_size —— 避免伪共享          ║
// ║  WHAT: 返回两个线程需独立写入的变量之间应保持的最小偏移量                     ║
// ║        (典型值: 64 字节, 即一条 cache line 的大小)                           ║
// ║  WHY:  两个独立变量在同一条 cache line 时, 一个线程的写入会把另一个线程      ║
// ║        缓存行里的副本弄脏, 迫使其重新从主存加载 —— 性能可差 10-100 倍        ║
// ║  WHEN: 多线程并发写不同字段; 高性能计数器/统计; 无锁数据结构                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_false_sharing() {
    lesson::print_header("Part 3: hardware_destructive_interference_size (C++17)");

    using namespace modern_syntax;

    // ── 3.1 理解问题: 未对齐的计数器 ──
    {
        lesson::print_subtitle("3.1  伪共享的本质");
        std::println("cache line 大小 (本平台推断): {} 字节",
                     std::hardware_destructive_interference_size);
        std::println("");

        std::println("MisalignedCounters (未对齐):");
        std::println("  sizeof       = {} 字节", sizeof(MisalignedCounters));
        std::println("  counter1 和 counter2 很可能在同一条 cache line");
        std::println("  → 线程A写 counter1, 线程B的 counter2 缓存失效!");
        std::println("  → 线程B必须重新从主存加载 → 性能暴跌");

        lesson::print_separator();

        std::println("AlignedCounters (已对齐):");
        std::println("  sizeof       = {} 字节", sizeof(AlignedCounters));
        std::println("  counter1 和 counter2 各自独占一条 cache line");
        std::println("  → 线程A写 counter1, 线程B的 counter2 完全不受影响!");
    }

    // ── 3.2 实际性能对比 ──
    {
        lesson::print_subtitle("3.2  性能对比: 伪共享 vs 对齐");

        constexpr int ITERATIONS = 100'000'000;
        constexpr int THREADS    = 2;

        // ── 测试1: 未对齐版本 (伪共享严重) ──
        {
            MisalignedCounters counters{};

            auto start = std::chrono::steady_clock::now();
            {
                std::vector<std::jthread> threads;
                threads.reserve(THREADS);
                for (int t = 0; t < THREADS; ++t) {
                    threads.emplace_back([&counters, tid = t]() {
                        for (int i = 0; i < ITERATIONS; ++i) {
                            if (tid == 0)
                                counters.counter1.fetch_add(1, std::memory_order_relaxed);
                            else
                                counters.counter2.fetch_add(1, std::memory_order_relaxed);
                        }
                    });
                }
            }
            auto end = std::chrono::steady_clock::now();

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::println("未对齐版本: {} ms  (伪共享严重)", ms);
            std::println("  counter1 = {}  counter2 = {}",
                         counters.counter1.load(), counters.counter2.load());
        }

        // ── 测试2: 对齐版本 (无伪共享) ──
        {
            AlignedCounters counters{};

            auto start = std::chrono::steady_clock::now();
            {
                std::vector<std::jthread> threads;
                threads.reserve(THREADS);
                for (int t = 0; t < THREADS; ++t) {
                    threads.emplace_back([&counters, tid = t]() {
                        for (int i = 0; i < ITERATIONS; ++i) {
                            if (tid == 0)
                                counters.counter1.fetch_add(1, std::memory_order_relaxed);
                            else
                                counters.counter2.fetch_add(1, std::memory_order_relaxed);
                        }
                    });
                }
            }
            auto end = std::chrono::steady_clock::now();

            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::println("对齐版本:   {} ms  (避免伪共享)", ms);
            std::println("  counter1 = {}  counter2 = {}",
                         counters.counter1.load(), counters.counter2.load());
        }

        lesson::print_note("对齐版本通常比未对齐版本快 2-10 倍, 差距随核心数增大而扩大");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    using namespace modern_syntax;

    // ── 陷阱1: 两个同类型的空成员不能共享地址 ──
    {
        lesson::print_subtitle("陷阱1: [[no_unique_address]] 对同类型空成员无效");
        // 如果 class 有两个相同空类型的 [[no_unique_address]] 成员,
        // 编译器必须给它们不同的地址 (C++ 标准要求)
        // → 它们无法共享地址, 每个都至少占 1 字节
        lesson::print_warn("同类型的两个 [[no_unique_address]] 成员依然各占 1 字节");
        lesson::print_note("解决方案: 让策略类继承自不同的基类, 或使用 EBO 基类方案");
    }

    // ── 陷阱2: explicit(bool) 不能替代运行时的条件判断 ──
    {
        lesson::print_subtitle("陷阱2: explicit(bool) 必须是编译期表达式");
        // explicit(some_runtime_bool) → 编译错误!
        // explicit 的条件必须是类型 traits、constexpr 变量或编译期常量
        lesson::print_warn("explicit(bool) 不接受运行时值, 只能用编译期常量表达式");
    }

    // ── 陷阱3: hardware_destructive_interference_size 可能较大 ──
    {
        lesson::print_subtitle("陷阱3: destructive_interference 的保守值可能浪费内存");
        std::println("本平台 hardware_destructive_interference_size = {}",
                     std::hardware_destructive_interference_size);
        std::println("sizeof(AlignedCounters) = {} (可能是 128 字节, 因为两个成员各对齐到 64)",
                     sizeof(AlignedCounters));
        lesson::print_warn("仅在 profiling 确认有伪共享时使用此优化, 否则浪费缓存");
    }

    // ── 陷阱4: [[no_unique_address]] 和 MSVC ABI ──
    {
        lesson::print_subtitle("陷阱4: MSVC 兼容性");
        lesson::print_note("GCC 11+ / Clang 13+ / MSVC 19.28+ (VS 2019 16.9+) 完全支持");
        lesson::print_note("较旧的 MSVC 版本可能需要 [[msvc::no_unique_address]]");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    using namespace modern_syntax;

    // ── 练习1: 创建带有多策略类的管理器 ──
    lesson::print_subtitle("练习1: 多策略 + [[no_unique_address]] 管理器");
    // 提示:
    //   struct CompPolicy { bool compare(int a, int b) const { return a < b; } };
    //   struct AllocPolicy { void* allocate(size_t n) { return ::operator new(n); } };
    //   template<typename C, typename A>
    //   class Manager {
    //       [[no_unique_address]] C comp;
    //       [[no_unique_address]] A alloc; // 不同类, 都能享受优化
    //       int data{};
    //   };
    // 思考: sizeof(Manager<CompPolicy, AllocPolicy>) 应该 ≈ sizeof(int)

    // ── 练习2: 为 SmartWrapper 编写条件 explicit ──
    lesson::print_subtitle("练习2: 实现自己的条件 explicit 包装器");
    // 提示: 用 explicit(!std::is_same_v<std::decay_t<U>, T>) 实现
    //       仅允许相同类型隐式构造, 其他类型必须显式

    // ── 练习3: 设计无伪共享的线程统计 ──
    lesson::print_subtitle("练习3: 设计无伪共享的多线程统计器");
    // 提示: 使用 alignas(hardware_destructive_interference_size) 对齐每个计数器
    //       不同线程写入不同计数器, 验证性能差异
}

// ── 打印大小信息 ──
void show_sizes() {
    lesson::print_header("类型大小一览");
    modern_syntax::print_size_info();
}

int main() {
    part1_no_unique_address();
    part2_explicit_bool();
    part3_false_sharing();

    show_sizes();
    pitfalls();
    exercises();
    return 0;
}
