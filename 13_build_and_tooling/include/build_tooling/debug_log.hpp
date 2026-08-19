// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  build_tooling/debug_log.hpp — 分级日志宏 (header-only)                     ║
// ║  主题: LOG_LEVEL 分级 + NDEBUG 条件编译                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
#pragma once

#include <format>
#include <print>
#include <string_view>

// ── 日志级别 ──────────────────────────────────────────────────────────────────
// 可用 -DLOG_LEVEL=N 覆盖:
//   0 = 只保留 ERROR   1 = +WARN   2 = +INFO   3 = +DEBUG (默认, 全部输出)
#ifndef LOG_LEVEL
#define LOG_LEVEL 3
#endif

namespace build_tooling {

// 统一输出一条带级别前缀的日志 (内部辅助, 一般不直接调用)
inline void log_line(std::string_view level, std::string_view msg) {
    std::println("[{}] {}", level, msg);
}

} // namespace build_tooling

// ── 分级日志宏 ────────────────────────────────────────────────────────────────
// 用 std::format 先格式化再加前缀 — 参数可以是任意可格式化组合
// 注意: 宏在预处理期展开, 分级过滤在编译期完成 (低级别直接展开为空)

#define LOG_ERROR(...) ::build_tooling::log_line("ERROR", std::format(__VA_ARGS__))

#if LOG_LEVEL >= 1
#define LOG_WARN(...) ::build_tooling::log_line("WARN", std::format(__VA_ARGS__))
#else
#define LOG_WARN(...) ((void)0)
#endif

#if LOG_LEVEL >= 2
#define LOG_INFO(...) ::build_tooling::log_line("INFO", std::format(__VA_ARGS__))
#else
#define LOG_INFO(...) ((void)0)
#endif

// DEBUG 日志: 只在 Debug 模式 (未定义 NDEBUG) 时编译进程序 — 与 assert 一致
#if !defined(NDEBUG) && LOG_LEVEL >= 3
#define DEBUG_LOG(...) ::build_tooling::log_line("DEBUG", std::format(__VA_ARGS__))
#else
#define DEBUG_LOG(...) ((void)0)
#endif
