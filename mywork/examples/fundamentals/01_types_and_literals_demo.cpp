#include <print>
#include <string>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>

#include "shared/lesson_utils.hpp"

#include "fundamentals/type_literals.hpp"

int main() {
    lesson::print_header("01 基本类型、字面量与类型工具");

    lesson::print_subtitle("Part 1: 基本算术类型");

    bool flag = true;
    char ch = 'A';
    short small = 42;
    int answer = 100;
    long big = 100000L;
    long long huge = 9876543210LL;
    float pi_f = 3.14159265F;
    double pi_d = 3.141592653589793;

    std::println("bool: {}({} bytes)", flag, sizeof(flag));
    std::println("char: {}({} bytes)", ch, sizeof(ch));
    std::println("short: {}({} bytes)", small, sizeof(small));
    std::println("int: {}({} bytes)", answer, sizeof(answer));
    std::println("long: {}({} bytes)", big, sizeof(big));
    std::println("long long: {}({} bytes)", huge, sizeof(answer));
    std::println("float: {}({} bytes)", pi_f, sizeof(pi_f));
    std::println("double: {}({} bytes)", pi_d, sizeof(pi_d));

    std::println("\n编译器类型大小常量（来自 fundamentals 库:");
    std::println("  fundamentals::kSizeInt    = {}", fundamentals::kSizeInt);
    std::println("  fundamentals::kSizeDouble    = {}", fundamentals::kSizeDouble);
    std::println("  fundamentals::kIntMax    = {}", fundamentals::kIntMax);

    lesson::print_subtitle("Part 2: 字面量");

    int dec = 255;
    int bin = 0b11111111;
    int oct = 0377;
    int hex = 0xFF;

    int readable = 1'000'000;

    std::println("十进制:   {}", dec);
    std::println("二进制:   {}", bin);
    std::println("八进制:   {}", oct);
    std::println("十六进制: {}", hex);
    std::println("分隔符:   {}", readable);

    auto f1 = 3.14f;
    auto f2 = 3.14;
    auto f3 = 3.14L;
    auto sci = 6.022e23;

    std::println("float:    {}", f1);
    std::println("double:   {}", f2);
    std::println("科学技术: {}", sci);

    char c1 = 'A';
    const char* str = "Hello";
    std::string cpp_str = "Modern C++";

    auto raw = R"(路径:C:\Users\Ecks\demo)";
    std::println("普通字符串: {}", str);
    std::println("c++ 字符串: {}", cpp_str);
    std::println("原始字符串: {}", raw);
    
    lesson::print_subtitle("Part 3: auto 类型推导");

    auto n = 42;
    auto pi = 3.14159;
    auto pif = 3.14159f;
    auto s = std::string("C++23自动推导");

    static_assert(std::is_same_v<decltype(n), int>);
    static_assert(std::is_same_v<decltype(pi), double>);
    static_assert(std::is_same_v<decltype(pif), float>);

    std::println("auto n -> int: {}", n);
    std::println("auto pi -> double: {}", pi);
    std::println("auto pif -> float: {}", pif);
    std::println("auto s -> string: {}", s);

    lesson::print_subtitle("Part 4: sizeof 查询");


    std::println("sizeof(int)    = {}", sizeof(int));
    std::println("sizeof(double) = {}", sizeof(double));
    std::println("sizeof(answer) = {}", sizeof(answer));

    int* ptr = nullptr;
    std::println("sizeof(*ptr)   = {}", sizeof(*ptr));

    lesson::print_subtitle("Part 5: 定长类型");

    int32_t exact_int = 1'000'000'000;
    uint64_t big_id = 18'446'744'073'709'551'615ULL;

    std::println("int32_t: {} ({} bytes)", exact_int, sizeof(int32_t));
    std::println("uint64_t: {} ({} bytes)", big_id, sizeof(uint64_t));

    lesson::print_subtitle("Part 6: 类型转换");

    double d = 10;
    std::println("隐式 int->double: {}", d);

    int a = 10, b = 4;
    double ratio = static_cast<double>(a) / b;
    std::println("显示转换 10/4 = {}", ratio);

    int truncated = static_cast<int>(3.99);
    std::println("窄化 double->int (3.99): {} (小数被截断!)", truncated);

    lesson::print_subtitle("Part 7: numeric_limits 查询");

    std::println("int 最大值: {}", std::numeric_limits<int>::max());
    std::println("int 最小值: {}", std::numeric_limits<int>::min());
    std::println("double 最低整数: {}", std::numeric_limits<double>::min());
    std::println("double 最大值: {}", std::numeric_limits<double>::max());


    std::println("10进制: {}", 1024);
    std::println("16进制: {}", 0x400);
    std::println("8进制:{}", 02000);
    std::println("二进制:{}", 0b10000000000);
    std::println("10进制带分割号: {}", 1'024);

    std::println("int 长度: {}", sizeof(int));
    std::println("double 长度: {}", sizeof(double));
    std::println("long 长度: {}", sizeof(long));
    std::println("long long 长度: {}", sizeof(long long));


    std::println("0.1+0.2 = {}", static_cast<int>(0.1+0.2));

    std::println("long long 最大值: {}", std::numeric_limits<unsigned long long>::max());

    auto f = 3.14f;
    std::println("f is float : {}", std::is_same_v<decltype(f), float>);

}