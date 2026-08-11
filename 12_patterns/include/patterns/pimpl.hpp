// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  pimpl.hpp — Pimpl 惯用法 (Pointer-to-Implementation)                       ║
// ║  编译防火墙：隐藏实现细节，减少头文件依赖，加速编译                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 核心概念:
//   - 公开头文件只暴露接口，实现细节放在 .cpp 中
//   - 通过 unique_ptr 持有前向声明的实现类
//   - 修改实现类时不需要重新编译使用 Widget 的代码
//   - 保持 ABI 稳定：实现类的内存布局变化不影响调用方
// 适用场景:
//   - 库的公开 API 头文件
//   - 成员较多且经常变化的类
//   - 需要隐藏第三方库依赖时
// 常见陷阱:
//   - unique_ptr + 前向声明需要显式析构函数(放在 .cpp 中)
//   - 增加了间接访问开销(一次指针解引用)
//   - 实现类的所有成员函数调用都需要通过 pimpl 指针

#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace patterns {

// ── Widget: Pimpl 惯用法演示类 ────────────────────────────────────────────────
// 公开头文件只暴露接口声明，不暴露实现细节
class Widget {
public:
    // 构造函数和析构函数
    explicit Widget(std::string name);
    ~Widget();                       // ⚠️ 必须在 .cpp 中定义!
                                     // 因为 unique_ptr 的 deleter 需要看到完整类型

    Widget(Widget&&) noexcept;       // 移动构造
    Widget& operator=(Widget&&) noexcept; // 移动赋值

    // 公开接口 — 调用方只需要知道这些
    void set_value(int v);
    int  get_value() const;
    void set_name(std::string_view name);
    std::string_view get_name() const;
    void print_state() const;

private:
    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  关键: 只声明不定义 — 头文件中不需要 #include "widget_impl.hpp" ║
    // ╚══════════════════════════════════════════════════════════════════╝
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace patterns
