#include "classes/my_ctor_dtor.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace lesson;

void part1_constructor_basics() {
    print_subtitle("Part 1: 构造函数基础");

    Person p1;
    p1.introduce();

    Person p2("艾克斯", 25);
    p2.introduce();
}

void part2_init_list() {
    print_subtitle("Part 2: 初始化列表");

    Config cfg("/etc/app.conf", 8080);
    cfg.show();
}

void part3_delegating_ctor() {
    print_subtitle("Part 3: 委托构造");

    MyRectangle square_(5.0);
    MyRectangle rect(6.0, 4.0);
    std::println("正方形面积: {:.0f}", square_.area());
    std::println("矩形面积: {:.0f}", rect.area());
}

void part4_default_delete() {
    print_subtitle("Part 4: =default/ =delete");

    NonCopyable a;
    NonCopyable c = std::move(a);

    Trivial t1;
    Trivial t2 = t1;

    std::println("t2.x = {}", t2.x);
}

void process(Duration d) {
    std::println("处理时长: {} 秒", d.total_seconds());
}

void part5_explicit() {
    print_subtitle("Part 5: explicit");

    process(Duration(10));

    std::println("explicit 让意图更清晰, 编译器会阻止意外的隐式转换");
}

void part6_destructor() {
    print_subtitle("Part 6: 析构函数");

    std::println("进入作用域");
    {
        Logger log("session_1");
    }

    std::println("离开作用域(Logger 自动解析)");
}

int main() {
    print_header("02 构造函数与析构函数");
    part1_constructor_basics();
    part2_init_list();
    part3_delegating_ctor();
    part4_default_delete();
    part5_explicit();
    part6_destructor();
    print_separator();
    std::println("02_ctor_dtor 完成!");
    return 0;
}