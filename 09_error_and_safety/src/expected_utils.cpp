// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  实现: error_safety/expected_utils.hpp                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "error_safety/expected_utils.hpp"
#include <charconv>       // std::from_chars
#include <stdexcept>
#include <cmath>

// ── parsePositiveInt ───────────────────────────────────────────────────────────

std::expected<int, ParseError> parsePositiveInt(std::string_view s) {
    if (s.empty()) {
        return std::unexpected{ParseError::EmptyInput};
        // unexpected<E>: 表示"意外地没有值，取而代之的是这个错误"
    }

    for (char c : s) {
        if (c < '0' || c > '9') {
            return std::unexpected{ParseError::InvalidChar};
        }
    }

    int value = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec == std::errc::result_out_of_range) {
        return std::unexpected{ParseError::Overflow};
    }

    return value;  // 成功: 返回期望的值
}

// ── parseErrorMsg ──────────────────────────────────────────────────────────────

std::string_view parseErrorMsg(ParseError e) {
    switch (e) {
        case ParseError::EmptyInput:         return "输入为空";
        case ParseError::InvalidChar:        return "包含非法字符 (只允许0-9)";
        case ParseError::Overflow:           return "数值溢出";
        case ParseError::NegativeNotAllowed: return "不允许负数";
    }
    return "未知错误";
}

// ── queryUserName (模拟数据库查询) ─────────────────────────────────────────────

std::expected<std::string, std::error_code> queryUserName(int userId) {
    // 模拟数据库: id 1-1000 是有效的
    if (userId < 1 || userId > 1000) {
        return std::unexpected{
            std::make_error_code(std::errc::no_such_file_or_directory)
        };
    }
    // 模拟查询结果
    return std::string{"用户_"} + std::to_string(userId);
}

// ── clampToRange ───────────────────────────────────────────────────────────────

std::expected<int, ParseError> clampToRange(int v, int min, int max) {
    if (v < min) {
        return std::unexpected{ParseError::NegativeNotAllowed};
    }
    if (v > max) {
        return std::unexpected{ParseError::Overflow};
    }
    return v;
}

// ── doubleIt ───────────────────────────────────────────────────────────────────

std::expected<int, ParseError> doubleIt(int v) {
    // 检测潜在的溢出
    if (v > 1'000'000'000) {
        return std::unexpected{ParseError::Overflow};
    }
    return v * 2;
}

// ── 三种安全除法 ──────────────────────────────────────────────────────────────

std::optional<double> safeDivideOpt(double a, double b) {
    if (b == 0.0) return std::nullopt;
    return a / b;
}

std::expected<double, DivError> safeDivideExp(double a, double b) {
    if (b == 0.0) return std::unexpected{DivError::DivisionByZero};
    double r = a / b;
    if (!std::isfinite(r)) return std::unexpected{DivError::Overflow};
    return r;
}

double safeDivideThrow(double a, double b) {
    if (b == 0.0) throw std::domain_error("除数不能为零");
    return a / b;
}
