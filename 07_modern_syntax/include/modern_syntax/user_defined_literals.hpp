#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  modern_syntax/user_defined_literals.hpp                                   ║
// ║  用户定义字面量 (operator"") —— _km/_deg 自定义后缀、                      ║
// ║  std::to_underlying (C++23)、标准库字面量 (chrono / string)                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <string>
#include <string_view>
#include <cmath>
#include <numbers>    // std::numbers::pi (C++20)

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  WHAT: 用户定义字面量 (User-Defined Literals, UDL)                          ║
// ║        允许定义后缀运算符，把字面量自动包装成自定义类型                     ║
// ║  WHY:  ① 消除单位混淆 —— 42_km vs 42_mi 不会搞混                          ║
// ║        ② 编译期类型安全 —— 不同单位的字面量是不同类型                      ║
// ║        ③ 自然表达力 —— 1.5_deg 比构造函数 Angle(1.5) 更直观               ║
// ║  WHEN: 物理量(距离/角度/温度/压力)、货币、颜色、测量值等带单位的量         ║
// ║        标准库已有: 1s (秒), 1ms (毫秒), "hello"s (std::string)              ║
// ║  RULE: 自定义 UDL 必须以 _ 开头 (C++ 标准保留无下划线前缀给实现)          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  1. Distance —— 物理距离，以米为内部存储单位                                ║
// ║     提供 _km (千米)、_mi (英里) 两个 UDL，自动转换为米                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
struct Distance {
    double meters;   // 内部统一以米为单位

    // 便捷查询
    constexpr auto as_km() const -> double { return meters / 1000.0; }
    constexpr auto as_mi() const -> double { return meters / 1609.344; }
};

// ── UDL 声明 (实现在 src/user_defined_literals.cpp) ─────────────────────────
// 浮点参数: 1.5_km  → operator""_km(1.5L)
Distance operator""_km(long double km);
// 整数参数: 42_km   → operator""_km(42ULL)
Distance operator""_km(unsigned long long km);
// 英里
Distance operator""_mi(long double miles);
Distance operator""_mi(unsigned long long miles);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  2. Angle —— 角度，以弧度为内部存储单位                                      ║
// ║     提供 _deg (度)、_rad (弧度) 两个 UDL                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
struct Angle {
    double radians;   // 内部统一以弧度为单位

    // 便捷查询
    constexpr auto degrees() const -> double { return radians * 180.0 / std::numbers::pi; }
};

// ── UDL 声明 ────────────────────────────────────────────────────────────────
Angle operator""_deg(long double deg);
Angle operator""_deg(unsigned long long deg);
Angle operator""_rad(long double rad);
Angle operator""_rad(unsigned long long rad);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  3. 自定义颜色字面量 _hex —— 以十六进制 0xRRGGBB 表示 RGB 颜色             ║
// ║     (仅接受整数参数，演示 UDL 参数类型的多样性)                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
struct Color {
    uint8_t r, g, b;
};

// 仅整数参数 (没有浮点版本 —— 0x123 不会是浮点数)
Color operator""_hex(unsigned long long hex);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  4. std::to_underlying (C++23) —— 将枚举值安全转换为底层整数                ║
// ║  WHAT:  std::to_underlying(enum_val) 替代 static_cast<UT>(enum_val)         ║
// ║  WHY:   ① 意图更明确 —— "我要底层整数" 而非泛泛的 "static_cast 某种转换"  ║
// ║         ② 自带类型检查 —— 编译器确认传入的是枚举类型                       ║
// ║         ③ 不会意外转换非枚举类型                                           ║
// ║  WHEN:  枚举转整数 (日志、序列化、索引、位运算)                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// 演示用枚举 —— 操作码
enum class OpCode : uint8_t {
    Nop    = 0,
    Load   = 1,
    Store  = 2,
    Add    = 3,
    Halt   = 255
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  5. 标准库字面量概览 (在 demo 中引入 using namespace 后用)                  ║
// ║  ┌──────────────────┬───────────────────────────────────────────────────┐   ║
// ║  │ std::chrono_literals │ 1s, 1ms, 1us, 1ns, 1h, 1min (C++14)         │   ║
// ║  │ std::string_literals │ "hello"s    → std::string      (C++14)       │   ║
// ║  │ std::string_view_literals │ "hello"sv → std::string_view (C++17)   │   ║
// ║  │ std::complex_literals│ 1.0 + 2.0i  → std::complex<double> (C++14)   │   ║
// ║  │ std::numbers          │ 3.14159f16 (std::float16_t 字面量, C++23)   │   ║
// ║  └──────────────────┴───────────────────────────────────────────────────┘   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

} // namespace modern_syntax
