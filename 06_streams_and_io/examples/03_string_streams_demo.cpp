// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  06_streams_and_io :: 03_string_streams_demo                                 ║
// ║  学习目标:                                                                   ║
// ║    1. 用 ostringstream 将多种数据拼接成字符串                                 ║
// ║    2. 用 istringstream 从字符串中解析出不同类型的数据                         ║
// ║    3. 用 stringstream 实现读写一体、原地修改                                   ║
// ║    4. 掌握 str() 方法 — 获取/设置底层字符串                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <sstream>     // istringstream, ostringstream, stringstream
#include <iostream>    // 仅用于 cout 配合 lesson_utils
#include <iomanip>     // setprecision
#include <string>
#include <vector>
#include "shared/lesson_utils.hpp"
#include "streams/simple_json.hpp"    // Entry + JSON 构建 + CSV 解析

int main() {
    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: ostringstream — 把数据拼成字符串
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: ostringstream 是一个输出流, 但它把数据写到内存中的 std::string 而不是文件
    // 它支持所有 << 操作符, 就像 cout 和 ofstream 一样
    // WHY: 避免多次字符串拼接导致大量临时对象分配 (string::operator+ 每次创建新对象)
    //      对比: string a = s1 + " " + s2 + " ";  // 产生多个临时 string
    //            ostringstream oss; oss << s1 << " " << s2 << " ";  // 一次搞定
    // WHEN: 构造 SQL 语句、生成 HTML/JSON、格式化日志、拼接路径
    lesson::print_header("06.3 — 字符串流: 拼接、解析、原地修改");

    lesson::print_subtitle("Part 1: ostringstream 拼接");
    {
        std::ostringstream oss;   // 只写的字符串流
        oss << "name=" << "Ecks"
            << ", age=" << 25
            << ", pi=" << 3.14159;   // 注意: 浮点数默认 6 位有效数
        std::string result = oss.str();  // .str() 获取底层 string (拷贝)
        std::println("拼接结果: {}", result);

        // 注意: .str() 返回副本, 底层 buffer 不受影响
        // 可以继续写入
        oss << ", extra";
        std::println("继续写入后: {}", oss.str());

        // 清空并重新使用
        oss.str("");    // 清空底层 buffer
        oss.clear();    // 清除状态标志 (重要! 否则 failbit 会导致后续写入失败)
        oss << "全新的内容";
        std::println("清空后重写: {}", oss.str());
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: istringstream — 从字符串解析数据
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: istringstream 把字符串当作输入源, 像 cin 一样用 >> 从中提取数据
    // WHY: 不需要手写字符串分割和类型转换, 利用流的 >> 操作符自动完成
    // WHEN: 解析配置文件、CSV 数据、命令行参数、反序列化
    // 与 std::from_chars 的对比: sstream 更灵活 (支持各种类型),
    //   from_chars 更快 (不分配内存, 但只支持数值类型)
    lesson::print_subtitle("Part 2: istringstream 解析");

    {
        // 场景: 从一行 "配置" 字符串中提取多个字段
        std::string config = "width=1920 height=1080 rate=60.0 title=MainWindow";
        std::istringstream iss(config);
        // 逐 token 解析: >> 默认以空白分隔
        std::string token;
        while (iss >> token) {
            // 每个 token 是 "key=value" 的形式
            auto eq_pos = token.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = token.substr(0, eq_pos);
                std::string val = token.substr(eq_pos + 1);
                std::println("  {:>8}: {}", key, val);
            }
        }
    }

    {
        // 场景: 解析 CSV 行 — 使用 streams::parse_csv_line() 辅助函数
        std::println("\n--- CSV 解析 (使用 streams::parse_csv_line) ---");
        std::string csv_line = "2025-08-11,1001,Ecks,95.5";

        // 一行代码完成解析!
        auto fields = streams::parse_csv_line(csv_line);

        std::println("  日期: {}", fields.size() > 0 ? fields[0] : "N/A");
        std::println("  ID:   {}", fields.size() > 1 ? fields[1] : "N/A");
        std::println("  姓名: {}", fields.size() > 2 ? fields[2] : "N/A");
        std::println("  分数: {}", fields.size() > 3 ? fields[3] : "N/A");

        // 底层原理 (也可以手动写):
        // std::istringstream iss(csv_line);
        // std::string date, name;
        // int id;
        // double score;
        // char comma;
        // std::getline(iss, date, ',');
        // iss >> id >> comma;
        // std::getline(iss, name, ',');
        // iss >> score;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: stringstream — 读写一体
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: stringstream 同时支持读和写, 可以在同一块内存 buffer 中先写后读
    // WHY: 先构造字符串, 再从中解析; 或原地修改字符串内容
    // WHEN: 模板处理、简单的词法分析、单元测试中模拟 I/O
    lesson::print_subtitle("Part 3: stringstream 读写一体");

    {
        // 先写后读: 构造一段文本, 然后逐词分析
        std::stringstream ss;
        ss << "apple banana cherry";    // 写入
        // 读指针默认在开头, 可以直接读
        std::string fruit;
        std::println("逐词读取:");
        while (ss >> fruit) {
            std::println("  - {}", fruit);
        }

        // 读完后流处于 eof 状态, 需要 clear 才能继续写入
        ss.clear();                     // 清除 eofbit
        ss.str("");                     // 清空 buffer (注意: str("") 清空, str() 获取)
        ss << "新的内容";                // 写入新内容
        std::println("重写后: {}", ss.str());
    }

    {
        // 原地修改: 读 → 修改 → 重写
        std::println("\n--- 原地修改 ---");
        std::stringstream ss("123 xyz 456");
        int a, c;
        std::string b;
        ss >> a >> b >> c;              // 读取: a=123, b="xyz", c=456

        // 修改后写回
        ss.str(""); ss.clear();         // 清空 buffer 和状态
        ss << a * 10 << " " << b << " " << c * 10;  // 每个数乘以 10
        std::println("原始: 123 xyz 456");
        std::println("修改后: {}", ss.str());
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: str() 方法深度解析
    // ═══════════════════════════════════════════════════════════════════════════
    // str()    — 返回底层字符串的副本 (不改变流)
    // str("")  — 将底层字符串设置为 "", 同时重置读/写位置
    // str(s)   — 将底层字符串设置为 s, 同时重置读/写位置到开头
    // 注意: str() 返回的是副本 — 修改它不会影响流; 要修改流请用 str(s)
    lesson::print_subtitle("Part 4: str() 方法详解");

    {
        std::stringstream ss("hello world");
        std::string s = ss.str();       // 获取副本
        s[0] = 'H';                     // 修改副本不影响流
        std::println("副本修改: {} (流不变: {})", s, ss.str());

        ss.str("你好 世界");             // 用新字符串替换底层 buffer
        std::println("str() 替换后: {}", ss.str());

        // .str() 返回拷贝后可以安全使用, 不受后续写入影响
        std::string snapshot = ss.str();
        ss << " 新增内容";
        std::println("快照: '{}'", snapshot);         // 不受影响
        std::println("当前: '{}'", ss.str());
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 实际应用 — 构建 JSON 片段 + 解析配置
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 5: 实战 — 使用 streams 库构建结构化文本");

    {
        // 用 streams::Entry + streams::build_json_array() 构建 JSON
        // 这些工具函数封装了 ostringstream 的使用模式
        std::vector<streams::Entry> entries = {
            {"Alice", 1001, 95.5},
            {"Bob",   1002, 87.0},
            {"Ecks",  1003, 99.9},
        };

        // 一行构建 JSON — 底层用 ostringstream 高效拼接
        std::string json = streams::build_json_array(entries);
        std::println("生成的 JSON:\n{}", json);

        // 你当然也可以看到每个 Entry 的 JSON 表示
        std::println("单个 Entry: {}", streams::entry_to_json(entries[0]));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("陷阱 1: 清空 stringstream 时只调 str(\"\") 不调 clear()");
    std::println("  → 如果流在 eof 或 fail 状态, 后续写入全部无效");
    std::println("  → 解决: 同时调用 ss.str(\"\") 和 ss.clear()");
    std::println("陷阱 2: 用 while(iss >> val) 但忘了流里有残留的 failbit");
    std::println("  → 只要之前操作失败过且没 clear(), 后续所有 >> 都立刻返回 false");
    std::println("  → 解决: 每次重用时都 ss.clear() + ss.str(\"\")");
    std::println("陷阱 3: >> 操作符会跳过前导空白, 不适合解析含空格的字段");
    std::println("  → 解决: 含空格的字段用 std::getline");
    std::println("陷阱 4: str() 返回的是副本, 后续写入不会反映到已获取的 str 上");
    std::println("  → 这其实是优点 (快照隔离), 但不要误以为它能实时反映");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("1. 用 ostringstream 构建一个 HTML 表格, 展示 3 个学生的成绩");
    std::println("2. 用 istringstream 解析一行 \"x=10, y=20, z=30\" 并计算 x+y+z");
    std::println("3. 写一个函数: 输入是 CSV 字符串, 输出是 vector<string> (用 getline + 逗号分隔)");
    std::println("4. 比较 stringstream 和 std::from_chars 解析 1000000 个整数串的性能");

    return 0;
}
