#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stream_utils.hpp — 流操作辅助工具                                           ║
// ║  提供流状态重置、安全读取等常用模式，避免在学习代码中重复编写相同的样板代码    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <limits>
#include <string>
#include <print>

namespace streams {

// ── 重置输入流状态 ────────────────────────────────────────────────────────────
// WHY: 当流因类型不匹配等原因进入 fail 状态后，必须先 clear() 清除状态位，
//      再用 ignore() 丢弃残留的错误数据，否则后续所有操作都无效
// WHEN: 任何检测到 fail() 返回 true 之后、重新尝试读取之前
// 示例:
//   int n; std::cin >> n;
//   if (std::cin.fail()) {
//       reset_stream(std::cin);      // 清除 failbit 并丢弃残留输入
//       std::println("输入无效，请重新输入数字");
//   }
inline void reset_stream(std::istream& is) {
    is.clear();                                                       // 清除所有状态标志位
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');      // 丢弃当前行剩余内容
}

// ── 跳过输入流中的空白字符 ────────────────────────────────────────────────────
// WHY: 使用 >> 读取数值后，换行符残留在缓冲区中，导致后续 getline() 读到空行
// WHEN: 在 >> 操作之后、getline() 之前
inline void skip_whitespace(std::istream& is) {
    is >> std::ws;  // std::ws 是一个操纵符, 消耗所有前导空白字符
}

// ── 安全读取一行 ──────────────────────────────────────────────────────────────
// WHY: 封装 getline + 空行检查, 减少样板代码
// 返回 false 表示遇到 EOF 或读取失败
inline bool safe_getline(std::istream& is, std::string& line) {
    return static_cast<bool>(std::getline(is, line));
}

} // namespace streams
