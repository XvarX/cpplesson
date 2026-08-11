// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  06_streams_and_io :: 01_iostream_basics_demo                                ║
// ║  学习目标:                                                                   ║
// ║    1. 掌握 cin/cout/cerr/clog 四种标准流                                     ║
// ║    2. 理解流状态 (good/fail/eof/bad) 及其处理方式 (clear/ignore)              ║
// ║    3. 使用 getline 安全读取整行                                               ║
// ║    4. 使用 I/O 操纵符控制输出格式 (boolalpha/hex/oct/setw/setprecision)       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <iostream>
#include <sstream>     // 仅用于演示, 实际 stringstream 将在 03 详解
#include <iomanip>     // setw, setprecision 等操纵符
#include <string>
#include <limits>      // numeric_limits
#include "shared/lesson_utils.hpp"
#include "streams/stream_utils.hpp"   // 我们的流辅助工具库

int main() {
    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 四大标准流 — 它们是什么?
    // ═══════════════════════════════════════════════════════════════════════════
    // C++ 程序启动时, 系统自动打开 4 个标准流对象:
    //   cin   — 标准输入 (键盘),  对应 C 的 stdin
    //   cout  — 标准输出 (屏幕),  对应 C 的 stdout
    //   cerr  — 标准错误 (屏幕),  无缓冲,  立即输出 — 适合紧急错误信息
    //   clog  — 标准日志 (屏幕),  有缓冲,  适合普通日志 — 性能更好
    // WHY: 把输入/输出/错误分开, 使得运行程序时可以分别重定向
    lesson::print_header("06.1 — iostream 基础: cin/cout/cerr/clog");

    lesson::print_subtitle("Part 1: 四大标准流");
    std::println("cout → 普通输出 (带缓冲, 累积到一定量才写入屏幕)");
    std::println("cerr → 错误输出 (无缓冲, 立刻写出, 适合诊断紧急错误)");
    std::println("clog → 日志输出 (带缓冲, 适合写入大量日志, 性能优于 cerr)");
    std::cout << "\n💡 试试: 把程序输出重定向到文件, cerr/clog 仍会显示在屏幕上\n"
              << "   ./program > out.txt        ← cout 进文件, cerr/clog 仍到屏幕\n"
              << "   ./program 2> err.txt      ← cerr 进文件\n";

    // 演示 cerr vs clog: cerr 无缓冲 = 立即输出; clog 带缓冲 = 攒够了才输出
    std::cout << "[cout 消息] 程序启动中...\n";
    std::clog << "[clog 消息] 这是一条日志 (默认显示在屏幕, 但可能比 cout 晚)\n";
    std::cerr << "[cerr 消息] 出现严重问题! (无缓冲, 立刻显示)\n";

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 流状态 — fail/eof/bad/good
    // ═══════════════════════════════════════════════════════════════════════════
    // 每个流对象维护一个内部状态, 有 4 个标志位:
    //   goodbit → 一切正常 (值为 0)
    //   eofbit  → 已读到文件/流末尾 (尝试读但没数据了)
    //   failbit → 操作失败, 但流对象本身正常 (如: 想读整数却遇到字母)
    //   badbit  → 流本身损坏 (如: 磁盘硬件错误), 不可恢复
    // WHY: 这些状态让你知道为什么读取失败了, 从而做出不同处理
    // WHEN: 任何涉及 cin/文件读取的操作后, 都应该检查流状态
    lesson::print_subtitle("Part 2: 流状态");

    // 演示 failbit: 输入类型不匹配
    // 注意: 此示例使用 istringstream 模拟用户输入, 避免交互阻塞
    {
        std::println("--- 模拟: 用户输入了一个字母而不是数字 ---");
        std::istringstream fake_input("hello");  // 模拟用户输入 "hello"
        int number = 0;
        fake_input >> number;  // 尝试读整数, 但 "hello" 不是整数 → failbit 被置位

        if (fake_input.fail()) {
            std::println("  [检测] 读取失败! fail() = true");
            std::println("  原因: 输入 'hello' 无法转换为整数");
            // 使用 streams::reset_stream() — 我们封装的辅助函数
            // 等价于: fake_input.clear() + fake_input.ignore(...)
            streams::reset_stream(fake_input);
            std::println("  处理: 调用了 streams::reset_stream() 重置流状态");
        }
    }
    // 演示 eofbit: 读到末尾
    {
        std::println("\n--- 模拟: 读取到数据末尾 ---");
        std::istringstream fake_input("42");
        int val;
        fake_input >> val;  // 成功读取 42
        std::println("  读取到: {}", val);
        // 此时 pos 已到末尾, 但 eofbit 未必置位 — eof 需要"尝试读但没读到"
        // 再尝试读一次, 就会触发 eof
        fake_input >> val;  // 没数据了
        if (fake_input.eof()) std::println("  [检测] 到达末尾! eof() = true");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: getline — 安全读取整行
    // ═══════════════════════════════════════════════════════════════════════════
    // cin >> 以空白字符 (空格/换行) 为分隔, 无法读取包含空格的字符串
    // std::getline(cin, str) 读取一整行 (到换行符为止, 换行符本身被丢弃)
    // WHY: 用户输入的名字/地址/备注都可能包含空格
    // WHEN: 任何需要读取"一行文本"的场景, 都应该用 getline 而不是 cin >>
    lesson::print_subtitle("Part 3: getline 安全读行");

    {
        std::istringstream fake_input("艾克斯\nD:\\\\projects\\\\mycode\n");  // 模拟两行输入
        std::string name, path;
        std::getline(fake_input, name);  // 读取整行 (包含中文 OK)
        std::getline(fake_input, path);  // 读取带反斜杠的路径
        std::println("  姓名: {}", name);
        std::println("  路径: {}", path);
    }

    // getline 的第三个参数: 自定义分隔符
    {
        std::println("\n--- 用自定义分隔符解析 ---");
        std::istringstream csv("北京,上海,广州,深圳");
        std::string city;
        // 用逗号作为分隔符, 逐个提取城市
        while (std::getline(csv, city, ',')) {
            std::println("  城市: {}", city);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: I/O 操纵符 — 格式化输出
    // ═══════════════════════════════════════════════════════════════════════════
    // 操纵符是放在 << 链中修改输出格式的函数/对象, 需要 #include <iomanip>
    //   boolalpha  → true/false 而不是 1/0
    //   hex/oct/dec → 十六/八/十进制
    //   setw(n)    → 下一个输出占 n 个宽度 (右对齐)
    //   setprecision(n) → 浮点数有效位数 (或小数位数, 取决于 fixed/scientific)
    //   fixed      → 固定小数点表示
    // WHY: 让输出对齐和可读, 不用手写格式化逻辑
    // WHEN: 打印表格、日志、报告时
    lesson::print_subtitle("Part 4: 操纵符");

    std::println("boolalpha: {} vs {}", true, false);  // std::println 默认用 true/false
    // 用 cout + 操纵符演示 (C++23 的 println 已内置格式化, 操纵符是较旧但兼容的方式)
    std::cout << "boolalpha: " << std::boolalpha << true << " " << false
              << std::noboolalpha << "  (noboolalpha: " << true << ")\n";
    std::cout << "hex: 255 = " << std::hex << 255 << std::dec << "\n";
    std::cout << "oct: 255 = " << std::oct << 255 << std::dec << "\n";

    // setw + setprecision 制作对齐表格
    std::cout << "\n--- 成绩表 (setw + setprecision) ---\n";
    std::cout << std::setw(10) << "姓名" << std::setw(8) << "分数" << std::setw(10) << "百分比\n";
    std::cout << std::setw(10) << "艾克斯" << std::setw(8) << 95
              << std::setw(10) << std::fixed << std::setprecision(1) << 95.0/100*100 << "%\n";
    std::cout << std::setw(10) << "小明" << std::setw(8) << 87
              << std::setw(10) << 87.0/100*100 << "%\n";

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: std::println 的现代格式化 (替换操纵符)
    // ═══════════════════════════════════════════════════════════════════════════
    // C++23 的 std::println 使用 std::format 语法, 比操纵符更直观:
    //   {:b} 二进制  {:x} 十六进制  {:o} 八进制
    //   在 04_format_and_print_demo 中会详细讲解
    lesson::print_subtitle("Part 5: println 格式化 (预览)");
    std::println("  bool:    {} / {}", true, false);
    std::println("  hex:     255 = {:x}", 255);
    std::println("  oct:     255 = {:o}", 255);
    std::println("  bin:     255 = {:b}", 255);       // C++23
    std::println("  定点:    {:.2f}", 3.14159265);     // 保留 2 位小数

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("陷阱 1: cin >> 后残留换行符, 导致后续 getline 读到空行");
    std::println("  → 解决: 在 cin >> 后用 streams::skip_whitespace() 或 cin.ignore()");
    std::println("陷阱 2: 只检查 eof 而不检查 fail — 文件可能因格式错误而中断");
    std::println("  → 解决: 循环条件写成 while (stream >> val) 而不是 while (!stream.eof())");
    std::println("陷阱 3: 忘记调 clear() — 一旦 failbit 被置位, 后续所有操作都无效");
    std::println("  → 解决: 处理错误后记得 streams::reset_stream() 或 stream.clear()");
    std::println("陷阱 4: setw 只对'下一个'输出有效, 不是持久的");
    std::println("  → 解决: 每次需要宽度时都重新写 setw(n)");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("1. 写一个程序: 从 cin 读取用户的姓名和年龄, 若年龄不是数字则提示重新输入");
    std::println("2. 将上面的程序输出一个对齐的表格: 姓名 | 年龄");
    std::println("3. 尝试用不同进制 (dec/hex/oct) 打印同一个数字, 观察输出");
    std::println("4. 用 while(cin >> n) 读取一串数字直到输入非数字, 然后打印总和");

    return 0;
}
