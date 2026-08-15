#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  lesson_utils.hpp — 学习辅助工具                                            ║
// ║  提供打印分隔线、标题、计时等功能，让学习代码聚焦在知识点上                  ║
// ║                                                                            ║
// ║  Windows 编码兼容: 调用 print_header() 时自动把控制台切到 UTF-8，          ║
// ║  否则中文输出在默认 GBK 代码页的终端里会乱码。                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <functional>

#if defined(_WIN32)
// Windows API: 设置控制台输入/输出代码页为 UTF-8 (65001)
// SetConsoleOutputCP — 输出代码页 (程序 → 终端)
// SetConsoleCP      — 输入代码页 (终端 → 程序)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace lesson {

// ── Windows 控制台 UTF-8 初始化 ─────────────────────────────────────────────
// 源文件是 UTF-8 编译 (/utf-8)，字符串在内存里是 UTF-8 字节。
// 但 Windows 终端默认用 GBK (代码页 936) 解读 std::cout 的输出 → 乱码。
// (std::print 不受影响 — MSVC 的实现会自动转宽字符走 WriteConsoleW)
//
// 这里用 C++17 inline 变量 + lambda: 只要任何 .cpp include 了这个头文件，
// 就会在 main() 之前自动把控制台切到 UTF-8 (代码页 65001)。
// inline 保证整个程序只有一份实例，不会重复执行。
#if defined(_WIN32)
inline const bool _console_utf8_setup = [] {
    SetConsoleOutputCP(65001);  // 输出切 UTF-8 (程序 → 终端)
    SetConsoleCP(65001);        // 输入切 UTF-8 (终端 → 程序, cin 读中文不乱码)
    return true;
}();
#endif

// 显式初始化 (一般不需要调用 — include 本头文件即自动完成)
inline void console_utf8_init() {
#if defined(_WIN32)
    (void)_console_utf8_setup;  // 触发 inline 变量初始化 (如果还没发生)
#endif
}


// ── 打印分隔线 ──────────────────────────────────────────────────────────────
inline void print_separator(std::string_view title = "") {
    std::cout << "\n";
    if (!title.empty()) {
        std::cout << "--- " << title << " ---\n";
    } else {
        std::cout << "----------------------------------------\n";
    }
}

// ── 打印带框标题 ────────────────────────────────────────────────────────────
// 每个示例程序的第一个调用 — 顺便完成控制台 UTF-8 初始化 (Windows)
inline void print_header(std::string_view title) {
    console_utf8_init();  // Windows: 切换控制台到 UTF-8，防止中文乱码
    std::cout << "\n"
              << "╔══════════════════════════════════════════╗\n"
              << "║  " << title << "\n"
              << "╚══════════════════════════════════════════╝\n\n";
}

// ── 打印子标题 ──────────────────────────────────────────────────────────────
inline void print_subtitle(std::string_view subtitle) {
    std::cout << "\n── " << subtitle << " ──\n\n";
}

// ── 打印提示 ────────────────────────────────────────────────────────────────
inline void print_note(std::string_view msg) {
    std::cout << "💡 " << msg << "\n";
}

// ── 打印警告 ────────────────────────────────────────────────────────────────
inline void print_warn(std::string_view msg) {
    std::cout << "⚠️  " << msg << "\n";
}

// ── 计时执行 ────────────────────────────────────────────────────────────────
// 用法: lesson::time_it("排序耗时", []{ std::sort(v.begin(), v.end()); });
inline void time_it(std::string_view label, std::function<void()> fn) {
    auto start = std::chrono::steady_clock::now();
    fn();
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "⏱ " << label << ": " << ms << " ms\n";
}

} // namespace lesson
