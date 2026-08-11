// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  error_safety/noexcept_contracts.hpp — noexcept 与契约                      ║
// ║  可复用的类型: SafeString (强异常安全保证), NoNoexceptMove, YesNoexceptMove ║
// ║  可复用的函数: safeAdd, conditionalSwap, computeImportantValue, 等           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <type_traits>
#include <cstdlib>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  safeAdd — 明确声明 noexcept 的简单函数                                     ║
// ║                                                                             ║
// ║  noexcept 是什么？                                                          ║
// ║  - 一种编译期契约：声明此函数不会抛出异常                                  ║
// ║  - 自 C++11 引入，C++17 起成为函数类型的一部分                             ║
// ║                                                                             ║
// ║  为什么需要 noexcept？                                                      ║
// ║  1. 编译器可以做更好的优化 (省略栈展开相关代码)                            ║
// ║  2. 让调用者知道：调用这个函数是安全的，不需要 try-catch 包裹             ║
// ║  3. 在泛型编程中判断是否可以使用移动等优化路径                              ║
// ║  4. 如果 noexcept 函数抛出了异常 → 直接 std::terminate()                  ║
// ║                                                                             ║
// ║  何时标记 noexcept？                                                        ║
// ║  - 析构函数 (编译器隐式添加)                                               ║
// ║  - 移动构造/赋值 (为了放入 vector 能高效重分配)                            ║
// ║  - swap 函数                                                                ║
// ║  - 简单的 getter/setter                                                     ║
// ║  - 内存释放函数 (operator delete)                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
constexpr int safeAdd(int a, int b) noexcept {
    return a + b;  // 加法不会抛出异常
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  conditionalSwap — 条件 noexcept：只有模板参数是 noexcept 可移动时才 noexcept║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T>
void conditionalSwap(T& a, T& b)
    noexcept(std::is_nothrow_move_constructible_v<T> &&
             std::is_nothrow_move_assignable_v<T>)
{
    T tmp{std::move(a)};
    a = std::move(b);
    b = std::move(tmp);
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  SafeString — 演示强异常安全保证：copy-and-swap 惯用法                      ║
// ║                                                                             ║
// ║  异常安全保证的三个级别：                                                   ║
// ║  1. 不抛出保证 (Nothrow): 用 noexcept 标记                                 ║
// ║  2. 强保证 (Strong): 操作要么完全成功，要么回滚到操作前状态                ║
// ║  3. 基本保证 (Basic): 异常发生时无资源泄漏，对象处于有效状态              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
class SafeString {
    std::string m_data;
public:
    explicit SafeString(std::string s) : m_data(std::move(s)) {}

    // 强异常安全保证的赋值:
    // 1. 先复制一份新数据 (可能抛出异常，但不影响原对象)
    // 2. 用 noexcept swap 替换 (swap 不抛异常)
    SafeString& operator=(const SafeString& other);

    void swap(SafeString& other) noexcept {
        m_data.swap(other.m_data);  // std::string::swap 是 noexcept
    }

    friend void swap(SafeString& a, SafeString& b) noexcept { a.swap(b); }

    const std::string& data() const { return m_data; }
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  NoNoexceptMove — 故意不声明 noexcept 移动的类型                           ║
// ║  YesNoexceptMove — 正确声明 noexcept 移动的类型                            ║
// ║                                                                             ║
// ║  std::vector 在重新分配内存时需要移动元素。                                ║
// ║  如果元素的移动构造函数是 noexcept，vector 使用移动 (高效)。              ║
// ║  如果移动不是 noexcept，vector 退化为拷贝 (安全但慢)。                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
struct NoNoexceptMove {
    std::string data;
    NoNoexceptMove(std::string d) : data(std::move(d)) {}
    NoNoexceptMove(NoNoexceptMove&&) = default;
};

struct YesNoexceptMove {
    std::string data;
    YesNoexceptMove(std::string d) : data(std::move(d)) {}
    YesNoexceptMove(YesNoexceptMove&&) noexcept = default;
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  FileResult — 错误码风格的枚举，配合 [[nodiscard]] 使用                    ║
// ║                                                                             ║
// ║  [[nodiscard]]: C++17 引入的属性，标记函数返回值不应被丢弃。              ║
// ║  C++20: [[nodiscard("消息")]] 可以带说明字符串。                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
enum class FileResult { Success, NotFound, PermissionDenied, IOError };

/// 纯计算函数 — 返回值不应被忽略
[[nodiscard]] int computeImportantValue();

/// C++20 风格: 带说明消息的 [[nodiscard]]
[[nodiscard("资源泄漏风险: 返回的句柄必须被管理")]]
void* allocateBuffer(size_t size);

/// 错误码风格的函数天然适合 [[nodiscard]]
[[nodiscard("必须检查文件操作的结果")]]
FileResult openFile(std::string_view path);
