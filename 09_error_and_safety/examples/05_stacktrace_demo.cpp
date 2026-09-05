// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 09_error_and_safety — 错误处理与安全                                 ║
// ║  主题: 05_stacktrace — std::stacktrace (C++23)                             ║
// ║  目标: 掌握调用栈捕获、basic_stacktrace/stacktrace_entry API、             ║
// ║        与异常和日志系统的集成                                               ║
// ║                                                                             ║
// ║  跟着敲: 理解如何在生产环境中用标准栈追踪替代平台相关的调试方案              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <stacktrace>   // C++23: std::stacktrace, std::stacktrace_entry
#include <stdexcept>
#include <type_traits> // std::is_integral_v
#include <iostream>
#include "error_safety/stacktrace.hpp"
#include "shared/lesson_utils.hpp"

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  辅助函数: 模拟深层调用链，用于展示栈追踪的效果                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

namespace {

// 模拟: 一个"业务逻辑"调用链
// alpha → beta → gamma → delta → epsilon
// 在 epsilon 中捕获栈追踪，可以清晰看到完整的调用路径

void epsilon() {
    // 在调用链最深处捕获栈追踪
    auto trace = error_safety::captureCurrent();
    std::println("  [epsilon] 在调用链深处捕获的栈追踪:");
    std::print("{}", error_safety::stacktraceToString(trace));
}

void delta()   { epsilon(); }
void gamma()   { delta(); }
void beta()    { gamma(); }
void alpha()   { beta(); }

} // anonymous namespace

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::stacktrace 基础 — 捕获与遍历                                ║
// ║                                                                             ║
// ║  std::stacktrace::current() 捕获当前执行点的调用栈快照。                   ║
// ║  栈是"值语义"的容器，可以安全地复制、存储、传递给其他函数。                ║
// ║  遍历栈帧: 索引 0 是最顶层 (离捕获点最近的帧)，依次向栈底方向排列。       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part1_stacktrace_basics() {
    lesson::print_separator("Part 1: std::stacktrace 基础");

    // ── 最简单的捕获 ──
    // current() 返回当前调用栈的快照
    auto trace = std::stacktrace::current();
    std::println("  栈帧数量: {}", trace.size());
    std::println("  调用栈内容:");
    std::print("{}", error_safety::stacktraceToString(trace));

    // ── 跳过顶层帧 ──
    // current(0) 保留所有帧 (包括 current() 自身的调用帧)
    // current(1) 跳过自身 (最常用)
    // current(2) 跳过两层，以此类推
    std::println("  使用 skip=1 跳过自身:");
    auto trace2 = error_safety::captureCurrent();  // 默认 skip=1
    std::print("{}", error_safety::stacktraceToString(trace2));
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: stacktrace_entry 详情 — 文件、行号、函数名                       ║
// ║                                                                             ║
// ║  每个 stacktrace_entry 代表一个栈帧，提供:                                  ║
// ║  - description()  函数名/符号描述 (平台相关的字符串)                       ║
// ║  - source_file()  源文件路径                                               ║
// ║  - source_line()  行号 (无调试信息时为 0)                                  ║
// ║  - native_handle() 平台相关的原生句柄                                      ║
// ║  - operator bool() 帧是否有效 (非空)                                       ║
// ║                                                                             ║
// ║  formatEntry() 将这些信息组合成统一的可读格式。                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part2_entry_details() {
    lesson::print_separator("Part 2: stacktrace_entry 详情");

    auto trace = std::stacktrace::current();

    std::println("  逐帧查看 (最多打印前 5 帧):");
    std::size_t count = std::min<std::size_t>(trace.size(), 5);
    for (std::size_t i = 0; i < count; ++i) {
        const auto& entry = trace[i];
        std::println("    ── 帧 #{} ──", i);
        std::println("      描述     : {}", entry.description());
        std::println("      源文件   : {}",
                     entry.source_file().empty() ? "(无调试信息)" : entry.source_file());
        std::println("      行号     : {}",
                     entry.source_line() == 0 ? "(不可用)" : std::to_string(entry.source_line()));
        std::println("      有效帧   : {}", entry ? "是" : "否");

        // native_handle() 的类型因平台而异 — libstdc++(MinGW) 返回整数地址,
        // MSVC 返回指针; 用 if constexpr 分支处理, 统一以可打印形式输出
        auto handle = entry.native_handle();
        if constexpr (std::is_integral_v<decltype(handle)>) {
            std::println("      原生句柄 : {:#x}", handle);
        } else {
            std::println("      原生句柄 : {:p}", handle);
        }
    }

    // ── 使用封装好的 formatEntry ──
    std::println("\n  使用 formatEntry() 格式化输出:");
    for (std::size_t i = 0; i < count; ++i) {
        std::println("{}", error_safety::formatEntry(trace[i], i));
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 与异常集成 — 在异常中嵌入调用栈                                  ║
// ║                                                                             ║
// ║  核心洞察:                                                                  ║
// ║  - 在 throw 点捕获栈追踪 → 得到异常的"来源"调用栈                          ║
// ║  - 在 catch 点捕获栈追踪 → 得到异常的"捕获"调用栈 (通常不是你想要的)      ║
// ║                                                                             ║
// ║  throwWithTrace<E> 模板在 throw 点自动捕获栈追踪并嵌入异常消息。           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part3_exception_integration() {
    lesson::print_separator("Part 3: 与异常集成");

    // ── 示例1: 使用 throwWithTrace 抛出带栈追踪的异常 ──
    std::println("  [示例] throwWithTrace<runtime_error> 抛出异常:");
    try {
        error_safety::throwWithTrace<std::runtime_error>("模拟的运行时错误");
    } catch (const std::exception& e) {
        // what() 返回的消息中已包含调用栈信息
        std::println("  捕获到异常:");
        std::println("{}", e.what());
    }

    // ── 示例2: 对比 — 在 catch 块中捕获栈追踪 ──
    // 这反映的是 catch 块的位置，不是异常的来源位置
    std::println("\n  [对比] 在 catch 块中捕获栈追踪 (反映 catch 位置):");
    try {
        throw std::runtime_error("普通异常 (无栈追踪)");
    } catch (const std::exception& e) {
        std::println("  异常消息: {}", e.what());
        std::println("  catch 块位置的栈追踪:");
        auto catchTrace = error_safety::captureCurrent();
        std::print("{}", error_safety::stacktraceToString(catchTrace));
        std::println("  (注意: 这是 catch 块的位置，不是 throw 的位置!)");
    }

    // ── 示例3: 深层调用中异常 + 栈追踪 ──
    std::println("\n  [示例] 深层调用链中的异常:");
    try {
        alpha();  // alpha → beta → gamma → delta → epsilon
        // epsilon 中只打印了栈追踪，没有抛出异常
        // 这里在 alpha 层级捕获一个异常来看看栈追踪
        error_safety::throwWithTrace<std::logic_error>(
            "在 alpha() 中检测到的逻辑错误");
    } catch (const std::exception& e) {
        std::println("  捕捉到: {}", e.what());
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 与日志集成 — logExceptionWithTrace                                ║
// ║                                                                             ║
// ║  在实际项目中，异常发生时应记录:                                           ║
// ║  1. 异常类型和消息                                                          ║
// ║  2. 发生时的调用栈 (堆栈快照)                                              ║
// ║  3. 上下文信息 (当前操作、用户、线程等)                                    ║
// ║                                                                             ║
// ║  logExceptionWithTrace() 演示了这种集成模式。                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part4_logging_integration() {
    lesson::print_separator("Part 4: 与日志集成");

    // ── 模拟一个会失败的操作 ──
    auto riskyOperation = [](int input) {
        if (input < 0) {
            // 捕获当前栈追踪并在异常消息中附带
            auto trace = error_safety::captureCurrent();
            error_safety::logExceptionWithTrace(
                std::format("riskyOperation 收到非法输入: {}", input),
                trace);
            throw std::invalid_argument(
                std::format("输入必须为非负数, 得到 {}", input));
        }
        return input * 2;
    };

    std::println("  成功调用: riskyOperation(21) = {}", riskyOperation(21));

    std::println("\n  失败调用 (将触发异常报告):");
    try {
        riskyOperation(-1);
    } catch (const std::exception& e) {
        std::println("  最终捕捉: {}", e.what());
    }

    // ── 完整的异常处理流程 ──
    std::println("\n  [完整流程] 捕获 → 记录栈追踪 → 重新抛出或恢复:");
    try {
        auto trace = error_safety::captureCurrent();
        // 模拟: 数据库操作失败
        error_safety::logExceptionWithTrace("数据库连接超时", trace);
        // 实际项目中: 可能在这里尝试重连或使用缓存数据
        std::println("  (日志已记录，尝试恢复...)");
    } catch (...) {
        std::println("  恢复失败，向上层报告");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ║                                                                             ║
// ║  1. 在性能敏感路径上频繁捕获栈追踪                                         ║
// ║     → stacktrace::current() 涉及栈展开和符号解析，每次调用可能有数十微秒  ║
// ║       甚至毫秒级的开销                                                     ║
// ║     正确方式: 仅在异常/错误路径捕获，或使用采样方式                       ║
// ║                                                                             ║
// ║  2. 混淆 throw 点的栈与 catch 点的栈                                       ║
// ║     → 在 catch 块中调用 stacktrace::current() 得到的是 catch 块的位置     ║
// ║       而非异常抛出位置                                                     ║
// ║     正确方式: 在 throw 前捕获 (如 throwWithTrace 模板)，或在异常对象中    ║
// ║       构造时嵌入栈追踪                                                     ║
// ║                                                                             ║
// ║  3. 依赖 description() 的精确字符串格式                                    ║
// ║     → description() 的格式因编译器/平台而异 (GCC vs MSVC vs Clang)        ║
// ║     正确方式: 仅用于人类阅读，不要做自动化的文本解析                       ║
// ║                                                                             ║
// ║  4. 假设 source_file() 和 source_line() 总是可用                            ║
// ║     → 在没有调试信息的构建中这些字段可能为空/为零                          ║
// ║     → 优化的 Release 构建中函数可能被内联，帧被合并或丢失                  ║
// ║     正确方式: 始终检查返回值，条件性地展示可用信息                         ║
// ║                                                                             ║
// ║  5. 在析构函数中抛出带栈追踪的异常                                          ║
// ║     → 析构函数默认 noexcept，抛出异常会触发 std::terminate                ║
// ║     正确方式: 析构函数中仅记录错误，不抛出                                 ║
// ║                                                                             ║
// ║  6. 忘记 #include <stacktrace> 只包含本头文件                              ║
// ║     → 本头文件已包含 <stacktrace>，无需手动重复包含                       ║
// ║     → 但如果你的代码直接使用 std::stacktrace 类型，确保编译器支持 C++23  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                        ║
// ║                                                                             ║
// ║  1. 实现一个 ExceptionWithStacktrace 类，继承自 std::exception:             ║
// ║     - 构造函数中自动捕获当前调用栈                                          ║
// ║     - 重写 what() 返回包含栈追踪信息的字符串                               ║
// ║     - 在 catch 块中测试: 打印异常的 what()                                 ║
// ║     提示: 在构造函数中调用 std::stacktrace::current()                      ║
// ║                                                                             ║
// ║  2. 实现一个 ScopedStacktraceLogger RAII 类:                                ║
// ║     - 构造函数接收一个"操作名称"参数，记录进入时间                         ║
// ║     - 析构函数输出操作名称、耗时毫秒数、当前调用栈                         ║
// ║     - 仅在 Debug 模式下 (使用 #ifndef NDEBUG 或 constexpr 条件编译)        ║
// ║       启用栈追踪捕获，Release 模式下仅输出耗时                             ║
// ║                                                                             ║
// ║  3. 实现一个 LogSink 类，支持以下功能:                                      ║
// ║     - void log(Level level, string_view msg) 方法                          ║
// ║     - 当 Level >= ERROR 时自动附加当前调用栈                                ║
// ║     - 经过合理设计的格式化输出                                              ║
// ║                                                                             ║
// ║  4. 编写一个简单的错误报告生成器:                                           ║
// ║     - 接收异常类型名称、异常消息、调用栈字符串                             ║
// ║     - 生成结构化的 JSON 错误报告                                            ║
// ║     - 包含时间戳、线程 ID、异常详情、栈追踪                                ║
// ║     提示: 使用 std::chrono 和 std::this_thread::get_id()                  ║
// ║                                                                             ║
// ║  5. 对比实验: 写两个版本的"深度嵌套调用"函数:                              ║
// ║     - 版本 A: 在每一层捕获 stacktrace (高开销)                              ║
// ║     - 版本 B: 仅在最外层捕获一次 stacktrace (低开销)                        ║
// ║     用 std::chrono 测量两者耗时差异，理解捕获开销                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

int main() {
    lesson::print_header("05_stacktrace — std::stacktrace (C++23)");

    part1_stacktrace_basics();
    part2_entry_details();
    part3_exception_integration();
    part4_logging_integration();

    std::println("\n✅ std::stacktrace 学习完成!");
    std::println("   提示: 如果输出中缺少文件名/行号，请确保编译时启用了调试符号。");
    std::println("   使用 -g (GCC/Clang) 或 /Zi (MSVC) 获取完整的栈追踪信息。");
    return 0;
}
