#include "classes/class_basics.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>

using namespace lesson;

void print1_struct_vs_class() {
    print_subtitle("Part 1: class vs struct");

    Point p{3.0, 4.0};
    std::println("点坐标: ({}, {})", p.x, p.y);

    Circle c;
    c.set_radius(5.0);
    std::println("圆面积: {:.2f}", c.area());
}

void part2_access_control() {
    print_subtitle("Part 2: public / private / protected");

    BankAccount acc("艾克斯", 1000.0);
    acc.deposit(500.0);
    std::println("余额: {:.0f}", acc.balance());
}

void part3_this_pointer() {
    print_subtitle("Part 3: this 指针");

    Counter counter;
    counter.reset().increment().increment().add(5);
    std::println("计数值: {}", counter.value());

    counter.set_count(42);
    std::println("设置后: {}", counter.value());
}

void part4_static_members() {
    print_subtitle("Part 4: static 成员");

    std::println("初始学生数: {}", Student::total());

    {
        Student s1("张三");
        Student s2("李四");
        std::println("两人在校: {}", Student::total());
    }
}