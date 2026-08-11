#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  rvalue_move.hpp — 左值/右值、移动语义、noexcept 移动、RVO/NRVO             ║
// ║  提供: Buffer(移动构造/赋值演示)、Tracker(构造追踪)、noexcept 对比类       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <algorithm>

namespace move_lambda {

// ═══════════════════════════════════════════════════════════════════════════════
// Buffer — 管理堆内存的类, 演示移动构造/移动赋值
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  移动构造: Buffer(Buffer&& other)        — 从 other 那里"偷走"资源
//        移动赋值: Buffer& operator=(Buffer&&)   — 释放自己的旧资源, 偷走 other 的
// WHY:   避免深拷贝, 对于管理堆内存的类性能提升巨大(O(n)→O(1))
// WHEN:  类管理了堆内存/文件句柄/锁等不可共享的资源时, 必须实现移动语义

class Buffer {
private:
    int* data_;
    size_t size_;

public:
    explicit Buffer(size_t n) : data_(new int[n]{}), size_(n) {
        std::println("[构造] 分配 {} 个 int", n);
    }

    // 拷贝构造(深拷贝) — 开销大
    Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::println("[拷贝] 深拷贝 {} 个 int", size_);
    }

    // ⭐ 移动构造 — 只转移指针, O(1)!
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;   // 关键! 让 other 不再拥有资源
        other.size_ = 0;         // 保持 other 处于有效状态
        std::println("[移动] 偷走 {} 个 int 的所有权", size_);
    }

    // ⭐ 移动赋值
    Buffer& operator=(Buffer&& other) noexcept {
        std::println("[移动赋值]");
        if (this != &other) {
            delete[] data_;              // 先释放自己的旧资源
            data_ = other.data_;         // 偷走 other 的资源
            size_ = other.size_;
            other.data_ = nullptr;       // other 不再拥有
            other.size_ = 0;
        }
        return *this;
    }

    ~Buffer() { delete[] data_; }

    size_t size() const { return size_; }
    bool empty() const { return data_ == nullptr; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// NoNoexceptMove — 移动构造没有 noexcept 的类
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  没有 noexcept → std::vector 扩容时使用拷贝而非移动(保证异常安全)
class NoNoexceptMove {
public:
    int* data = nullptr;
    NoNoexceptMove() = default;
    // 注意: 没有 noexcept!
    NoNoexceptMove(NoNoexceptMove&& other) : data(other.data) {
        other.data = nullptr;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// WithNoexceptMove — 移动构造标记了 noexcept 的类
// ═══════════════════════════════════════════════════════════════════════════════
// ✅ 标记了 noexcept → std::vector 扩容时优先使用移动, 性能更好
class WithNoexceptMove {
public:
    int* data = nullptr;
    WithNoexceptMove() = default;
    WithNoexceptMove(WithNoexceptMove&& other) noexcept : data(other.data) {
        other.data = nullptr;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Tracker — 带输出的结构体, 用于观察构造/拷贝/移动的调用
// ═══════════════════════════════════════════════════════════════════════════════
struct Tracker {
    int id;
    Tracker(int i) : id(i) {
        std::println("  构造 Tracker({})", id);
    }
    Tracker(const Tracker& o) : id(o.id) {
        std::println("  拷贝 Tracker({})", id);
    }
    Tracker(Tracker&& o) noexcept : id(o.id) {
        std::println("  移动 Tracker({})", id);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// create_good / create_bad — 演示 RVO/NRVO 的行为差异
// ═══════════════════════════════════════════════════════════════════════════════
// ✅ 好写法: 返回局部变量, 编译器会自动做 NRVO
Tracker create_good();

// ❌ 坏写法: 对局部变量 std::move, 反而阻止了 NRVO!
Tracker create_bad();

} // namespace move_lambda
