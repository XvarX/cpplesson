#pragma once

#include <string>
#include <string_view>

namespace fundamentals {
    [[nodiscard]] std::string classify_number(int n);

    [[nodiscard]] std::string_view day_name(int day);

    void describt_type(char c);
}