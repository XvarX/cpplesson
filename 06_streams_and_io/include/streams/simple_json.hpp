#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  simple_json.hpp — 简易 JSON 构建与 CSV 解析工具                            ║
// ║  用 ostringstream 演示结构化文本的拼接; 用 istringstream 演示解析            ║
// ║  注意: 仅用于学习 stringstream 的用法, 实际项目请用专业 JSON/CSV 库          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <string>
#include <vector>
#include <print>

namespace streams {

// ── Entry: 数据条目 ──────────────────────────────────────────────────────────
struct Entry {
    std::string name;
    int id = 0;
    double score = 0.0;
};

// ── 声明 (实现在 src/simple_json.cpp) ────────────────────────────────────────

/// 将单个 Entry 转换为 JSON 对象字符串 (如 {"name":"Ecks", "id":1001, "score":99.9})
std::string entry_to_json(const Entry& e);

/// 将 Entry 列表构建为 JSON 数组字符串
std::string build_json_array(const std::vector<Entry>& entries);

/// 解析 CSV 行 (逗号分隔), 返回字段列表
/// 注意: 此实现不处理引号包裹的字段, 仅用于演示 istringstream 基本用法
std::vector<std::string> parse_csv_line(const std::string& line);

} // namespace streams
