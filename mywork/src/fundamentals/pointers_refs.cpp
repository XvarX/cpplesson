#include<memory>
#include "fundamentals/my_pointers_refs.hpp"

namespace fundamentals {
    int add(int a, int b) {
        return a + b;
    }

    int sub(int a, int b) {
        return a - b;
    }

    int mul(int a, int b) {
        return a * b;
    }

    int calc(int x, int y, int(*op)(int, int)) {
        return op(x, y);
    }

    int calc_v2(int x, int y, BinaryIntOp op) {
        return op(x, y);
    }

    void swap(int* x, int *y) {
        int temp = *x;
        *x = *y;
        *y = temp;
    }

    void swap2(int& x, int& y) {
        int temp = x;
        x = y;
        y = temp;
    }

    int trimul(const int& x) {
        int temp = x*3;
        return temp;
    }

    void foreach(SingleIntOp op, int array[], int N) {
        for(int i=0; i < N; i++) {
            array[i] = op(array[i]);
        }
    }

    int** alloc_2d_int(int rows, int cols) {
        int** arr = static_cast<int**>(malloc(sizeof(int*) * rows));
        if (arr == nullptr) {
            return nullptr;
        }

        for(int i=0;i < rows; i++) {
            arr[i] = static_cast<int*>(malloc(sizeof(int) * cols));
            if(arr[i] == nullptr) {
                
            }
        }
    }
}