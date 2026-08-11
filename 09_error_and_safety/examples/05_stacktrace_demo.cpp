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
    std::size_t count = std::min(trace.size(), static_cast<std::size_t>(5));
    for (std::size_t i = 0; i < count; ++i) {
        const auto& entry = trace[i];
        std::println("    ── 帧 #{} ──", i);
        std::println("      描述     : {}", entry.description());
        std::println("      源文件   : {}",
                     entry.source_file().empty() ? "(无调试信息)" : entry.source_file());
        std::println("      行号     : {}",
                     entry.source_line() == 0 ? "(不可用)" : std::to_string(entry.source_line()));
        std::println("      有效帧   : {}", entry ? "是" : "否");

        // native_handle() 的类型因平台而异 (void* / DWORD64 等)
        // 这里用 static_cast 转为 void* 以便统一打印
        auto handle = static_cast<const void*>(entry.native_handle());
        std::println("      原生句柄 : {:p}", handle);
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
// ║  常见陷阱回顾                                                                ║
// ║                                                                             ║
// ║  1. 不要在热点路径上捕获栈追踪 — 捕获有开销 (栈展开 + 符号解析)           ║
// ║  2. throw 点的栈 ≠ catch 点的栈 — 确保在 throw 前捕获                      ║
// ║  3. description() 格式因平台而异 — 不可用于自动化文本分析                  ║
// ║  4. source_file/line 在无调试符号时为空/0 — 始终检查                       ║
// ║  5. 析构函数中不要抛带栈追踪的异常 — 析构函数默认 noexcept                ║
// ║  6. 确保编译器支持 <stacktrace> (GCC 12+, Clang 17+, MSVC 19.34+)          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习提示                                                                    ║
// ║                                                                             ║
// ║  1. ExceptionWithStacktrace: 继承 std::exception, 在构造函数中捕获栈追踪   ║
// ║  2. ScopedStacktraceLogger: RAII 类, 构造记录进入, 析构输出耗时+栈        ║
// ║  3. LogSink: ERROR 级别自动附加调用栈                                       ║
// ║  4. JSON 错误报告: 结构化输出时间戳/线程ID/异常/栈追踪                     ║
// ║  5. 性能对比: 逐层捕获 vs 仅外层捕获 — 测量耗时差异                       ║
// ║                                                                             ║
// ║  详细说明参见 include/error_safety/stacktrace.hpp                          ║
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
