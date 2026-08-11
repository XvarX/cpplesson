// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  pimpl.cpp — Widget 类的实现                                               ║
// ║  包含 Impl 的完整定义和所有 Widget 成员函数的实现                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "patterns/pimpl.hpp"
#include <print>
#include <string>

namespace patterns {

// ═══════════════════════════════════════════════════════════════════════════════
// 私有实现类: 包含所有真正的数据成员和实现逻辑
// ═══════════════════════════════════════════════════════════════════════════════
struct Widget::Impl {
    std::string name;
    int         value = 0;

    // 实现类可以自由包含任意头文件,不会污染调用方
    // 比如可以 #include <vector>, <map>, 第三方库等
    void do_something_internal() {
        // 复杂的内部逻辑,修改这里不需要重新编译 Widget 的用户
    }
};

// ── 构造函数: 在 .cpp 中创建 Impl 对象 ────────────────────────────────────────
Widget::Widget(std::string name)
    : pimpl_(std::make_unique<Impl>(Impl{std::move(name), 0}))
{}

// ── 析构函数: 必须在这里定义(能看到 Impl 完整定义的地方) ──────────────────────
// 如果放在头文件中用 = default,编译器会尝试在头文件中生成析构代码,
// 但那时 Impl 是不完整类型,导致编译错误
Widget::~Widget() = default;

// ── 移动构造/赋值 ──────────────────────────────────────────────────────────────
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

// ── 公开接口实现: 委托给 Impl ────────────────────────────────────────────────
void Widget::set_value(int v) {
    pimpl_->value = v;
}

int Widget::get_value() const {
    return pimpl_->value;
}

void Widget::set_name(std::string_view name) {
    pimpl_->name = std::string(name);
}

std::string_view Widget::get_name() const {
    return pimpl_->name;
}

void Widget::print_state() const {
    std::println("Widget[{}] value = {}", pimpl_->name, pimpl_->value);
}

} // namespace patterns
