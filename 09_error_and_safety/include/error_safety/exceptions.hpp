// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  error_safety/exceptions.hpp — 异常机制基础                                  ║
// ║  可复用的类型: DatabaseConnection, ScopeGuard                                ║
// ║  可复用的函数: deepFunction, maybeThrow                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <string>
#include <string_view>
#include <stdexcept>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  DatabaseConnection — 演示构造函数中使用异常                                ║
// ║                                                                             ║
// ║  构造函数无法返回值，只能用异常报告构造失败。                                ║
// ║  如果在构造函数中抛出异常，对象不会被创建，已构造的子对象会被析构。          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
class DatabaseConnection {
    std::string m_connStr;
public:
    /// @throws std::invalid_argument 如果连接字符串为空
    /// @throws std::runtime_error   如果连接字符串以 "bad:" 开头（模拟连接失败）
    explicit DatabaseConnection(std::string_view connStr);

    std::string_view connectionString() const { return m_connStr; }
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  ScopeGuard — RAII 守卫，记录进入和离开作用域                               ║
// ║                                                                             ║
// ║  用于演示栈展开 (Stack Unwinding)：                                         ║
// ║  当异常被抛出时，所有离开作用域的局部对象的析构函数会被调用。                ║
// ║  析构函数默认 noexcept，不应抛出异常。                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
class ScopeGuard {
    std::string m_name;
public:
    explicit ScopeGuard(std::string name);
    ~ScopeGuard();
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  deepFunction — 递归调用，演示栈展开                                        ║
// ║                                                                             ║
// ║  当 level >= 3 时抛出异常，触发栈展开。                                     ║
// ║  观察所有 ScopeGuard 对象的析构顺序（构造的逆序）。                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void deepFunction(int level);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  maybeThrow — 简单的 throw 开关，用于演示 catch(...) 与重新抛出            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void maybeThrow(bool shouldThrow);
