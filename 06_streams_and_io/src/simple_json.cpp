// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  simple_json.cpp — 简易 JSON 构建与 CSV 解析实现                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "streams/simple_json.hpp"
#include <sstream>
#include <iomanip>

namespace streams {

std::string entry_to_json(const Entry& e) {
    std::ostringstream oss;
    // 使用 ostringstream 拼接 JSON 字段 — 比多次 operator+ 更高效 (减少临时对象)
    oss << "{\"name\":\"" << e.name
        << "\", \"id\":" << e.id
        << ", \"score\":" << std::fixed << std::setprecision(1) << e.score
        << "}";
    return oss.str();
}

std::string build_json_array(const std::vector<Entry>& entries) {
    std::ostringstream json;
    json << "[\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        json << "  " << entry_to_json(entries[i]);
        if (i + 1 < entries.size()) json << ",";
        json << "\n";
    }
    json << "]\n";
    return json.str();
}

std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> fields;
    std::istringstream iss(line);
    std::string field;

    // 用 getline 配合 ',' 分隔符逐字段提取
    // 注意: >> 会跳过空白, 而 getline 不会 — 选择哪个取决于数据格式
    while (std::getline(iss, field, ',')) {
        fields.push_back(field);
    }

    // 如果最后是空字段 (如 "a,b,"), 也要保留
    if (!line.empty() && line.back() == ',') {
        fields.push_back("");
    }

    return fields;
}

} // namespace streams
