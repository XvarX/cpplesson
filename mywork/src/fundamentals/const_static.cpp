#include "fundamentals/const_static.hpp"

namespace fundamentals {
    int create_count() {
        static int count = 0;

        return ++count;
    }

    int get_counter_value() {
        return -1;
    }
}