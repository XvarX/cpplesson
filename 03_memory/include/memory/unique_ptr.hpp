#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: memory/unique_ptr.hpp — 独占所有权智能指针 (库头文件)           ║
// ║  提供: Resource 演示类、FileCloser 自定义删除器、工厂函数、消费函数          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <memory>       // unique_ptr, make_unique (C++14), default_delete
#include <string>
#include <cstdio>       // FILE*, std::fclose (演示自定义删除器)

/// @brief 演示用资源类，构造和析构时打印消息
/// 模拟数据库连接、文件句柄等需要显式管理生命周期的资源
struct Resource {
    std::string name;
    Resource(std::string n) : name(std::move(n)) {
        std::println("  获取资源: {}", name);
    }
    ~Resource() { std::println("  释放资源: {}", name); }
    void use() const { std::println("  使用资源: {}", name); }
};

/// @brief 自定义删除器: 关闭 C 风格 FILE*
/// 演示 unique_ptr<T, Deleter> 管理非 new 出来的资源
struct FileCloser {
    void operator()(std::FILE* f) const noexcept {
        if (f) {
            std::println("  关闭文件句柄");
            std::fclose(f);
        }
    }
};

/// @brief 工厂函数: 返回 unique_ptr，简洁地转移所有权
/// make_unique 在函数内部创建，return 时自动移动所有权给调用者
std::unique_ptr<Resource> create_resource(const std::string& name);

/// @brief 接受 unique_ptr 作为参数的函数 (按值传递 = 接收所有权)
/// res 离开作用域时资源自动释放
void consume_resource(std::unique_ptr<Resource> res);
