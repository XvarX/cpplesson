#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace fundamentals {
    [[nodiscard]] std::string classify_number(int n);

    [[nodiscard]] std::string_view day_name(int day);

    void describe_type(char c);

    void printmonthday(const int& month);

    void print99();

    int calsum(const int& a);

    bool findbinary(const std::vector<int>& vec, const int& x);
}