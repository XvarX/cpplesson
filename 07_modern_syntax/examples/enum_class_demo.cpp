// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: enum class (限定作用域枚举) / 底层类型 / using enum / is_enum        ║
// ║  学习目标:                                                                  ║
// ║    1. 理解 enum class 相比普通 enum 的优势                                  ║
// ║    2. 掌握底层类型指定 (: int / : uint8_t)                                  ║
// ║    3. 学会 using enum (C++20) 引入枚举值到当前作用域                        ║
// ║    4. 了解 std::is_enum 类型萃取                                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/enum_class.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <string>
#include <type_traits> // std::is_enum_v, std::underlying_type_t
#include <utility>     // std::to_underlying (C++23)
#include <cstdint>     // uint8_t

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: 为什么需要 enum class                                              ║
// ║  WHAT: enum class (限定作用域枚举) 将枚举值放在类作用域内                   ║
// ║  WHY:  传统 enum 的三大痛点:                                               ║
// ║        ① 命名污染 —— 所有枚举值泄漏到外层作用域,容易冲突                   ║
// ║        ② 隐式转 int —— 可以无意中与整数运算, 违背类型安全                 ║
// ║        ③ 不可前向声明 —— 编译器不知道大小, 无法在头文件中声明              ║
// ║  WHEN: 任何时候定义枚举都应该优先使用 enum class                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_why_enum_class() {
    lesson::print_header("Part 1: enum class vs 传统 enum");

    // ── 1.1 传统 enum 的问题 ──
    {
        lesson::print_subtitle("1.1  传统 enum 的命名污染");
        // 传统 enum: 值泄漏到外层
        enum OldColor { RED, GREEN, BLUE };
        // 现在 RED/GREEN/BLUE 在外层可见 —— 不能定义同名变量
        // int RED = 5;              // 编译错误: RED 已经定义!
        int color = RED;            // 隐式转为 int, color = 0
        std::println("color = {}  (RED 隐式转成了整数!)", color);

        lesson::print_warn("传统 enum 值泄漏到外层 + 隐式转 int = 危险");
    }

    // ── 1.2 enum class 的解决方案 ──
    {
        lesson::print_subtitle("1.2  enum class 限定作用域");
        enum class NewColor { Red, Green, Blue };
        // NewColor::Red 在 NewColor 作用域内, 不会污染外层
        int Red = 5;                // OK! 没有冲突
        auto nc = NewColor::Green;

        // 强类型: 不会隐式转为 int
        std::println("Red={}", Red);
        // std::println("nc={}", nc); // 编译错误! enum class 不能隐式转 int

        // 需要显式转换
        std::println("nc 的整数表示 = {}",
                     static_cast<int>(nc));      // 输出: 1
        std::println("nc 的整数表示(底层) = {}",
                     std::to_underlying(nc));    // C++23 更简洁
    }

    // ── 1.3 前向声明对比 ──
    {
        lesson::print_subtitle("1.3  前向声明");
        // 传统 enum 不能前向声明 (除非指定底层类型, C++11 起才支持)
        // enum OldFlag;            // 错误! 传统 enum 不能前向声明

        // enum class 默认底层类型是 int, 天然支持前向声明
        enum class Flag : uint8_t;  // OK: 前向声明, 指定底层类型
        // ... 在另一个文件中定义
        enum class Flag : uint8_t { None = 0, Read = 1, Write = 2, Exec = 4 };
        lesson::print_note("enum class 天然支持前向声明 —— 解耦头文件依赖");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 底层类型指定 (: type)                                              ║
// ║  WHAT: enum class Name : UnderlyingType { ... }                             ║
// ║  WHY:  ① 精确控制内存布局(如网络协议/文件格式)                              ║
// ║        ② 保证 ABI 兼容 (不同平台可能有不同的默认大小)                      ║
// ║        ③ 前向声明时必须指定底层类型                                        ║
// ║  WHEN: 二进制协议、嵌入式设备、跨平台 API                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_underlying_type() {
    lesson::print_header("Part 2: 底层类型指定");

    using namespace modern_syntax;

    // ── 2.1 指定底层类型 ──
    {
        lesson::print_subtitle("2.1  指定底层类型控制内存");
        // sizeof(HttpStatus) == sizeof(uint16_t) == 2 字节
        std::println("HttpStatus 类型大小: {} 字节", sizeof(HttpStatus));

        // 不指定的话, 编译器默认用 int (4字节)
        enum class Default { A, B, C };
        std::println("默认 enum class 大小: {} 字节", sizeof(Default));
    }

    // ── 2.2 获取底层类型 ──
    {
        lesson::print_subtitle("2.2  std::underlying_type 获取底层类型");

        // 用 type traits 查询底层类型
        using UT = std::underlying_type_t<Priority>;   // UT = uint8_t
        static_assert(std::is_same_v<UT, uint8_t>);

        // 转换为底层整数值
        auto val = static_cast<UT>(Priority::High);
        std::println("Priority::High 的数值 = {}", val); // 输出: 255
    }

    // ── 2.3 位掩码标志 (flags) 模式 ──
    {
        lesson::print_subtitle("2.3  位掩码标志模式");
        // C++ 需要重载按位运算符 (enum class 不隐式转为整数)
        auto perm = static_cast<uint8_t>(Permission::Read)
                  | static_cast<uint8_t>(Permission::Write);

        bool can_read  = perm & static_cast<uint8_t>(Permission::Read);
        bool can_exec  = perm & static_cast<uint8_t>(Permission::Execute);
        std::println("可读: {}  可执行: {}", can_read, can_exec);  // true, false
        lesson::print_note("位掩码: 需要显式 static_cast (可写 operator| 简化)");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: using enum (C++20)                                                ║
// ║  WHAT: using enum EnumName; —— 将枚举值引入当前作用域                      ║
// ║  WHY:  省去冗长的 EnumName:: 前缀; 特别是在 switch 语句中                  ║
// ║  WHEN: 函数体内大量使用同一枚举值; 嵌套命名空间中的简洁写法                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_using_enum() {
    lesson::print_header("Part 3: using enum (C++20)");

    using namespace modern_syntax;

    // ── 3.1 在函数体内引入 ──
    {
        lesson::print_subtitle("3.1  函数体内 using enum");

        using enum Season;  // 把 Spring/Summer/Autumn/Winter 引入当前作用域
        auto s = Summer;    // 不需要写 Season::Summer
        std::println("当前季节: {}", static_cast<int>(s));

        // switch 里的清爽写法
        switch (s) {
        case Spring: std::println("春暖花开"); break;
        case Summer: std::println("夏日炎炎"); break;
        case Autumn: std::println("秋高气爽"); break;
        case Winter: std::println("冬雪皑皑"); break;
        }
    }

    // ── 3.2 在命名空间/类作用域中引入 ──
    {
        lesson::print_subtitle("3.2  类作用域中引入");
        // using enum 也可以出现在 namespace / class 中
        // namespace game {
        //     using enum Season;   // 该命名空间内所有代码都能直接用
        // }
        lesson::print_note("using enum 可出现在 namespace/class/function 作用域");
    }

    // ── 3.3 作用域对比 ──
    {
        lesson::print_subtitle("3.3  作用域对比演示");
        // 外层使用限定名
        auto s1 = Season::Winter;
        {
            using enum Season;      // 内层作用域引入
            auto s2 = Spring;       // OK: 可以直接写
            // s2 的作用域结束后, using enum 的效果也随之结束
            std::println("内层 s2 = {}", static_cast<int>(s2));
        }
        // 回到外层: Season::Spring 需要限定名
        std::println("外层 s1 = {}", static_cast<int>(s1));
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: std::is_enum 与枚举相关的类型萃取                                 ║
// ║  WHAT: std::is_enum<T> 判断类型 T 是否为枚举; 还有 is_scoped_enum (C++23)  ║
// ║  WHY:  模板元编程中根据类型是否为枚举来分支处理                             ║
// ║  WHEN: 泛型序列化/反序列化; 命令行解析; 配置读取                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_is_enum() {
    lesson::print_header("Part 4: std::is_enum 与类型萃取");

    using namespace modern_syntax;

    enum class Color { Red, Green, Blue };
    enum OldStyle { A, B, C };

    // ── 4.1 基本检查 ──
    {
        lesson::print_subtitle("4.1  std::is_enum_v 判断");
        std::println("Color   是枚举? {}", std::is_enum_v<Color>);     // true
        std::println("OldStyle 是枚举? {}", std::is_enum_v<OldStyle>); // true
        std::println("int      是枚举? {}", std::is_enum_v<int>);      // false
        std::println("string   是枚举? {}", std::is_enum_v<std::string>); // false
    }

    // ── 4.2 搭配 if constexpr 泛型处理 ──
    {
        lesson::print_subtitle("4.2  if constexpr + is_enum 泛型转换");

        std::println("Color::Blue → {}", to_number(Color::Blue)); // 输出: 2
        std::println("42         → {}", to_number(42));            // 输出: 42
        std::println("3.14       → {}", to_number(3.14));          // 输出: 3.14
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: enum class 不能隐式转为 int
    {
        lesson::print_subtitle("陷阱1: 忘记显式转换");
        enum class Level { Low, Mid, High };
        Level l = Level::High;
        // int idx = l;          // 错误!
        int idx = static_cast<int>(l);  // 正确
        std::println("idx = {}", idx);
        lesson::print_warn("enum class 不能隐式转 int —— 必须显式 static_cast");
    }

    // 陷阱2: using enum 与已有名称冲突
    {
        lesson::print_subtitle("陷阱2: using enum 引入的名称冲突");
        enum class Dir { Up, Down, Left, Right };
        int Up = 10;            // 外层有同名变量
        // {
        //     using enum Dir;   // 错误! Up 与 int Up 冲突
        //     auto d = Up;
        // }
        // 解决方案: 不冲突时用 using enum, 否则保持限定名
        auto d = Dir::Up;       // 始终安全的写法
        std::println("d = {}", static_cast<int>(d));
        lesson::print_warn("using enum 引入的名称会与外层同名变量冲突");
    }

    // 陷阱3: 不同 enum class 的值不能直接比较
    {
        lesson::print_subtitle("陷阱3: 不同类型枚举比较");
        enum class A { X = 1 };
        enum class B { Y = 1 };
        // if (A::X == B::Y) {} // 错误! 不同类型
        if (static_cast<int>(A::X) == static_cast<int>(B::Y)) { // OK
            std::println("转换后对比: A::X == B::Y");
        }
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 定义一个 HTTP 方法枚举 (GET, POST, PUT, DELETE)
    //        底层类型 uint8_t，写一个函数把枚举转成字符串
    lesson::print_subtitle("练习1: HTTP 方法枚举");
    // 提示: 参考 modern_syntax::HttpMethod

    // 练习2: 用 using enum 简化 switch 语句，打印中文季节
    lesson::print_subtitle("练习2: using enum 简化 switch");
    // 提示: 参考 modern_syntax::Season

    // 练习3: 写一个模板函数，传入枚举值时自动转为整数，非枚举直接返回
    lesson::print_subtitle("练习3: is_enum + if constexpr 泛型处理");
    // 提示: 参考 modern_syntax::to_number
}

int main() {
    part1_why_enum_class();
    part2_underlying_type();
    part3_using_enum();
    part4_is_enum();

    pitfalls();
    exercises();
    return 0;
}
