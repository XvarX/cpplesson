#pragma once

#include <cstddef>
#include <array>

namespace fundamentals {
    template<typename T, std::size_t N>
    constexpr std::size_t array_size(const T (&)[N]) noexcept {
        return N;
    }

    inline constexpr int demo_scores[] = {95, 82, 78, 91, 88};
    inline constexpr std::size_t kDemoScoreSize = array_size(demo_scores);

    inline constexpr int demo_matrix[2][3] = {
        {1,2,3},
        {4,5,6}
    };

    void reverse1(int array[], int N) {
        for(int i=0; i < N/2; i++) {
            int temp = array[i];
            array[i] = array[N-(i+1)];
            array[N-(i+1)] = temp;
        }
    };
};