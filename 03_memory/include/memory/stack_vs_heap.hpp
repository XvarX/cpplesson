#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: memory/stack_vs_heap.hpp — 栈 vs 堆分配 (库头文件)              ║
// ║  提供: Widget 演示类、全局 operator new/delete 声明 (用于追踪分配)           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <cstddef>   // std::size_t
#include <cstdlib>   // std::malloc, std::free

/// @brief 演示用的简单类，带构造/析构消息
/// 用于直观观察栈分配和堆分配时对象的生命周期差异
struct Widget {
    int id;
    Widget(int i) : id(i) { std::println("  构造 Widget #{}", id); }
    ~Widget() { std::println("  析构 Widget #{:}", id); }
};

/// @brief 重载全局 operator new 来追踪每一次堆分配
/// @note 这是教学演示用途，生产代码中极少重载全局 operator new
void* operator new(std::size_t size);

/// @brief 配套的全局 operator delete (非 sized 版本)
void operator delete(void* p) noexcept;

/// @brief 配套的全局 operator delete (sized 版本, C++14+)
void operator delete(void* p, std::size_t size) noexcept;
