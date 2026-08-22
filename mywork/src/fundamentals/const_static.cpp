#include <atomic>
#include "fundamentals/const_static.hpp"

namespace fundamentals {
    int create_counter() {
        static std::atomic<int> count = 0;

        return ++count;
    }

    int get_counter_value() {
        return -1;
    }
}