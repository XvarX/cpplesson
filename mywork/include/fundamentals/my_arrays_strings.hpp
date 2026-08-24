#pragma once

#include <cstddef>
#include <array>

#include <iostream>

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

    template<std::size_t N>
    void reverse2(int (&array)[N]) {
        for(int i=0; i < N/2; i++) {
            int temp = array[i];
            array[i] = array[N-(i+1)];
            array[N-(i+1)] = temp;
        }
    }

    int mystrlen(const char* str) {
        int i = 0;
        while(1) {
            if(str[i] == '\0') {
                break;
            }
            i+=1;
        }
        return i;

    }

    void reverse3(std::array<int, 6>* myarray) {
        int N = 6;
        for(int i=0; i < N/2; i++) {
            int temp = myarray->at(i);
            myarray->at(i) = myarray->at(N-(i+1));
            myarray->at(N-(i+1)) = temp;
        }
    }

    template<std::size_t N>
    void reverse4(std::array<int, N>* myarray) {
        for(int i=0; i < N/2; i++) {
            int temp = myarray->at(i);
            myarray->at(i) = myarray->at(N-(i+1));
            myarray->at(N-(i+1)) = temp;
        }
    }

    std::size_t printsizeof(int array[10]) {
        return sizeof(array);
    }
};