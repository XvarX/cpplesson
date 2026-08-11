// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  06_streams_and_io :: 04_format_and_print_demo                               ║
// ║  学习目标:                                                                   ║
// ║    1. 使用 std::format (C++20) 进行类型安全的字符串格式化                      ║
// ║    2. 使用 std::print/std::println (C++23) 直接输出                          ║
// ║    3. 掌握格式说明符: 宽度、精度、对齐、填充、数值进制                          ║
// ║    4. 为自定义类型实现 std::formatter 特化                                    ║
// ║    5. 理解编译期格式字符串检查的优势                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>       // C++23: std::print, std::println
#include <format>      // C++20: std::format, std::formatter, std::format_to
#include <string>
#include <chrono>      // 演示时间格式化
#include <ctime>       // time_t, localtime_s / ctime
#include <sstream>
#include "shared/lesson_utils.hpp"
#include "streams/point3d.hpp"    // Point3D + 自定义 formatter 特化

// 注意: 需要 GCC 14+ / Clang 18+ / MSVC 2022 17.10+ 才能完整支持 std::print
// 如果编译不通过, 检查编译器版本!
static_assert(__cplusplus >= 202302L, "需要 C++23 编译器支持");

int main() {
    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: std::format — C++20 的格式化基石
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: std::format(fmt, args...) 根据格式字符串生成 std::string
    // 它类似于 Python 的 f-string 或 C 的 snprintf, 但:
    //   - 类型安全 (编译期检查参数类型和数量是否匹配)
    //   - 不需要指定 %d/%s/%f (自动推导类型)
    //   - 支持自定义类型的格式化
    //   - 不会缓冲区溢出
    // WHY: 比 stringstream 简洁, 比 snprintf 安全, 比运算符拼接直观
    // WHEN: 任何需要 "把值嵌入字符串" 的场景 — 日志、消息、报告、序列化
    lesson::print_header("06.4 — 格式化: std::format + std::print + 自定义 formatter");

    lesson::print_subtitle("Part 1: std::format 基础");

    // 基本用法: {} 是占位符, 按位置替换
    std::string s1 = std::format("姓名: {}, 年龄: {}", "艾克斯", 25);
    std::println("{}", s1);

    // 位置索引: {0} {1} 可以重复使用、改变顺序
    std::println("位置索引: {1} 在 {0} 之后, {0} 又出现了", "A", "B");

    // 格式说明符: {:spec}   spec 控制宽度/精度/对齐等
    // 语法: {[index]:[fill][align][width][.precision][type]}
    //   fill  - 填充字符 (默认空格)
    //   align - < 左对齐, > 右对齐, ^ 居中
    //   width - 最小宽度
    //   .prec - 浮点小数位数 或 字符串最大长度
    //   type  - 类型表示: d(十进制), x(十六进制), b(二进制), f(固定小数点)...
    std::println("默认:     |{}|", 42);
    std::println("宽度 6:   |{:6}|", 42);      // 默认右对齐
    std::println("左对齐:   |{:<6}|", 42);     // < 左对齐
    std::println("居中:     |{:^6}|", 42);     // ^ 居中
    std::println("填零:     |{:06}|", 42);     // 0 填充
    std::println("hex:      |{:x}|", 255);
    std::println("bin:      |{:b}|", 255);

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: std::print / std::println — C++23 直接输出
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: std::print 把格式化结果直接写入 FILE* (默认 stdout), 不构造中间 string
    //     std::println 等价于 std::print + 自动追加 \n
    // WHY: 比 std::cout << std::format(...) 少一次字符串分配; 线程安全 (单次调用原子写入)
    // WHEN: 代替 std::cout 进行所有输出 (在支持 C++23 的前提下)
    lesson::print_subtitle("Part 2: std::print / std::println");

    std::println("println 自动换行: {} + {} = {}", 10, 20, 10 + 20);
    std::print("print 不换行... ");
    std::print("所以两句话在同一行\n");

    // 输出到 stderr: 第二个参数指定 FILE*
    std::println(std::cerr, "[stderr] 这是一条错误消息");

    // 对比: cout vs println 的线程安全性
    // cout << a << b;   // 两个 << 不是原子的, 多线程可能交错
    // println("{} {}", a, b);  // 原子写入, 多线程安全
    std::println("\n💡 println 优势: 单次调用原子写入, 多线程安全; cout<< 可能错行");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 格式说明符深度探索
    // ═══════════════════════════════════════════════════════════════════════════
    // 完整格式说明符结构:  [[fill]align][sign][#][0][width][.precision][L][type]
    // 本节重点演示最常用的组合
    lesson::print_subtitle("Part 3: 格式说明符深度探索");

    // ── 浮点数精度与表示 ──
    const double pi = 3.141592653589793;
    std::println("默认:      {}", pi);         // 3.141592653589793
    std::println(".2f:       {:.2f}", pi);      // 3.14   固定小数点, 2 位
    std::println(".6f:       {:.6f}", pi);      // 3.141593 固定, 6 位
    std::println(".2e:       {:.2e}", pi);      // 3.14e+00 科学计数法
    std::println("宽度 10.2f: |{:10.2f}|", pi); // |      3.14| 右对齐

    // ── 整数对齐表格 (实战: 打印数据报告) ──
    std::println("\n--- 数据报告 ---");
    std::println("{:<10} {:>6} {:>6} {:>8}", "姓名", "数量", "单价", "总价");
    std::println("{:-<30}", "");  // 分隔线: 用 '-' 填充
    std::println("{:<10} {:>6} {:>6} {:>8.2f}", "苹果", 10, 5, 50.0);
    std::println("{:<10} {:>6} {:>6} {:>8.2f}", "橙子", 20, 3, 60.0);
    std::println("{:<10} {:>6} {:>6} {:>8.2f}", "总计", 30, "", 110.0);

    // ── 二进制/八进制/十六进制 ──
    std::println("\n进制转换:");
    std::println("  十进制:     {:d}", 42);   // d = decimal
    std::println("  十六进制:   {:x}", 42);   // x = hex (小写)
    std::println("  大十六进制: {:X}", 42);   // X = hex (大写)
    std::println("  八进制:     {:o}", 42);   // o = octal
    std::println("  二进制:     {:b}", 42);   // b = binary (C++23)
    std::println("  带前缀:     {:#x} {:#o} {:#b}", 42, 42, 42);  // # = 显示前缀

    // ── 字符串截断与对齐 ──
    std::string long_str = "Hello, C++ World!";
    std::println("截断 5 字符:  {:.5}", long_str);  // "Hello" — .precision 对字符串表示截断
    std::println("左对齐 20 宽: |{:<20}|", long_str);
    std::println("居中对齐 30:  |{:^30}|", long_str);

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 自定义 formatter — 让 format 支持你的类型
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: 特化 std::formatter<MyType> 实现 parse() 和 format()
    // WHY: 统一格式化语法 — 所有类型都用 {} 格式化, 不区分内置/自定义
    // WHEN: 你需要频繁打印自定义类型的值时; 或希望自定义类型用于日志框架
    // 注意: Point3D 和它的 formatter 特化定义在 include/streams/point3d.hpp 中
    lesson::print_subtitle("Part 4: 自定义 formatter (streams::Point3D)");

    streams::Point3D origin{0, 0, 0};
    streams::Point3D player{10.5, 20.3, 5.0};

    std::println("原点:   {}", origin);
    std::println("玩家位置: {}", player);

    // 演示 format_to: 不创建新 string, 直接写入已有 buffer
    char buf[64];
    auto result = std::format_to(buf, "位置: {}", player);
    *result = '\0';  // format_to 不自动加 null
    std::println("用 format_to 写入 buffer: {}", buf);

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 编译期格式检查 — 为什么比 printf 安全
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: std::format / std::print 的格式字符串在编译期被解析
    //     如果参数数量/类型与占位符不匹配 → 编译错误, 不是运行时崩溃
    // WHY: printf 这类 C 函数在运行时才检查格式, 写错了就 UB (未定义行为)
    //      格式字符串参数不匹配是 C/C++ 最常见的 bug 来源之一
    // WHEN: 总是! 从 C++20/23 开始, 永远使用 format/print, 告别 printf
    lesson::print_subtitle("Part 5: 编译期格式检查");

    // 以下代码如果取消注释, 会触发编译错误 (不是运行时错误!):
    // std::println("错误: {} 个参数但只有 {} 个占位符", 42);  // 编译错误! 占位符多, 参数少
    // std::println("{:d}", "hello");  // 编译错误! 类型不匹配: d 需要整数

    std::println("✅ 编译期检查保证: 格式字符串错误在编译期就被捕获!");
    std::println("   不会像 printf 那样悄悄崩掉或打印乱码");

    // 对比: printf 的经典问题
    // printf("%d", "hello");   // 编译通过! 运行结果是 UB
    // printf("%s %d", name);   // 少写一个参数, 编译通过, 运行读到栈上随机值

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 6: 综合实例 — 格式化日志函数
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 6: 综合 — 格式化日志");

    // 模拟一个简单的日志输出: 时间戳 + 级别 + 消息
    // C++20 支持 chrono 类型的格式化说明符 {:%Y-%m-%d %H:%M:%S}
    // 但编译器支持度不一; 这里用 C 的 ctime() 获取时间字符串以便演示
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    // ctime 返回的字符串自带换行, 用 stringstream 去掉
    std::string time_str = std::ctime(&t);
    time_str.pop_back();  // 去掉末尾换行

    std::println("[{}] [INFO]  服务启动完成", time_str);
    std::println("[{}] [DEBUG] 配置已加载, 共 {} 项", time_str, 12);
    std::println("[{}] [WARN]  内存使用率达到 {:.1f}%", time_str, 78.5);

    // 如果你的编译器完整支持 C++20 chrono 格式化, 可以这样写:
    // auto now = std::chrono::system_clock::now();
    // std::println("[{:%Y-%m-%d %H:%M:%S}] [INFO] 服务启动完成", now);

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("陷阱 1: 用 printf 而不是 format — 丧失了类型安全和编译期检查");
    std::println("  → 解决: 统一使用 std::format/std::print (C++20/23)");
    std::println("陷阱 2: 在 format 字符串中用了不支持的格式说明符");
    std::println("  → 解决: 查文档, 或让编译器报错 (编译期检查会帮你发现)");
    std::println("陷阱 3: 忘记 std::format_to 不自动加 null 终止符");
    std::println("  → 解决: 手动加 *result = '\\0', 或用 std::format_to_n");
    std::println("陷阱 4: 把 std::format 返回的 string 当作临时量, 却传了 c_str() 给 API");
    std::println("  → auto s = std::format(...); func(s.c_str());  ← OK, s 在作用域内有效");
    std::println("  → func(std::format(...).c_str());             ← 危险! 临时 string 已销毁");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("1. 用 std::format 制作一个九九乘法表, 每列对齐");
    std::println("2. 为 struct Color { uint8_t r,g,b; } 实现 std::formatter, 输出为 #RRGGBB 格式");
    std::println("3. 写一个函数, 用 std::print 输出一个带头部、分隔线、数据的表格");
    std::println("4. 对比 std::format + std::cout 与直接用 std::print 的性能 (打印 100000 行)");

    return 0;
}
