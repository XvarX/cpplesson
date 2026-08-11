#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: memory/allocator_basics.hpp — 分配器入门 (库头文件)             ║
// ║  提供: Point 演示结构体、CountingAllocator 自定义统计分配器                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <cstddef>      // std::size_t

/// @brief 演示用简单类型 — 构造时打印消息，方便观察分配器行为
struct Point {
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {
        std::println("    Point({}, {}) 构造", x, y);
    }
    ~Point() { /* 为了简洁, 不打印析构 */ }
};

/// @brief 一个最简单的自定义分配器 — 基于 std::allocator 的统计包装
/// 记录分配次数、释放次数、总分配字节数等统计信息
/// @note 分配器必须可 rebind — 让 allocator<int> 能分配 allocator<string>
template <typename T>
struct CountingAllocator {
    using value_type = T;

    // 分配器必须可 rebind — 让 allocator<int> 能分配 allocator<string>
    template <typename U>
    struct rebind { using other = CountingAllocator<U>; };

    CountingAllocator() = default;
    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) noexcept {}

    /// @brief 核心函数: allocate — 分配 n 个 T 对象的原始内存
    [[nodiscard]] T* allocate(std::size_t n) {
        ++alloc_count;
        total_bytes += n * sizeof(T);
        if (n > max_alloc) max_alloc = n;
        auto* p = static_cast<T*>(::operator new(n * sizeof(T)));
        // std::println("  [分配器] 分配 {} 个 {} ({} 字节)", n, typeid(T).name(), n * sizeof(T));
        return p;
    }

    /// @brief 核心函数: deallocate — 释放之前分配的内存
    void deallocate(T* p, std::size_t n) noexcept {
        ++dealloc_count;
        ::operator delete(p, n * sizeof(T));
    }

    // C++11+: 可选, 允许在已分配内存上构造 (用于优化)
    // 不实现则使用全局 placement new

    // 两个分配器是否等价 (决定容器能否交换内存)
    bool operator==(const CountingAllocator&) const noexcept { return true; }

    // 统计信息 (C++17 inline static 成员)
    static inline std::size_t alloc_count   = 0;
    static inline std::size_t dealloc_count = 0;
    static inline std::size_t total_bytes   = 0;
    static inline std::size_t max_alloc     = 0;

    static void print_stats() {
        std::println("  分配次数: {}, 释放次数: {}", alloc_count, dealloc_count);
        std::println("  总分配字节: {}, 最大单次分配: {} 个元素", total_bytes, max_alloc);
    }
};
