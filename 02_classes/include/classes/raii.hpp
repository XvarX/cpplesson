#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — RAII 与资源管理 (raii)                                  ║
// ║  内容: RAII 核心思想/ScopeGuard/LockGuard/文件 RAII 包装                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <string_view>
#include <exception>
#include <functional>
#include <utility>
#include <cstdio>
#include <cstring>
#include <cerrno>

// ═══════════════════════════════════════════════════════════════════════════════
// RaiiArray — RAII 核心: 将资源的生命周期绑定到对象
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 将资源的生命周期绑定到对象的生命周期
//        构造时获取资源 → 析构时自动释放资源
// 为什么: 消除手动释放、异常安全、代码简洁
// 何时用: 管理任何需要配对获取/释放的资源

class RaiiArray {
public:
    explicit RaiiArray(size_t size)
        : data_(new int[size]()), size_(size) {} // 构造: 分配

    ~RaiiArray() { delete[] data_; }              // 析构: 自动释放

    RaiiArray(const RaiiArray&) = delete;          // 禁止拷贝
    RaiiArray& operator=(const RaiiArray&) = delete;

    int& operator[](size_t i)       { return data_[i]; }
    int  operator[](size_t i) const { return data_[i]; }
    size_t size()             const { return size_; }

private:
    int* data_;
    size_t size_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ScopeGuard — 确保清理代码执行 (类似 Go 的 defer)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 在作用域结束时自动执行指定操作 (类似 Go 的 defer)
// 为什么: 确保即使异常或提前返回，清理/回滚代码也一定执行
// 何时用: 需要在函数出口执行清理 (保存状态、回滚事务、关闭句柄)

template <typename F>
class ScopeGuard {
public:
    explicit ScopeGuard(F&& f) : f_(std::forward<F>(f)), active_(true) {}

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ScopeGuard(ScopeGuard&& other) noexcept
        : f_(std::move(other.f_)), active_(std::exchange(other.active_, false)) {}

    ~ScopeGuard() { if (active_) f_(); }  // 析构时自动执行

    void dismiss() { active_ = false; }     // 取消执行 (操作成功则不需要回滚)

private:
    F f_;
    bool active_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// SimpleMutex / LockGuard — 互斥锁的 RAII 包装 (简化演示)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 构造时加锁, 析构时解锁 (std::lock_guard / std::scoped_lock 的简化版)
// 为什么: 永远不用手动 unlock; 异常安全; 避免死锁 (scoped_lock)
// 何时用: 每次使用互斥锁时都应使用 RAII 包装而非手动 lock/unlock

class SimpleMutex {
public:
    void lock()   { locked_ = true; }
    void unlock() { locked_ = false; }
    bool is_locked() const { return locked_; }
private:
    bool locked_ = false;
};

// std::lock_guard 的简化实现
class LockGuard {
public:
    explicit LockGuard(SimpleMutex& m) : mutex_(m) {
        mutex_.lock();
        std::println("    🔒 加锁");
    }

    ~LockGuard() {
        mutex_.unlock();
        std::println("    🔓 自动解锁");
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    SimpleMutex& mutex_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// File — 文件 RAII 包装
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 用 RAII 封装 FILE*，自动管理文件生命周期
// 为什么: 再也不用写 fclose — 文件绝不可能泄漏
// 何时用: 管理任何 OS 句柄 (文件/套接字/管道等)

class File {
public:
    explicit File(const char* path, const char* mode);

    ~File();

    // 禁止拷贝 (文件句柄不应共享)
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // 允许移动
    File(File&& other) noexcept : file_(std::exchange(other.file_, nullptr)) {}
    File& operator=(File&& other) noexcept {
        if (this != &other) {
            if (file_) std::fclose(file_);
            file_ = std::exchange(other.file_, nullptr);
        }
        return *this;
    }

    bool is_open() const { return file_ != nullptr; }

    std::string read_all();
    void write(std::string_view data);
    void close();

private:
    std::FILE* file_ = nullptr;
};
