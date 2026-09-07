#include "classes/class_basics.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>

using namespace lesson;

void part1_struct_vs_class() {
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

    std::println("出作用域后: {}", Student::total());
}

void part5_const_member() {
    print_subtitle("Part 5: const 成员函数");

    Temperature t(25.0);
    std::println("摄氏: {:1f}, 华氏: {:.1f}", t.get_celsius(), t.get_fahrenheit());

    const Temperature ct(0.0);
    std::println("冰点华氏: {:.1f}", ct.get_fahrenheit());

    for(int i = 0; i < 3; ++i) t.cached_fahrenheit();
    std::println("缓存访问次数: {}", t.read_count);
}


int main() {
    print_header("01 类的声明与基础");
    part1_struct_vs_class();
    part2_access_control();
    part3_this_pointer();
    part4_static_members();
    part5_const_member();
    std::println("01_class_declaration 完成! ");
    return 0;
}