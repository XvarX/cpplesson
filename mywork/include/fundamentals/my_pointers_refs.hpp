#pragma once

namespace fundamentals {
    int add(int a, int b);
    int sub(int a, int b);
    int mul(int a, int b);

    int calc(int x, int y, int(*op)(int, int));


    using BinaryIntOp = int (*)(int, int);

    int calc_v2(int x, int y, BinaryIntOp op);

    void swap(int* x, int* y);

    void swap2(int& x, int& y);

    int trimul(const int& x);

    using SingleIntOp = int (*)(const int&);

    void foreach(SingleIntOp op, int array[], int N);
}