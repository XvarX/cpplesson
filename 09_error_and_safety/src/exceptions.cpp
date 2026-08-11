// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  实现: error_safety/exceptions.hpp                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "error_safety/exceptions.hpp"
#include <print>
#include <stdexcept>

// ── DatabaseConnection 构造函数 ────────────────────────────────────────────────

DatabaseConnection::DatabaseConnection(std::string_view connStr)
    : m_connStr(connStr)
{
    std::println("  尝试连接数据库: {}", m_connStr);
    if (m_connStr.empty()) {
        throw std::invalid_argument("连接字符串不能为空");
    }
    if (m_connStr.starts_with("bad:")) {
        throw std::runtime_error("数据库连接失败: 服务器不可达");
    }
    std::println("  连接成功!");
}

// ── ScopeGuard ─────────────────────────────────────────────────────────────────

ScopeGuard::ScopeGuard(std::string name) : m_name(std::move(name)) {
    std::println("    -> [{}] 进入作用域", m_name);
}

ScopeGuard::~ScopeGuard() {
    std::println("    <- [{}] 离开作用域 (析构)", m_name);
}

// ── deepFunction — 递归调用，演示栈展开 ────────────────────────────────────────

void deepFunction(int level) {
    ScopeGuard sg("deepFunction 级别 " + std::to_string(level));
    if (level >= 3) {
        std::println("    !!! [级别{}] 抛出异常!", level);
        throw std::runtime_error("深层错误");
    }
    if (level < 3) deepFunction(level + 1);
}

// ── maybeThrow ─────────────────────────────────────────────────────────────────

void maybeThrow(bool shouldThrow) {
    if (shouldThrow) {
        throw std::logic_error("逻辑错误: shouldThrow == true");
    }
}
