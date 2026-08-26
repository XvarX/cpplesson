#include "fundamentals/my_control_flow.hpp"
#include <print>

namespace fundamentals {
    [[nodiscard]] std::string classify_number(int n) {
        if (n > 0) return "正数";
        if (n < 0) return "负数";
        return "零";
    }

    [[nodiscard]] std::string_view day_name(int day) {
        switch(day) {
            case 1: return "星期一";
            case 2: return "星期二";
            case 3: return "星期三";
            case 4: return "星期四";
            case 5: return "星期五";
            case 6: return "星期六";
            case 7: return "星期日";
            default: return "无效的日子";
        }
    }

    void describe_type(char c) {
        switch (c) {
            case 'a':case 'e': case 'i': case 'o': case 'u':
            case 'A':case 'E': case 'I': case 'O': case 'U':
                std::println("'{}' 是元音字母", c);
                break;
            
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                [[fallthrough]];
            default:
                std::println("'{}' 不是元音（数学或其他字符）", c);
                break;
        }
    }
}