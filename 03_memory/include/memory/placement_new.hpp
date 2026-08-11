#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: memory/placement_new.hpp — 定位 new、对齐、手动内存管理 (库头)  ║
// ║  提供: Vec3 演示结构体、CacheLineAligned 对齐结构体                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <cmath>       // std::sqrt
#include <cstdint>     // std::uintptr_t
#include <cstddef>     // std::byte

/// @brief 三维向量 — 用于演示 placement new 的对象构造与析构
/// 打印构造/析构消息以便观察对象生命周期
struct Vec3 {
    float x, y, z;
    Vec3(float a, float b, float c) : x(a), y(b), z(c) {
        std::println("  构造 Vec3({}, {}, {})  @ {}", x, y, z, static_cast<void*>(this));
    }
    ~Vec3() { std::println("  析构 Vec3({}, {}, {})  @ {}", x, y, z, static_cast<void*>(this)); }
    float length() const { return std::sqrt(x*x + y*y + z*z); }
};
static_assert(sizeof(Vec3) == 12, "Vec3 应为 12 字节 (3 x float)");

/// @brief 使用 alignas 指定对齐的结构体 — 用于 SIMD 或 DMA 场景
/// 64 字节对齐确保不同线程的变量落在不同缓存行，避免伪共享 (false sharing)
struct alignas(64) CacheLineAligned {
    int data[8];        // 32 字节数据 + 填充至 64 字节边界
    void info() const {
        std::println("  对齐要求: alignof={}, sizeof={}", alignof(CacheLineAligned), sizeof(CacheLineAligned));
    }
};
