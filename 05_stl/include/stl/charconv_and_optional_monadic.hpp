#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/charconv_and_optional_monadic.hpp — 高性能转换与 optional 单子操作      ║
// ║  主题: to_chars/from_chars (高性能字符串转换)、optional 单子操作 (C++23)      ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include <charconv>
#include <cmath>
#include <expected>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace stl_learn {

// ── Part 1: std::to_chars — 高性能数值到字符串转换 ────────────────────────────
// 是什么: C++17 引入的无分配、无 locale、不抛异常的数值转字符串函数
// 为什么: 比 snprintf/stringstream/to_string 快 5-10 倍; 不依赖 locale; 无堆分配
// 什么时候用: 日志系统; 序列化/反序列化; 高频交易; 任何需要高性能数值转字符串的场景
void part1_to_chars();

// ── Part 2: std::from_chars — 高性能字符串到数值转换 ───────────────────────────
// 是什么: C++17 引入的无分配、无 locale、不抛异常的字符串转数值函数
// 为什么: 比 atoi/strtod/scanf/stringstream 快数倍; 无全局状态; 精确错误报告
// 什么时候用: 解析配置文件; JSON/CSV 解析器; 网络协议解析; 命令行参数解析
void part2_from_chars();

// ── Part 3: std::optional 单子操作 (C++23) ─────────────────────────────────────
// 是什么: and_then(链式调用返回 optional 的函数) / or_else(为空时提供替代值) /
//         transform(映射值但不改变 optional 状态)
// 为什么: 优雅地处理"可能为空"的链式操作，避免嵌套 if-else; 函数式编程风格;
//         比手动判断 has_value() 更安全、更可读
// 什么时候用: 多步骤的管道式数据处理; 链路中任一步骤可能失败;
//             替代"箭头式"嵌套 if 判断; 配置查找链
void part3_optional_monadic();

// ── Part 4: chardconv + optional 综合实战 — 安全解析流水线 ─────────────────────
// 是什么: 将 from_chars 的解析结果封装为 optional，利用单子操作构建解析流水线
// 为什么: 真实场景中解析和业务逻辑紧密相连; 展现两个主题的协同价值
// 什么时候用: 构建健壮的数据解析管道; 输入验证 + 转换 + 业务逻辑组合
void part4_combined();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void charconv_and_optional_monadic_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void charconv_and_optional_monadic_exercises();

} // namespace stl_learn
