#pragma once

#include <cstdint>
#include <cstddef>
#include <limits>

namespace fundamentals {
    inline constexpr std::size_t kSizeBool = sizeof(bool);
    inline constexpr std::size_t kSizeChar = sizeof(char);
    inline constexpr std::size_t kSizeShort = sizeof(short);
    inline constexpr std::size_t kSizeInt = sizeof(int);
    inline constexpr std::size_t kSizeLong = sizeof(long);
    inline constexpr std::size_t kSizeLongLong = sizeof(long long);
    inline constexpr std::size_t kSizeFloat = sizeof(float);
    inline constexpr std::size_t kSizeDouble = sizeof(double);
    
    inline constexpr int kIntMax = std::numeric_limits<int>::max();
    inline constexpr int kIntMin = std::numeric_limits<int>::min();
    inline constexpr long long kLLongMax = std::numeric_limits<long long>::max();
    inline constexpr double kDoubleMax = std::numeric_limits<double>::max();
    inline constexpr double kDoubleMin = std::numeric_limits<double>::min();
}