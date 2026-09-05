#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 拷贝控制与移动语义 (copy_move)                         ║
// ║  内容: 三五零法则 / swap 惯用法 / noexcept 移动                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <utility>
#include <algorithm>
#include <cstring>

// ═══════════════════════════════════════════════════════════════════════════════
// Point3D — 零法则示例: 不管理资源，不定义任何特殊函数
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 零法则 → 若类不管理资源，不定义任何特殊函数 — 交给编译器
// 为什么: 编译器自动生成拷贝/移动/析构，一切正确
// 何时用: 成员都是值类型 (int, double, string, vector...) 的类

class Point3D {
public:
    Point3D(double x, double y, double z) : x_(x), y_(y), z_(z) {}
    void print() const { std::println("({}, {}, {})", x_, y_, z_); }
private:
    double x_, y_, z_;  // 编译器管理拷贝/移动，一切正确
};

// ═══════════════════════════════════════════════════════════════════════════════
// Buffer — 五法则示例: 管理动态资源，需定义全部五种特殊函数
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 五法则 → 若需要自定义析构/拷贝/移动之一 → 五者都需要
// 为什么: 编译器自动生成是浅拷贝 → double-free / 悬空指针
// 何时用: 类管理动态内存、文件句柄、锁等需要手动释放的资源

class Buffer {
public:
    explicit Buffer(size_t size)
        : data_(new int[size]()), size_(size) {}

    // 2. 拷贝构造: 深拷贝
    Buffer(const Buffer& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy_n(other.data_, size_, data_);
    }

    // 3. 拷贝赋值: 使用 copy-and-swap 惯用法
    Buffer& operator=(const Buffer& other) {
        Buffer tmp(other);   // 复用拷贝构造
        swap(tmp);           // 安全交换
        return *this;
    }

    // 4. 移动构造: 窃取资源，源对象置空
    Buffer(Buffer&& other) noexcept
        : data_(std::exchange(other.data_, nullptr))
        , size_(std::exchange(other.size_, 0)) {}

    // 5. 移动赋值: 同样 noexcept
    Buffer& operator=(Buffer&& other) noexcept {
        Buffer tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    // 1. 析构: 释放资源
    ~Buffer() { delete[] data_; }

    void swap(Buffer& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    size_t size() const { return size_; }

private:
    int* data_;
    size_t size_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// String — swap 惯用法 (copy-and-swap 统一拷贝/移动赋值)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: copy-and-swap: 先拷贝一份临时对象，再与自身 swap
// 为什么: ① 异常安全 (拷贝失败时原对象不变)
//         ② 代码复用 (拷贝赋值和移动赋值统一逻辑)
//         ③ 自动处理自赋值
// 何时用: 实现拷贝赋值和移动赋值时

class String {
public:
    explicit String(const char* s)
        : data_(new char[std::strlen(s) + 1]) {
        std::strcpy(data_, s);
    }

    String(const String& other)
        : data_(new char[std::strlen(other.data_) + 1]) {
        std::strcpy(data_, other.data_);
    }

    String(String&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)) {}

    // copy-and-swap: 拷贝赋值和移动赋值合二为一
    String& operator=(String other) noexcept { // other 按值传递!
        swap(other);  // other 是拷贝或移动来的，直接 swap
        return *this;
    }

    ~String() { delete[] data_; }

    void swap(String& other) noexcept { std::swap(data_, other.data_); }
    const char* c_str() const { return data_; }

private:
    char* data_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// NoexceptDemo — noexcept 对 vector 扩容行为的影响
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: noexcept 标记函数不抛异常
// 为什么: ① 编译器可生成更优代码
//         ② vector 扩容时: 移动操作标记 noexcept 才用移动，否则降级为拷贝
// 何时用: 移动构造/移动赋值必须 noexcept；析构函数默认 noexcept

class NoexceptDemo {
public:
    explicit NoexceptDemo(int id) : id_(id) {}

    NoexceptDemo(const NoexceptDemo&) {
        std::println("  📋 拷贝 NoexceptDemo({})", id_);
    }

    NoexceptDemo(NoexceptDemo&&) noexcept {         // 加上 noexcept!
        std::println("  🚀 移动 NoexceptDemo({})", id_);
    }

    // 如果没有 noexcept，vector 扩容时会退化为拷贝!
private:
    int id_;
};
