#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  lesson_utils.hpp — 学习辅助工具                                            ║
// ║  提供打印分隔线、标题、计时等功能，让学习代码聚焦在知识点上                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <functional>

namespace lesson {

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
inline void print_header(std::string_view title) {
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
