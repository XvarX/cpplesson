// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  error_safety/stacktrace.hpp — std::stacktrace (C++23)                      ║
// ║  捕获调用栈、basic_stacktrace、stacktrace_entry、与异常/日志集成            ║
// ║  可复用的函数: captureCurrent, formatEntry, stacktraceToString,             ║
// ║               logExceptionWithTrace, throwWithTrace                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <stacktrace>    // C++23: std::stacktrace, std::stacktrace_entry
#include <string>
#include <string_view>
#include <exception>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  std::stacktrace 是什么？                                                   ║
// ║                                                                             ║
// ║  C++23 引入的 <stacktrace> 头文件提供了标准的调用栈捕获与遍历能力:          ║
// ║                                                                             ║
// ║  - std::stacktrace_entry        单个栈帧的信息 (文件、行号、函数名)         ║
// ║  - std::basic_stacktrace<A>     栈帧的容器 (可指定分配器)                   ║
// ║  - std::stacktrace              basic_stacktrace 的默认实例化               ║
// ║                                                                             ║
// ║  核心方法:                                                                  ║
// ║  - std::stacktrace::current()   捕获当前执行点的调用栈                     ║
// ║  - entry.source_file()         返回源文件名 (可能为空字符串)               ║
// ║  - entry.source_line()         返回行号 (可能为 0 如果不可用)              ║
// ║  - entry.description()         返回人类可读的描述 (函数签名等)             ║
// ║  - entry.native_handle()       返回平台相关的原生句柄                      ║
// ║                                                                             ║
// ║  为什么需要 std::stacktrace？                                               ║
// ║  1. 生产环境调试: 崩溃时记录调用栈，无需附加调试器                        ║
// ║  2. 异常增强: 在异常中附带调用栈信息，大幅提升排查效率                    ║
// ║  3. 性能分析: 了解热点代码的调用路径                                       ║
// ║  4. 日志增强: 关键日志自动附带调用上下文，便于追溯                        ║
// ║  5. 标准化: 替代平台相关方案 (backtrace/StackWalk)，一次编写到处运行     ║
// ║                                                                             ║
// ║  何时使用 std::stacktrace？                                                 ║
// ║  - 异常处理: 在 catch 块中捕获栈追踪，与异常消息一起记录                  ║
// ║  - 断言失败: 在 assert 替代宏/SIGABRT handler 中自动记录                  ║
// ║  - 错误日志: 在 ERROR 级别日志中自动附加栈上下文                          ║
// ║  - 调试构建: 在 Debug 模式下启用全栈捕获，Release 模式下有条件启用        ║
// ║  - 性能敏感路径: 仅在异常/错误场景捕获，正常路径零开销                    ║
// ║                                                                             ║
// ║  注意事项:                                                                  ║
// ║  - Stacktrace 的捕获有一定开销 (栈展开+符号解析)，应仅在需要时调用        ║
// ║  - 符号信息取决于编译选项 (-g, 调试符号) 和平台 (DWARF/PDB)              ║
// ║  - 优化构建中某些帧可能被内联消失，栈信息可能不完整                        ║
// ║  - description() 的格式是平台相关的，不要做严格的字符串比较                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

namespace error_safety {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  captureCurrent — 捕获当前调用栈                                           ║
// ║                                                                             ║
// ║  skip 参数: 跳过最顶层的 N 个栈帧 (默认跳过自身这一帧)                     ║
// ║  返回值: 调用栈的快照 (值语义，可安全传递和存储)                           ║
// ║                                                                             ║
// ║  典型用法:                                                                  ║
// ║    auto trace = captureCurrent();  // 在需要了解调用上下文的地方捕获       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
[[nodiscard]] std::stacktrace captureCurrent(std::size_t skip = 1);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  formatEntry — 将单个栈帧格式化为可读字符串                                ║
// ║                                                                             ║
// ║  格式: "  #N description at file:line"                                    ║
// ║  如果 source_file() 为空，则只输出 description                             ║
// ║  如果 description() 为空，则使用 native_handle() 的十六进制表示           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
[[nodiscard]] std::string formatEntry(const std::stacktrace_entry& entry,
                                       std::size_t index);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stacktraceToString — 将整个调用栈转换为多行字符串                         ║
// ║                                                                             ║
// ║  格式: 每行一个帧，从栈顶 (调用点) 到栈底 (main/线程入口)                 ║
// ║  以缩进和帧编号组织输出                                                     ║
// ║                                                                             ║
// ║  典型用法:                                                                  ║
// ║    auto trace = captureCurrent();                                           ║
// ║    std::println("调用栈:\n{}", stacktraceToString(trace));                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
[[nodiscard]] std::string stacktraceToString(const std::stacktrace& trace,
                                              std::size_t skip = 0);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  logExceptionWithTrace — 将异常消息与调用栈一起输出到日志                  ║
// ║                                                                             ║
// ║  这是一个示范函数，展示如何将栈追踪集成到异常处理流程中。                  ║
// ║  实际项目中，你可能会将其输出到日志文件或监控系统。                        ║
// ║                                                                             ║
// ║  参数:                                                                      ║
// ║    context  — 异常发生的上下文描述 ("解析配置文件失败" 等)                  ║
// ║    trace    — 在异常点捕获的调用栈                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void logExceptionWithTrace(std::string_view context,
                            const std::stacktrace& trace);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  throwWithTrace — 抛出异常并附带调用栈信息                                 ║
// ║                                                                             ║
// ║  这是一个模板函数 (在头文件中实现)，演示如何将栈追踪嵌入到异常消息中。    ║
// ║                                                                             ║
// ║  用法:                                                                      ║
// ║    throwWithTrace<std::runtime_error>("操作失败");                          ║
// ║                                                                             ║
// ║  注意: 栈追踪在 throw 点捕获，反映了异常的来源位置。                       ║
// ║        catch 块中的 stacktrace::current() 反映的是 catch 块的位置，        ║
// ║        而非异常的来源位置。                                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename ExceptionT>
[[noreturn]] void throwWithTrace(std::string_view message) {
    auto trace = std::stacktrace::current(1);
    auto fullMsg = std::string{message}
                 + "\n--- 调用栈 ---\n"
                 + stacktraceToString(trace);
    throw ExceptionT(fullMsg);
}

} // namespace error_safety

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
