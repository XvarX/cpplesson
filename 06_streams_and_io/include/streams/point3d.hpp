#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  point3d.hpp — Point3D 类型及其 std::formatter 特化                         ║
// ║  演示如何让自定义类型支持 std::format / std::print                          ║
// ║  这是 C++20 最重要的扩展点之一: 任何类型只要特化 formatter 就能用于 format   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <format>
#include <print>

namespace streams {

// ── Point3D: 三维空间中的点 ──────────────────────────────────────────────────
struct Point3D {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

} // namespace streams

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  std::formatter<streams::Point3D> 特化                                      ║
// ║  必须定义在 std 命名空间内                                                   ║
// ║  需要实现两个方法:                                                           ║
// ║    1. parse()   — 解析格式说明符 (如 {:f} 中的 f), 返回解析结束位置         ║
// ║    2. format()  — 执行格式化, 将结果写入输出迭代器                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template <>
struct std::formatter<streams::Point3D> {
    // 解析格式说明符 — 本示例只支持默认格式, 忽略所有说明符
    // ctx.begin() 指向格式说明符的起始位置 (即 '{' 后面的第一个字符)
    // 返回指向格式说明符结束位置 ('}') 的迭代器
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        while (it != ctx.end() && *it != '}') ++it;
        return it;  // 返回解析结束位置
    }

    // 格式化: 把 Point3D 写入输出迭代器
    // fc.out() 返回输出迭代器, 写入的内容最终到达目标 (字符串或控制台)
    // format_to 把格式化后的坐标拼接写入
    auto format(const streams::Point3D& p, std::format_context& fc) const {
        return std::format_to(fc.out(), "({:.1f}, {:.1f}, {:.1f})", p.x, p.y, p.z);
    }
};
