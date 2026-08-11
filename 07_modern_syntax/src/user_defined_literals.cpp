// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/user_defined_literals.cpp                                               ║
// ║  实现用户定义字面量运算符函数                                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/user_defined_literals.hpp"

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Distance UDL 实现                                                           ║
// ║  1 km = 1000 m   1 mi = 1609.344 m                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// 浮点 _km: 1.5_km → 传入 1.5L，转为 1500.0 米
Distance operator""_km(long double km) {
    return Distance{static_cast<double>(km * 1000.0L)};
}

// 整数 _km: 42_km → 传入 42ULL，转为 42000.0 米
Distance operator""_km(unsigned long long km) {
    return Distance{static_cast<double>(km * 1000ULL)};
}

// 浮点 _mi: 2.5_mi → 传入 2.5L，转为米
Distance operator""_mi(long double miles) {
    return Distance{static_cast<double>(miles * 1609.344L)};
}

// 整数 _mi: 3_mi → 传入 3ULL，转为米
Distance operator""_mi(unsigned long long miles) {
    return Distance{static_cast<double>(miles * 1609ULL + miles * 344ULL / 1000ULL)};
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Angle UDL 实现                                                              ║
// ║  deg → rad: 乘以 π / 180                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

Angle operator""_deg(long double deg) {
    return Angle{static_cast<double>(deg * std::numbers::pi_v<long double> / 180.0L)};
}

Angle operator""_deg(unsigned long long deg) {
    return Angle{static_cast<double>(deg * std::numbers::pi / 180.0)};
}

Angle operator""_rad(long double rad) {
    return Angle{static_cast<double>(rad)};
}

Angle operator""_rad(unsigned long long rad) {
    return Angle{static_cast<double>(rad)};
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Color UDL 实现                                                              ║
// ║  0xRRGGBB → 拆出 R/G/B 三个通道                                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

Color operator""_hex(unsigned long long hex) {
    return Color{
        static_cast<uint8_t>((hex >> 16) & 0xFF),   // R
        static_cast<uint8_t>((hex >>  8) & 0xFF),   // G
        static_cast<uint8_t>((hex >>  0) & 0xFF)    // B
    };
}

} // namespace modern_syntax
