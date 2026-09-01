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

    void printmonthday(const int& month) {
        switch(month) {
            case 1: case 3:case 5:case 7: case 8: case 10: case 12:
                std::println("{}月份有 31 天", month);
                break;
            case 4:case 6:case 9:case 11:
                std::println("{}月份有 30 天", month);
                break;
            case 2:
                std::println("2月份有 28 天");
                [[fallthrough]];
            default:
                std::println("你输入的月份不对");
        }
    }

    void print99() {
        for(int i = 1; i <= 9; i++) {
            for (int j = 1; j <= 9; j++) {
                std::print("{}x{} = {}",i, j, i*j);
                std::print(" ");
            }
            std::println("");
        }
    }

    int calsum(const int& x) {
        int iSum = x;
        int iResult = 0;
        while(iSum != 0) {
            iResult += iSum%10;
            iSum = iSum/10;
        }
        return iResult;
    }

    bool findbinary(const std::vector<int>& vec, const int& x) {
        int len = vec.size();
        int index = (len-1)/2;
        int startindex = 0;
        int endindex = len-1;
        int iFlag = 0;

        while(true) {
            if (startindex == index || endindex == index) {
                if (vec[startindex] == x) {
                    iFlag = 1;
                } else if (vec[endindex] == x) {
                    iFlag = 1;
                }

                break;
            } else {

                if (vec[index] == x) {
                    iFlag = 1;
                    break;
                } else if (vec[index] < x) {
                    startindex = index;
                    index = (startindex+endindex)/2;
                } else if (vec[index] > x) {
                    endindex = index;
                    index = (startindex+endindex)/2;
                }
            }
        }

        if(iFlag == 1) {
            return true;
        } else {
            return false;
        }

    }
}