// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_fundamentals 示例 — 基本类型、字面量与类型工具                            ║
// ║  目标: 掌握 C++ 内置类型、字面量写法、auto/sizeof、定长类型、类型转换        ║
// ║  前置: 00_quickstart (环境搭建)                                               ║
// ║  对应库头文件: fundamentals/types_literals.hpp                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>        // C++23 std::print / std::println
#include <string>       // std::string
#include <cstdint>      // int32_t, uint64_t 等定长整数
#include <cstddef>      // std::size_t, std::ptrdiff_t
#include <limits>       // std::numeric_limits
#include <type_traits>  // std::is_same_v (类型比较)
#include "shared/lesson_utils.hpp"
#include "fundamentals/types_literals.hpp"

int main() {
    lesson::print_header("01 基本类型、字面量与类型工具");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 1: 基本算术类型 — 每种类型的"是什么、为什么、什么时候用"
    // ═══════════════════════════════════════════════════════════════════════════════
    // C++ 的内置类型直接映射到硬件，没有额外的运行时开销。这是 C++ "零开销抽象"
    // 哲学的基石。你写的 int 就是 CPU 寄存器里那个 int。
    //
    // 类型            大小(通常)    范围 (大致)              用途
    // bool            1 byte       true / false             条件判断、标志位
    // char            1 byte       -128 ~ 127 (或 0~255)    单个字符、字节操作
    // short           2 bytes      -32768 ~ 32767           节省内存的场景
    // int             4 bytes      -21亿 ~ 21亿             最常用的整数
    // long            4 或 8 bytes 平台相关                 兼容旧代码
    // long long       8 bytes      极大整数                  需要 64 位整数
    // float           4 bytes      约 7 位有效数字          图形、传感器数据(精度要求不高)
    // double          8 bytes      约 15 位有效数字         科学计算、高精度场景
    // wchar_t         2 或 4 bytes Unicode 代码点           宽字符 (Windows API 常用)
    // --------------------------------------------------------------------------------

    // 为什么存在这么多整数类型？ 历史原因 + 不同场景需要不同大小的整数。
    // 嵌入式系统可能想用 short 省内存，科学计算需要 long long 表示大数。
    lesson::print_subtitle("Part 1: 基本算术类型");

    bool  flag     = true;             // true / false，内存中至少占 1 字节
    char  ch       = 'A';              // 单引号，char 本质是 8 位整数
    short small    = 42;
    int   answer   = 100;
    long  big      = 100000L;          // L 后缀表示 long 字面量
    long long huge = 9876543210LL;     // LL 后缀表示 long long
    float pi_f     = 3.14159265f;      // f 后缀 → float (不加 f 默认是 double!)
    double pi_d    = 3.141592653589793; // 不加后缀的浮点字面量默认是 double

    std::println("bool:     {} ({} bytes)", flag, sizeof(flag));
    std::println("char:     {} ({} bytes)", ch, sizeof(ch));
    std::println("short:    {} ({} bytes)", small, sizeof(short));
    std::println("int:      {} ({} bytes)", answer, sizeof(int));
    std::println("long:     {} ({} bytes)", big, sizeof(long));
    std::println("long long: {} ({} bytes)", huge, sizeof(long long));
    std::println("float:    {:.7f} ({} bytes)", pi_f, sizeof(float));
    std::println("double:   {:.15f} ({} bytes)", pi_d, sizeof(double));

    // 使用库中的编译期类型大小常量 (定义在 types_literals.hpp)
    std::println("\n编译期类型大小常量 (来自 fundamentals 库):");
    std::println("  fundamentals::kSizeInt    = {}", fundamentals::kSizeInt);
    std::println("  fundamentals::kSizeDouble = {}", fundamentals::kSizeDouble);
    std::println("  fundamentals::kIntMax     = {}", fundamentals::kIntMax);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 2: 字面量 (Literals) — 数字、字符及其他表示法
    // ═══════════════════════════════════════════════════════════════════════════════
    // 字面量是直接写在代码里的常数值。C++ 支持多种进制和前缀，帮助你表达意图。
    // 为什么需要后缀？ 编译器需要知道字面量的确切类型，否则会按默认规则推导，
    // 可能导致精度丢失 (比如 3.14 默认是 double, 赋值给 float 时会有隐式转换)。
    lesson::print_subtitle("Part 2: 字面量");

    // ── 整数字面量 ──
    int dec  = 255;      // 十进制 (默认)
    int bin  = 0b11111111; // 0b 前缀 → 二进制 (C++14)
    int oct  = 0377;     // 0 前缀 → 八进制 (注意，容易误读!)
    int hex  = 0xFF;     // 0x 前缀 → 十六进制
    // 使用单引号分隔数字 (C++14)，不影响值，纯增强可读性
    int readable = 1'000'000;

    std::println("十进制:    {}", dec);
    std::println("二进制:    {}", bin);
    std::println("八进制:    {}", oct);
    std::println("十六进制:  {}", hex);
    std::println("分隔符:    {}", readable);

    // ── 浮点字面量后缀 ──
    auto f1 = 3.14f;     // float
    auto f2 = 3.14;      // double (默认!)
    auto f3 = 3.14L;     // long double
    // 科学计数法
    auto sci = 6.022e23; // 阿伏伽德罗常数

    std::println("float:       {}", f1);
    std::println("double:      {}", f2);
    std::println("科学计数:    {}", sci);

    // ── 字符与字符串字面量 ──
    char        c1 = 'A';                // 普通字符
    const char* str = "Hello";           // C 风格字符串 (const char[6])
    std::string cpp_str = "Modern C++";  // C++ 风格字符串
    // 原始字符串字面量 (C++11): R"(...)"，内部不需要转义反斜杠
    auto raw = R"(路径: C:\Users\Ecks\demo)";
    std::println("普通字符串: {}", str);
    std::println("C++ 字符串: {}", cpp_str);
    std::println("原始字符串: {}", raw);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 3: auto — 让编译器推导类型
    // ═══════════════════════════════════════════════════════════════════════════════
    // auto 告诉编译器: "你帮我决定类型"。这减少了冗长代码，避免手动写错类型。
    // 什么时候用？ 1) 类型太长 (如迭代器) 2) 泛型代码 3) 避免类型不匹配
    // 什么时候不用？ 1) 需要明确类型以提升可读性 2) 想要控制隐式转换时
    lesson::print_subtitle("Part 3: auto 类型推导");

    auto n   = 42;            // int
    auto pi  = 3.14159;       // double (默认)
    auto pif = 3.14159f;      // float (后缀起作用)
    auto s   = std::string{"C++23自动推导"};  // std::string

    // 验证 auto 推导的类型是否正确
    static_assert(std::is_same_v<decltype(n), int>);
    static_assert(std::is_same_v<decltype(pi), double>);
    static_assert(std::is_same_v<decltype(pif), float>);

    std::println("auto n   → int:    {}", n);
    std::println("auto pi  → double: {}", pi);
    std::println("auto pif → float:  {}", pif);
    std::println("auto s   → string: {}", s);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 4: sizeof — 查询类型/变量大小
    // ═══════════════════════════════════════════════════════════════════════════════
    // sizeof 是编译期运算符，返回类型或变量占用的字节数。它不评估表达式。
    // 为什么需要？ 1) 内存分配 2) 平台差异处理 3) 确保数据对齐
    lesson::print_subtitle("Part 4: sizeof 查询");

    // sizeof 的目标可以是一个类型，也可以是一个变量/表达式
    std::println("sizeof(int)       = {}", sizeof(int));
    std::println("sizeof(double)    = {}", sizeof(double));
    std::println("sizeof(answer)    = {}", sizeof(answer));  // answer 是 int
    // sizeof 不会真的执行表达式，所以下面这行安全:
    int* ptr = nullptr;
    std::println("sizeof(*ptr)      = {}", sizeof(*ptr));    // 只查类型，不会解引用 null

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 5: 定长类型 (Fixed-width integers) — 跨平台一致的大小
    // ═══════════════════════════════════════════════════════════════════════════════
    // C++ 标准不保证 int 永远是 4 字节。需要确定大小时，用 <cstdint> 里的类型:
    //   int8_t, int16_t, int32_t, int64_t    (有符号)
    //   uint8_t, uint16_t, uint32_t, uint64_t (无符号)
    // 什么时候用？ 网络协议、文件格式、嵌入式开发、跨平台数据序列化
    lesson::print_subtitle("Part 5: 定长类型");

    int32_t  exact_int = 1'000'000'000;   // 保证 32 位，有符号
    uint64_t big_id    = 18'446'744'073'709'551'615ULL; // 保证 64 位，无符号

    std::println("int32_t:  {} ({} bytes)", exact_int, sizeof(int32_t));
    std::println("uint64_t: {} ({} bytes)", big_id, sizeof(uint64_t));

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 6: 类型转换 — 隐式 vs 显式
    // ═══════════════════════════════════════════════════════════════════════════════
    // C++ 有四种显式转换和一个 C 风格转换。你应该优先用 C++ 的显式转换：
    //   static_cast  — 最常用，相关类型之间的转换 (int→double, 基类↔派生类)
    //   const_cast   — 去掉或添加 const 限定符 (极度谨慎使用!)
    //   reinterpret_cast — 完全无关类型间的按位重解释 (危险，几乎不用于普通代码)
    //   dynamic_cast — 运行时类型检查的向下转换 (多态场景，后面讲)
    // 隐式转换: 编译器自动做的，如 int→double、派生类指针→基类指针
    lesson::print_subtitle("Part 6: 类型转换");

    // ── 隐式转换 ──
    double d = 10;           // int → double (安全的，不丢失精度)
    std::println("隐式 int→double: {}", d);

    // ── 显式转换 (推荐方式) ──
    int a = 10, b = 4;
    double ratio = static_cast<double>(a) / b;  // 先把 a 转为 double
    std::println("显式转换 10/4 = {}", ratio);   // 期望输出 2.5

    // ── 窄化转换警告 ──
    // double → int 会截断小数部分，推荐用 static_cast 明确意图
    int truncated = static_cast<int>(3.99);
    std::println("窄化 double→int (3.99): {} (小数被截断!)", truncated);

    // ── C 风格转换 (不推荐) ──
    // int x = (int)3.14;  // 功能太强、太隐蔽，容易藏 bug

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 7: std::numeric_limits — 查询类型的数值极限
    // ═══════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 7: numeric_limits 查询");

    std::println("int 最大值: {}", std::numeric_limits<int>::max());
    std::println("int 最小值: {}", std::numeric_limits<int>::min());
    std::println("double 最低正数: {}", std::numeric_limits<double>::min());
    std::println("double 最大值:   {}", std::numeric_limits<double>::max());

    // ── 常见陷阱 (Common Pitfalls) ────────────────────────────────────────────
    lesson::print_separator("常见陷阱");
    lesson::print_warn("陷阱1: 整型溢出 — int 超出范围会回绕 (未定义行为的有符号溢出)");
    lesson::print_warn("陷阱2: 浮点字面量默认是 double — 3.14 不是 float! 用 3.14f 明确指定");
    lesson::print_warn("陷阱3: C 风格转换隐藏危险 — 全部改用 static_cast");
    lesson::print_warn("陷阱4: unsigned 与 signed 混用 — 比较时可能得到意外结果");
    lesson::print_warn("陷阱5: char 可能是有符号/无符号 — 取决于平台，用 int8_t/uint8_t 明确");
    lesson::print_warn("陷阱6: auto 会丢弃 const 和引用 — auto x = ref; 会拷贝!");

    // ── 练习 ─────────────────────────────────────────────────────────────────
    lesson::print_separator("练习");
    std::println("1. 写出 5 种不同进制的整数 1024 的字面量 (十进制/十六进制/八进制/二进制/带分隔符)");
    std::println("2. 用 sizeof 比较 short, int, long, long long 在你的平台上的大小");
    std::println("3. 练习 static_cast: 把 0.1+0.2 转为 int，观察截断结果");
    std::println("4. 用 std::numeric_limits 查询 unsigned long long 的最大值");
    std::println("5. 实验: 把 3.14f 赋值给 auto 变量，std::is_same_v 验证它是 float");

    return 0;
}
