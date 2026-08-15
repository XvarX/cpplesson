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
// 但 Windows 终端默认用 GBK (代码页 936) 解读输出 → 乱码。
// 这里在第一次打印前把控制台切到 UTF-8 (代码页 65001)。
// inline + 函数内静态变量: 整个进程只执行一次，之后的所有打印都正常。
inline void console_utf8_init() {
#if defined(_WIN32)
    static bool done = [] {
        SetConsoleOutputCP(65001);  // 输出切 UTF-8
        SetConsoleCP(65001);        // 输入也切 UTF-8 (cin 读中文不乱码)
        return true;
    }();
    (void)done;  // 消除未使用警告; static 保证只初始化一次
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
