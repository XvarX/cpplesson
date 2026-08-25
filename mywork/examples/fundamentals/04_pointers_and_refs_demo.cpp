#include <print>
#include <string>
#include <memory>
#include <cstddef>
#include <functional>
#include "shared/lesson_utils.hpp"
#include "fundamentals/my_pointers_refs.hpp"

int main() {
    lesson::print_header("04 指针、引用、nullptr、函数指针、void*");

    lesson::print_subtitle("Part 1: 指针基础");

    int value = 42;
    int* ptr = &value;

    int** ptr_to_ptr = &ptr;

    std::println("value         = {}", value);
    std::println("&value        = {}", static_cast<const void*>(&value));
    std::println("ptr           = {} (就是 &value)", static_cast<const void*>(ptr));
    std::println("*ptr          = {} (解引用, 就是 value)", *ptr);
    std::println("&ptr          = {} (指针自己的地址)", static_cast<const void*>(&ptr));
    std::println("**ptr_to_ptr  = {} (两次解引用)", **ptr_to_ptr);

    *ptr = 100;
    std::println("通过 *ptr = 100 修改后， value = {}", value);

    lesson::print_subtitle("Part 2: nullptr vs NULL");

    int* np = nullptr;
    static_assert(std::is_same_v<decltype(nullptr), std::nullptr_t>);

    if (np == nullptr) {
        std::println("np 是空指针, 不能解引用");
    }

    lesson::print_subtitle("Part 3: 引用");

    int original = 10;
    int& ref = original;

    std::println("original = {}, ref = {}", original, ref);

    original = 30;
    std::println("original = {}, ref = {}", original, ref);

    lesson::print_subtitle("Part 4: const 指针与 const 引用");

    int x = 50;
    const int* cp = &x;
    int const * cp2 = &x;
    int* const pc = &x;
    *pc = 60;
    const int* const cpc = &x;

    std::println("经过 *pc=60, x = {}", x);

    const int& cref = x;

    std::println("const 引用绑定到 x:{}", cref);

    const int& temp_ref = 100;
    std::println("const 引用绑定到临时对象: {}", temp_ref);

    lesson::print_subtitle("Part 5: 函数指针");

    int (*fp)(int, int) = nullptr;
    fp = fundamentals::add;
    std::println("fp(3, 5) = add(3, 5) = {}", fp(3, 5));

    fp = fundamentals::sub;
    std::println("fp(3, 5) = sub(3, 5) = {}", fp(3, 5));

    std::println("calc(10, 5, add) = {}", fundamentals::calc(10, 5, fundamentals::add));
    std::println("calc(10, 5, mul) = {}", fundamentals::calc(10, 5, fundamentals::mul));

    int (*ops[])(int, int) = {
        fundamentals::add,
        fundamentals::sub,
        fundamentals::mul
    };

    std::println("函数指针数组 ops: add={}, sub={}, mul={}",
                 ops[0](8, 3), ops[1](8, 3), ops[2](8, 3));

    fundamentals::BinaryIntOp op_alias = fundamentals::add;

    std::println("通过类型别名调用: calc_v2(20, 10, add) = {}",
                 fundamentals::calc_v2(20, 10, op_alias));

    lesson::print_subtitle("Part 6: 指针的指针");

    int val = 999;
    int* p1 = &val;
    int** p2 = &p1;
    int*** p3 = &p2;

    std::println("val = {}", val);
    std::println("*p1 = {}", *p1);
    std::println("**p2 = {}", **p2);
    std::println("***p3 = {}", ***p3);

    **p2 = 777;
    std::println("**p2 = 777 后, val = {}", val);

    lesson::print_subtitle("Part 7: void*");

    int i = 10;
    double d = 3.14;
    void* vp = &i;

    std::println("void* 指向 int: {}", static_cast<const void*>(vp));
    vp = &d;
    std::println("void* 指向 double: {}", static_cast<const void*>(vp));

    int* ip = static_cast<int*>(vp);
    vp = &i;
    ip = static_cast<int*>(vp);
    std::println("正确转换: *ip = {}", *ip);

    int a = 5;
    int b = 10;
    fundamentals::swap2(a, b);

    std::println("swap a b {} {}", a, b);

    int myarray[] = {1,2,3,4,5};

    fundamentals::foreach(fundamentals::trimul, myarray, 5);

    std::println("for each {}", myarray[1]);

}