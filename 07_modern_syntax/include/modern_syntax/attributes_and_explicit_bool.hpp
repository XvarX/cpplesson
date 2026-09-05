#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  modern_syntax/attributes_and_explicit_bool.hpp                            ║
// ║  [[no_unique_address]](C++20) / explicit(bool)(C++20) /                   ║
// ║  std::hardware_destructive_interference_size(C++17) —— 可复用类型          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <cstddef>      // std::size_t
#include <new>          // std::hardware_destructive_interference_size
#include <type_traits>  // std::is_convertible_v, std::is_same_v
#include <atomic>       // std::atomic
#include <string>
#include <utility>      // std::forward, std::declval

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  一、[[no_unique_address]] (C++20) —— 空成员优化                          ║
// ║                                                                           ║
// ║  WHAT: 告诉编译器: 如果某个非静态成员是空类型(无成员变量), 它可以与       ║
// ║        其他成员共享地址, 不必占用独立存储空间                              ║
// ║  WHY:  ① 空类型成员(如策略类/比较器/分配器/删除器)占至少 1 字节           ║
// ║           (C++ 要求每个对象有唯一地址), 浪费空间                           ║
// ║        ② 模板代码常用 "空基类优化(EBO)" 来避免此浪费, 但写成成员比        ║
// ║           基类更直观, [[no_unique_address]] 让成员也能享受空优化           ║
// ║        ③ 策略模式 / tag dispatch / 无状态仿函数作为成员时, 零开销         ║
// ║  WHEN: 策略模式(policy-based design); 无状态分配器/比较器/删除器;          ║
// ║        tag 类型作为成员; 任何只有类型名没有数据的空类作为成员               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ── 一个典型的空策略类 ──
// 只有成员函数, 没有成员变量 → 空类 (大小为 1 字节)
struct LoggingPolicy {
    void log(const std::string& msg) const;
};

struct NoOpPolicy {
    void log(const std::string&) const {}  // 什么都不做, 但满足接口
};

// ── Process 类: 持有空策略对象作为成员 ──
// 传统做法: 作为基类(利用 EBO), 但作为成员更直观却浪费空间
// C++20: [[no_unique_address]] 让成员也能享受空优化
class Processor {
public:
    // 使用 [[no_unique_address]]: 即使 LoggingPolicy 是空类,
    // 该成员也可以与 int data_ 或其他成员共享地址
    // 不带属性: sizeof(Processor) = 8 (padding) 或更大
    // 带属性:   sizeof(Processor) = 4 (int) + 可能的 padding
    [[no_unique_address]] LoggingPolicy logger;
    int data_{};

    void process() const;
};

// ── 模板版本的处理器: 策略类作为模板参数, 空类型也是可能的 ──
// 这就是 [[no_unique_address]] 在模板代码中大放异彩的场景
template<typename LoggerPolicy>
class GenericProcessor {
public:
    [[no_unique_address]] LoggerPolicy logger;
    int value{};

    void run() const;
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  二、explicit(bool) (C++20) —— 条件 explicit                              ║
// ║                                                                           ║
// ║  WHAT: 将 explicit 关键字变成一个运行时…不, 是编译期布尔表达式!            ║
// ║        explicit(true) = explicit;  explicit(false) = 非 explicit          ║
// ║                                                                           ║
// ║  WHY:  ① 模板代码中, 有时候希望允许隐式转换, 有时候又不希望 ——           ║
// ║           取决于模板参数之间的关系                                         ║
// ║        ② 经典模式: explicit(!std::is_convertible_v<U, T>) ——               ║
// ║           当 U 可隐式转换为 T 时, 允许隐式构造; 否则禁止                   ║
// ║        ③ 包装器类 (如 std::optional, std::variant) 大量使用此特性          ║
// ║                                                                           ║
// ║  WHEN: 模板包装器; 条件转换构造函数; 类似 std::optional<T> 的设计;         ║
// ║        你想 "有时候" 允许隐式构造的模板类                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ── 简单包装器: 演示条件 explicit ──
// 规则: 如果 U 安全地可转换为 T (即 U 就是 T 本身), 允许隐式转换
//       否则, 必须显式构造以避免意外
template<typename T>
class SmartWrapper {
public:
    T value;

    // 默认构造
    SmartWrapper() = default;

    // ★ explicit(bool) —— C++20 的核心特性
    // explicit(false): 允许隐式转换 → SmartWrapper<int> w = 42;  // OK, 如果条件为真
    // explicit(true):  禁止隐式转换 → SmartWrapper<int> w = 42;   // 编译错误! 必须显式
    template<typename U>
    explicit(!std::is_convertible_v<U, T>)    // 条件: 只有当 U 不可安全转换为 T 时才是 explicit
    SmartWrapper(U&& u) : value(std::forward<U>(u)) {}

    T get() const { return value; }
};

// ── 另一种常见模式: 基于 is_same 的条件 explicit ──
// 允许从相同类型隐式构造, 禁止从其他类型隐式构造
template<typename T>
class ValueHolder {
public:
    T data;

    ValueHolder() = default;

    // 条件: 如果 U 不是 T, 则为 explicit (禁止隐式转换)
    // 如果 U 就是 T, 则为 explicit(false) (允许隐式转换)
    template<typename U>
    explicit(!std::is_same_v<std::decay_t<U>, T>)
    ValueHolder(U&& val) : data(std::forward<U>(val)) {}

    T get() const { return data; }
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  三、std::hardware_destructive_interference_size (C++17) —— 避免伪共享    ║
// ║                                                                           ║
// ║  WHAT: 编译器/硬件建议的 "两个对象之间最小间距", 以杜绝伪共享              ║
// ║        (伪共享: 两个独立变量在同一 cache line, 一个线程写会                ║
// ║         使另一个线程的缓存失效, 即使它们逻辑上无关)                         ║
// ║                                                                           ║
// ║  WHY:  ① 伪共享是高性能并发代码的"隐形杀手" —— 代码逻辑正确, 但           ║
// ║           性能比预期差 10-100 倍                                           ║
// ║        ② 不同 CPU 架构 cache line 大小不同 (通常 64 字节, 但               ║
// ║           可能是 128), 硬编码不跨平台                                      ║
// ║        ③ 配合 alignas 让各线程独占自己的 cache line                       ║
// ║                                                                           ║
// ║  WHEN: 多线程共享结构体(如计数器/统计); 高性能无锁数据结构;                  ║
// ║        MPMC 队列; 线程池统计; 任何 "多写者" 并发场景                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ── 演示伪共享问题的结构 ──
// 两个原子变量紧密相邻 → 位于同一条 cache line → 伪共享
// 线程 A 写 counter1, 线程 B 写 counter2
// 结果: A 的写入导致整个 cache line 失效, B 只能从主存重新读取
struct MisalignedCounters {
    std::atomic<long long> counter1{0};
    std::atomic<long long> counter2{0};  // 和 counter1 在同一缓存行 → 伪共享!
};

// ── 正确做法: 使用 alignas 将两个计数器放在不同缓存行 ──
// 每个线程独占一个 cache line, 互不干扰
struct AlignedCounters {
    alignas(std::hardware_destructive_interference_size)
        std::atomic<long long> counter1{0};
    alignas(std::hardware_destructive_interference_size)
        std::atomic<long long> counter2{0};
    // counter1 和 counter2 现在各自位于独立的 cache line
};

// ── 实际应用: 线程安全的简单统计结构 ──
// 多个线程各自写入不同字段, 必须避免伪共享
struct alignas(std::hardware_destructive_interference_size) ThreadStats {
    std::atomic<long long> ops_completed{0};
    std::atomic<long long> bytes_written{0};
    std::atomic<long long> errors{0};
    // 注: 这里三个原子变量仍在同一结构内, 但在线程数组中使用时
    //     每个 ThreadStats 整体对齐, 数组元素各自独占 cache line
};

// ── 工具函数: 打印各种类型的大小, 直观展示优化效果 ──
void print_size_info();

} // namespace modern_syntax
