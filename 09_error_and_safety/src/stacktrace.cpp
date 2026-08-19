// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  实现: error_safety/stacktrace.hpp                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "error_safety/stacktrace.hpp"
#include <format>   // std::format (C++20)
#include <print>    // std::println (C++23)
#include <sstream>  // std::ostringstream (用于高效的字符串拼接)

namespace error_safety {

// ── captureCurrent ───────────────────────────────────────────────────────────────

std::stacktrace captureCurrent(std::size_t skip) {
    // std::stacktrace::current(skip) 跳过最顶层的 skip 帧
    // skip=0 返回完整的栈 (包括 current() 自身的调用帧)
    // skip=1 跳过 current() 自身这一帧 (即 captureCurrent 自身的调用者)
    //
    // 可选的第二个参数 max_depth 可以限制捕获深度，避免过长的栈追踪
    // captureCurrent(1, 32) — 跳过 1 帧，最多捕获 32 帧
    return std::stacktrace::current(skip);
}

// ── formatEntry ──────────────────────────────────────────────────────────────────

std::string formatEntry(const std::stacktrace_entry& entry, std::size_t index) {
    // entry 可能为"空" (例如 current() 返回的栈深度小于预期时)
    // 空的 entry 的 operator bool() 返回 false
    if (!entry) {
        return std::format("  #{:<3} <空帧>", index);
    }

    // description() 返回函数签名或其他描述信息 (平台相关)
    // 如果不可用则返回空字符串
    std::string desc = entry.description();
    if (desc.empty()) {
        // 回退方案: 使用 native_handle() 的地址值作为标识
        // 注意: native_handle() 的类型是实现定义的 —
        //   libstdc++(MinGW) 返回整数地址 (uintptr_t)，MSVC 返回指针
        auto handle = entry.native_handle();
        if constexpr (std::is_integral_v<decltype(handle)>) {
            desc = std::format("<未知符号 @ {:#x}>", handle);
        } else {
            desc = std::format("<未知符号 @ {:p}>", handle);
        }
    }

    // source_file() 和 source_line() 在调试符号可用时提供精确位置
    // 如果调试信息不可用，source_file() 返回空字符串，source_line() 返回 0
    std::string file = entry.source_file();
    std::uint_least32_t line = entry.source_line();

    if (file.empty() || line == 0) {
        // 无调试信息: 仅输出描述
        return std::format("  #{:<3} {}", index, desc);
    }

    // 完整信息: 描述 + 文件:行号
    return std::format("  #{:<3} {} at {}:{}", index, desc, file, line);
}

// ── stacktraceToString ───────────────────────────────────────────────────────────

std::string stacktraceToString(const std::stacktrace& trace, std::size_t skip) {
    // 空栈: 可能由无符号支持、捕获失败等原因导致
    if (trace.size() <= skip) {
        return "  (调用栈不可用 — 可能缺少调试符号或栈信息已被剥离)\n";
    }

    std::ostringstream oss;
    oss << std::format("  调用栈深度: {} 帧\n", trace.size() - skip);

    // 遍历栈帧: 索引 0 是最顶层 (调用点最近的帧)
    // 从 skip 开始，跳过不需要展示的前几帧
    for (std::size_t i = skip; i < trace.size(); ++i) {
        oss << formatEntry(trace[i], i - skip) << '\n';
    }

    return oss.str();
}

// ── logExceptionWithTrace ────────────────────────────────────────────────────────

void logExceptionWithTrace(std::string_view context,
                            const std::stacktrace& trace) {
    // 输出格式化的异常报告，包含上下文描述和调用栈
    // 实际项目中可以用 std::format 写入日志文件或发送到远程监控
    std::println("┌─ 异常报告 ──────────────────────────────────────────────");
    std::println("│ 上下文 : {}", context);
    std::println("│ 调用栈 :");
    // 逐行输出栈追踪，用 │ 前缀保持报告格式一致
    // stacktraceToString 返回的字符串已经包含缩进和换行，直接拼接即可
    std::print("{}", stacktraceToString(trace));
    std::println("└──────────────────────────────────────────────────────────");
}

} // namespace error_safety
