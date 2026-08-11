// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: 用户定义字面量 (operator"") / std::to_underlying / 标准库字面量      ║
// ║  学习目标:                                                                  ║
// ║    1. 理解 operator"" 的语法与参数类型约束                                  ║
// ║    2. 掌握自定义 _km / _deg / _hex 等后缀的写法                            ║
// ║    3. 学会 std::to_underlying (C++23) 安全获取枚举底层值                   ║
// ║    4. 了解标准库字面量: chrono_literals / string_literals / string_view_literals║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/user_defined_literals.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <string>
#include <string_view>
#include <utility>       // std::to_underlying (C++23)
#include <chrono>        // std::chrono_literals: 1s, 1ms, 1h, 1min
#include <version>       // C++ 特性检测宏

// 只引入需要的字面量命名空间 (避免 using namespace std 污染)
using namespace std::chrono_literals;      // 1s, 1ms, 1us, 1ns, 1h, 1min
using namespace std::string_literals;       // "hello"s → std::string
using namespace std::string_view_literals;  // "hello"sv → std::string_view (C++17)

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: 自定义 _km / _mi 后缀 —— Distance 类型                            ║
// ║  WHAT: operator""_km(value) 返回 Distance 对象，内部统一以米存储             ║
// ║  WHY:  不同单位之间自动转换，编译期类型安全，避免 NASA 火星探测器事故       ║
// ║  WHEN: 物理仿真、地理信息系统(GIS)、运动控制、游戏引擎                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_custom_distance_udl() {
    lesson::print_header("Part 1: 自定义 _km / _mi 后缀");

    using namespace modern_syntax;

    // ── 1.1 浮点与整数 _km ──
    {
        lesson::print_subtitle("1.1  浮点与整数 _km 后缀");
        auto marathon = 42.195_km;     // 全程马拉松 ≈ 42195 米
        auto five_km  = 5_km;          // 5 公里

        std::println("马拉松距离: {:.3f} m  ({:.2f} km)",
                     marathon.meters, marathon.as_km());
        std::println("5 公里: {} m", five_km.meters);
        std::println("同一单位: 比较 {:.3f} m 和 {:.0f} m",
                     marathon.meters, five_km.meters);
    }

    // ── 1.2 英里 _mi 后缀 ──
    {
        lesson::print_subtitle("1.2  _mi (英里) 后缀 — 自动转米");
        auto mile_run = 1.0_mi;        // 1 英里 ≈ 1609.344 米
        auto three_mi = 3_mi;          // 3 英里

        std::println("1 英里 = {:.3f} m  ({:.4f} km)",
                     mile_run.meters, mile_run.as_km());
        std::println("3 英里 = {:.0f} m", three_mi.meters);
    }

    // ── 1.3 单位混用对比 ──
    {
        lesson::print_subtitle("1.3  单位混用对比");
        auto d1 = 5_km;                // 5 公里 → 5000 米
        auto d2 = 5_mi;                // 5 英里 → 8046.72 米
        std::println("5 km  → {:>10.2f} m", d1.meters);
        std::println("5 mi  → {:>10.2f} m", d2.meters);
        std::println("两者相差 {:.2f} m", d2.meters - d1.meters);
        lesson::print_note("UDL 让单位区分一目了然：5_km vs 5_mi");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 自定义 _deg / _rad 后缀 —— Angle 类型                            ║
// ║  WHAT: operator""_deg(value) / operator""_rad(value) 返回 Angle             ║
// ║  WHY:  角度制与弧度制混用是常见 bug 来源; UDL 在源头消灭歧义               ║
// ║  WHEN: 图形学(OpenGL/DirectX)、机器人运动学、信号处理、导航                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_custom_angle_udl() {
    lesson::print_header("Part 2: 自定义 _deg / _rad 后缀");

    using namespace modern_syntax;

    // ── 2.1 角度 _deg 后缀 ──
    {
        lesson::print_subtitle("2.1  _deg (度) 后缀 — 自动转弧度");
        auto right_angle = 90_deg;     // 90° → π/2 弧度
        auto half_circle = 180.0_deg;  // 180° → π 弧度
        auto full_circle = 360_deg;    // 360° → 2π 弧度

        std::println("90°    = {:.6f} rad  (回算: {:.1f}°)",
                     right_angle.radians, right_angle.degrees());
        std::println("180°   = {:.6f} rad  (回算: {:.1f}°)",
                     half_circle.radians, half_circle.degrees());
        std::println("360°   = {:.6f} rad  (回算: {:.1f}°)",
                     full_circle.radians, full_circle.degrees());
    }

    // ── 2.2 弧度 _rad 后缀 ──
    {
        lesson::print_subtitle("2.2  _rad (弧度) 后缀 — 透传存储");
        auto one_rad = 1.0_rad;       // 1 弧度 ≈ 57.3°
        auto pi_rad  = std::numbers::pi; // π 弧度 = 180°
        // 注: 没有 _rad 后缀的数字就只是普通浮点数

        std::println("1  rad        = {:.6f} rad  ({:.2f}°)",
                     one_rad.radians, one_rad.degrees());
        std::println("π (3.141593)  = 对应的角度 {:.1f}°",
                     (pi_rad * 180.0 / std::numbers::pi));
    }

    // ── 2.3 三角运算混用 ──
    {
        lesson::print_subtitle("2.3  sin(角度) 混用 —— UDL 防混淆");
        // 直接计算 sin 需要弧度，_deg 帮你完成了转换
        auto a = 30_deg;  // 内部已转为弧度
        std::println("sin(30°) = {:.6f}", std::sin(a.radians));  // ≈ 0.5
        std::println("cos(60°) = {:.6f}", std::cos(60_deg.radians)); // ≈ 0.5
        lesson::print_note("_deg 后缀确保传入 sin/cos 的是弧度，而非角度");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 自定义 _hex 后缀 + std::to_underlying (C++23)                     ║
// ║  WHAT: _hex 把 0xRRGGBB 转为 RGB 颜色；std::to_underlying 安全转枚举底层值 ║
// ║  WHY:  UDL 不仅是单位转换，还可以做数据解包、编码转换                        ║
// ║        std::to_underlying 比 static_cast<int>(e) 更安全更可读               ║
// ║  WHEN: 颜色值解析; 枚举序列化/日志; 枚举用作数组索引                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_hex_color_and_to_underlying() {
    lesson::print_header("Part 3: _hex 后缀 + std::to_underlying (C++23)");

    using namespace modern_syntax;

    // ── 3.1 _hex 颜色后缀 ──
    {
        lesson::print_subtitle("3.1  _hex 颜色后缀 (0xRRGGBB → RGB)");
        auto red    = 0xFF0000_hex;
        auto green  = 0x00FF00_hex;
        auto blue   = 0x0000FF_hex;
        auto orange = 0xFF6600_hex;  // 经典橙色

        std::println("Red:    R={:3d} G={:3d} B={:3d}", red.r, red.g, red.b);
        std::println("Green:  R={:3d} G={:3d} B={:3d}", green.r, green.g, green.b);
        std::println("Blue:   R={:3d} G={:3d} B={:3d}", blue.r, blue.g, blue.b);
        std::println("Orange: R={:3d} G={:3d} B={:3d}", orange.r, orange.g, orange.b);
        lesson::print_note("_hex 对整数参数做位运算拆解，而非单位转换");
    }

    // ── 3.2 std::to_underlying (C++23) ──
    {
        lesson::print_subtitle("3.2  std::to_underlying — 枚举 → 底层整数");
        OpCode op = OpCode::Add;

        // C++23 新方式 (推荐)
        auto val1 = std::to_underlying(op);
        std::println("OpCode::Add = {} (via to_underlying)", val1);

        // 旧方式 (C++11/14/17)
        auto val2 = static_cast<std::underlying_type_t<OpCode>>(op);
        auto val3 = static_cast<uint8_t>(op);
        std::println("旧方式1: {}  旧方式2: {}  (同样结果，但冗长且不安全)", val2, val3);

        // 所有操作码遍历
        std::println("\n所有 OpCode 的值:");
        for (auto code : {OpCode::Nop, OpCode::Load, OpCode::Store,
                          OpCode::Add, OpCode::Halt}) {
            std::println("  {:>5} = {:>3}",
                         "enum",    // 占位 (无法直接打印枚举名)
                         std::to_underlying(code));
        }
    }

    // ── 3.3 to_underlying 的错误捕获 ──
    {
        lesson::print_subtitle("3.3  to_underlying 类型安全对比");
        int plain_int = 42;
        // auto err = std::to_underlying(plain_int);  // 编译错误! int 不是枚举
        // 而 static_cast<uint8_t>(plain_int) 会静默截断 42 → 42
        std::println("普通 int 42 无法传入 to_underlying —— 编译器帮你挡住错误");
        lesson::print_warn("static_cast 允许任意类型转换; to_underlying 只接受枚举");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 标准库字面量 — chrono_literals                                     ║
// ║  WHAT: 1s / 1ms / 1us / 1ns / 1h / 1min 返回 std::chrono::duration         ║
// ║  WHY:  比 std::chrono::seconds{1} 可读性强 10 倍                            ║
// ║  WHEN: 定时器、超时、帧率控制、性能测试                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_chrono_literals() {
    lesson::print_header("Part 4: 标准库字面量 — chrono_literals");

    // ── 4.1 基本时间字面量 ──
    {
        lesson::print_subtitle("4.1  时间字面量: s / ms / us / ns / h / min");
        // using namespace std::chrono_literals 已在文件顶部引入

        auto wait  = 2s;       // std::chrono::seconds{2}
        auto tick  = 50ms;     // std::chrono::milliseconds{50}
        auto micro = 100us;    // std::chrono::microseconds{100}
        auto nano  = 500ns;    // std::chrono::nanoseconds{500}
        auto hour  = 1h;       // std::chrono::hours{1}
        auto mins  = 30min;    // std::chrono::minutes{30}

        std::println("等待: {} 秒",  wait.count());
        std::println("帧间隔:  {} 毫秒 (60fps ≈ 16.7ms)", tick.count());
        std::println("微秒精度: {} μs", micro.count());
        std::println("纳秒精度: {} ns", nano.count());
        std::println("会议时长: {} 小时 {} 分钟",
                     hour.count(), mins.count());
    }

    // ── 4.2 时间运算 — 字面量直接运算 ──
    {
        lesson::print_subtitle("4.2  时间运算: 字面量直接加减");
        auto total = 1h + 30min + 15s;  // 1小时30分15秒

        using namespace std::chrono;
        auto as_seconds = duration_cast<seconds>(total);
        std::println("1h + 30min + 15s = {} 秒", as_seconds.count());

        auto frame_time = 1000ms / 60;  // 60fps 每帧 ≈ 16.7ms
        std::println("1000ms / 60 = {:.2f}ms  (~60fps)",
                     1.0 * frame_time.count());
    }

    // ── 4.3 chrono::duration 组合 ──
    {
        lesson::print_subtitle("4.3  浮点时间字面量 (C++20)");
        // 浮点字面量: 1.5s 返回 duration<double>
        auto half = 1.5s;
        std::println("1.5s = {} 秒 (浮点 duration)", half.count());
        std::println("1.5s 转换为毫秒: {:.0f}ms", (1.5s * 1000).count());
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: 标准库字面量 — string_literals / string_view_literals             ║
// ║  WHAT: "abc"s → std::string; "abc"sv → std::string_view                     ║
// ║  WHY:  ① 告别 std::string("abc") 的啰嗦写法                                ║
// ║        ② auto 推导: auto s = "hello"s 得到 std::string 而非 const char*    ║
// ║        ③ sv 字面量零拷贝: 字符串视图不分配内存                              ║
// ║  WHEN: 需要 string 时用 s; 只读访问时用 sv; auto 推导时用                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_string_literals() {
    lesson::print_header("Part 5: 标准库字面量 — string_literals / string_view_literals");

    // ── 5.1 "hello"s —— 直接得到 std::string ──
    {
        lesson::print_subtitle("5.1  \"...\"s — 直接构造 std::string");
        // using namespace std::string_literals 已在文件顶部引入

        auto name = "艾克斯"s;              // std::string, 不是 const char*
        auto greeting = "你好, "s + name + "!"s;  // 字符串拼接

        std::println("greeting = {}", greeting);
        std::println("长度 = {}", greeting.length());

        // 对比: 没有 s 后缀时的麻烦
        const char* raw = "裸字符串";       // const char*
        std::string old_way(raw);           // 需要显式构造
        // "你好" + "世界";                // 编译错误! 不能两个指针相加
        // "你好"s + "世界"s;               // OK: 两个 string 可以相加
    }

    // ── 5.2 "abc"sv —— 零拷贝字符串视图 ──
    {
        lesson::print_subtitle("5.2  \"...\"sv — std::string_view 零拷贝");
        // using namespace std::string_view_literals 已在文件顶部引入

        auto sv  = "hello world"sv;          // string_view: 无内存分配!
        auto sub = sv.substr(0, 5);          // "hello"  零拷贝视图

        std::println("完整视图: {}", sv);
        std::println("子串: {}  (长度={})", sub, sub.length());
        std::println("sizeof(string_view) = {} 字节  (vs string 通常 32 字节)",
                     sizeof(std::string_view));
    }

    // ── 5.3 auto 推导陷阱 ──
    {
        lesson::print_subtitle("5.3  auto 推导对比: 有/无后缀的区别");
        auto a = "hello";       // auto → const char*
        auto b = "hello"s;      // auto → std::string
        auto c = "hello"sv;     // auto → std::string_view

        std::println("auto a = \"hello\"   → {}  (const char*)", typeid(a).name());
        std::println("auto b = \"hello\"s  → {}  (std::string)",  typeid(b).name());
        // 注: typeid().name() 的输出是编译器相关的, 此处仅演示类型不同
        lesson::print_note("\"hello\"  (裸字面量) → const char*");
        lesson::print_note("\"hello\"s (带后缀)   → std::string");
        lesson::print_note("\"hello\"sv(带后缀)   → std::string_view");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: 自定义 UDL 必须以 _ 开头
    {
        lesson::print_subtitle("陷阱1: 自定义 UDL 必须以 _ 开头");
        lesson::print_warn("operator\"\"km  → 编译错误! 标准保留无下划线前缀");
        lesson::print_note("operator\"\"_km → OK: _下划线是用户的保留空间");
    }

    // 陷阱2: UDL 参数类型限制
    {
        lesson::print_subtitle("陷阱2: UDL 参数类型限制严格");
        lesson::print_warn("浮点 UDL 参数必须是 long double, 不能是 float/double");
        lesson::print_warn("整数 UDL 参数必须是 unsigned long long, 不能是 int/size_t");
        lesson::print_note("字符串 UDL 参数: (const char*, size_t) 或 (const char*)");
        lesson::print_note("字符 UDL 参数: char");
    }

    // 陷阱3: using namespace 污染
    {
        lesson::print_subtitle("陷阱3: using namespace 只引入需要的");
        lesson::print_warn("推荐: using namespace std::chrono_literals;");
        lesson::print_warn("不推荐: using namespace std; (引入所有标准库字面量)");
    }

    // 陷阱4: 字面量运算符不能在类作用域内定义
    {
        lesson::print_subtitle("陷阱4: UDL 运算符不能是类成员");
        lesson::print_warn("UDL 必须在全局或命名空间作用域定义, class 内不行");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 实现 _cm 后缀 (厘米转米)
    lesson::print_subtitle("练习1: 实现 _cm 后缀");
    lesson::print_note("Distance operator\"\"_cm(long double cm) { return Distance{cm / 100.0}; }");

    // 练习2: 实现 _ft 后缀 (英尺转米, 1ft = 0.3048m)
    lesson::print_subtitle("练习2: 实现 _ft 后缀");
    lesson::print_note("Distance operator\"\"_ft(long double ft) { return Distance{ft * 0.3048}; }");

    // 练习3: 用 std::to_underlying 写枚举 → 字符串函数
    lesson::print_subtitle("练习3: to_underlying + format 枚举序列化");
    lesson::print_note("auto s = std::format(\"value={}\", std::to_underlying(e));");

    // 练习4: 使用 chrono_literals 模拟帧率控制
    lesson::print_subtitle("练习4: chrono_literals 模拟帧率");
    lesson::print_note("auto frame_time = 1000ms / 60;  // 60fps 每帧时间");
}

int main() {
    part1_custom_distance_udl();
    part2_custom_angle_udl();
    part3_hex_color_and_to_underlying();
    part4_chrono_literals();
    part5_string_literals();

    pitfalls();
    exercises();
    return 0;
}
