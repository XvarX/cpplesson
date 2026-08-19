// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/charconv_and_optional_monadic.cpp — 高性能转换与 optional 单子操作       ║
// ║  教学演示函数实现                                                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/charconv_and_optional_monadic.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::to_chars — 高性能数值到字符串转换                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_to_chars() {
    lesson::print_subtitle("Part 1: std::to_chars — 数值 → 字符串");

    // ── to_chars 签名 ──
    // to_chars(char* first, char* last, 数值值, int base = 10);
    // 返回值: to_chars_result { char* ptr; errc ec; }
    //   - ptr 指向写入结束位置 (不是 '\0' 结尾!)
    //   - ec == errc{} 表示成功

    // ── 整数转换 ──
    char buf[64];  // 预分配足够大的缓冲区

    // 十进制整数
    auto [ptr1, ec1] = to_chars(buf, buf + sizeof(buf), 42);
    // to_chars 不会自动加 '\0'，需要手动根据 ptr 截取
    if (ec1 == errc{}) {
        string_view result(buf, ptr1 - buf);  // 构造 string_view，无需拷贝
        println("整数 42 → \"{}\" (长度: {})", result, ptr1 - buf);
        // 预期: "42" (2)
    }

    // 不同进制: 2 进制、8 进制、16 进制
    auto [ptr_bin, ec_bin] = to_chars(buf, buf + sizeof(buf), 255, 2);
    string_view bin_result(buf, ptr_bin - buf);
    println("255 二进制 → \"{}\"", bin_result);  // 预期: "11111111"

    auto [ptr_oct, ec_oct] = to_chars(buf, buf + sizeof(buf), 255, 8);
    string_view oct_result(buf, ptr_oct - buf);
    println("255 八进制 → \"{}\"", oct_result);  // 预期: "377"

    auto [ptr_hex, ec_hex] = to_chars(buf, buf + sizeof(buf), 255, 16);
    string_view hex_result(buf, ptr_hex - buf);
    println("255 十六进制 → \"{}\"", hex_result);  // 预期: "ff"

    // 负数
    to_chars(buf, buf + sizeof(buf), -12345);
    println("负数 -12345 → \"{}\"", string_view(buf));  // 注意: string_view 读到 \0

    // ── 浮点数转换 (C++17 支持，C++23 完善) ──
    // 默认格式: 最短表示，保证往返转换 (round-trip)
    auto [ptr_f1, ec_f1] = to_chars(buf, buf + sizeof(buf), 3.14159265);
    println("π ≈ \"{}\"", string_view(buf, ptr_f1 - buf));

    // 指定精度: chars_format::fixed (定点)
    to_chars(buf, buf + sizeof(buf), 3.14159265, chars_format::fixed, 4);
    println("π fixed(4) → \"{}\"", buf);  // 预期: "3.1416"

    // chars_format::scientific (科学计数法)
    to_chars(buf, buf + sizeof(buf), 299792458.0, chars_format::scientific, 3);
    println("光速 scientific(3) → \"{}\"", buf);  // 预期: "2.998e+08"

    // chars_format::general (通用，自动选择)
    to_chars(buf, buf + sizeof(buf), 0.000012345, chars_format::general, 4);
    println("通用格式(4) → \"{}\"", buf);  // 预期: "1.235e-05"

    // chars_format::hex (十六进制浮点，C++17)
    to_chars(buf, buf + sizeof(buf), 1.5, chars_format::hex);
    println("1.5 十六进制 → \"{}\"", buf);  // 预期: "1.8p+0" (1.5 = 1.8p0)

    // ── 性能对比概念 ──
    lesson::print_note("to_chars 比 to_string/snprintf 快 5-10 倍，原因是无 locale、无分配、无异常");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::from_chars — 高性能字符串到数值转换                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_from_chars() {
    lesson::print_subtitle("Part 2: std::from_chars — 字符串 → 数值");

    // ── from_chars 签名 ──
    // from_chars(const char* first, const char* last, 数值& value, int base = 10);
    // 返回值: from_chars_result { const char* ptr; errc ec; }
    //   - ptr 指向第一个未解析的字符
    //   - ec == errc{} 表示成功
    //   - ec == errc::invalid_argument 表示无法解析
    //   - ec == errc::result_out_of_range 表示值超出范围

    // ── 整数解析 ──
    auto try_parse_int = [](string_view sv) {
        int value = 0;
        auto [ptr, ec] = from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec == errc{}) {
            println("解析成功: \"{}\" → {}, 剩余: \"{}\"",
                    sv, value, string_view(ptr, sv.data() + sv.size() - ptr));
        } else if (ec == errc::invalid_argument) {
            println("解析失败: \"{}\" 不是有效整数", sv);
        } else if (ec == errc::result_out_of_range) {
            println("解析失败: \"{}\" 超出 int 范围", sv);
        }
    };

    try_parse_int("42");       // 预期: 42
    try_parse_int("-100");     // 预期: -100
    try_parse_int("  123");    // 注意: from_chars 不跳过前导空白!
    try_parse_int("123abc");   // 预期: 123, 剩余 "abc"
    try_parse_int("999999999999999999999");  // 超出范围
    try_parse_int("hello");    // 预期: 不是有效整数

    // ── 不同进制 ──
    int val = 0;
    from_chars("ff", "ff" + 2, val, 16);
    println("十六进制 \"ff\" → {}", val);  // 预期: 255

    from_chars("1010", "1010" + 4, val, 2);
    println("二进制 \"1010\" → {}", val);  // 预期: 10

    from_chars("77", "77" + 2, val, 8);
    println("八进制 \"77\" → {}", val);  // 预期: 63

    // ── 浮点数解析 ──
    auto try_parse_double = [](string_view sv) {
        double value = 0.0;
        auto [ptr, ec] = from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec == errc{}) {
            println("解析成功: \"{}\" → {:.6f}, 剩余: \"{}\"",
                    sv, value, string_view(ptr, sv.data() + sv.size() - ptr));
        } else {
            println("解析失败: \"{}\"", sv);
        }
    };

    try_parse_double("3.14159");     // 预期: 3.141590
    try_parse_double("-2.5e3");      // 预期: -2500.000000
    try_parse_double("1.23e-4");     // 预期: 0.000123
    try_parse_double("inf");         // 预期: inf (C++17 支持)
    try_parse_double("nan");         // 预期: nan

    // ── from_chars 不跳空白！──
    lesson::print_note("from_chars 不会跳过前导空白，这是与 strtod/scanf 的重要区别");

    double d = 0.0;
    auto [ptr, ec] = from_chars("  3.14", "  3.14" + 6, d);
    if (ec != errc{}) {
        println("\"  3.14\" 解析失败 — 前导空白不被接受");
    }

    // 正确做法: 手动跳过空白
    string_view input = "  3.14";
    input.remove_prefix(min(input.find_first_not_of(" \t\n\r"), input.size()));
    from_chars(input.data(), input.data() + input.size(), d);
    println("跳过空白后 \"{}\" → {}", input, d);  // 预期: 3.14

    // ── 性能对比 ──
    lesson::print_note("from_chars 比 atoi/strtod 快 3-10 倍，且不依赖全局 locale");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: std::optional 单子操作 (C++23)                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_optional_monadic() {
    lesson::print_subtitle("Part 3: std::optional 单子操作 (C++23)");

    // ── and_then: 链式组合 ──
    // 如果 optional 有值，调用 f(value) 并返回一个新的 optional
    // 如果 optional 为空，直接返回空 optional（短路）
    // 签名: optional<U> and_then(F&& f) 其中 f: T -> optional<U>

    // 模拟场景: 用户 → 查询ID → 查询资料
    auto find_id = [](const string& name) -> optional<int> {
        if (name == "Alice") return 1001;
        if (name == "Bob")   return 1002;
        return nullopt;
    };
    auto find_profile = [](int id) -> optional<string> {
        if (id == 1001) return "Alice 的资料: 工程师, 30岁";
        if (id == 1002) return "Bob 的资料: 设计师, 25岁";
        return nullopt;
    };

    // 传统写法: 嵌套 if
    auto id = find_id("Alice");
    if (id) {
        auto profile = find_profile(*id);
        if (profile) println("传统: {}", *profile);
    }

    // C++23 and_then 写法: 清晰的管道
    auto profile = find_id("Alice")
        .and_then(find_profile);
    if (profile) println("and_then: {}", *profile);
    // 预期: "Alice 的资料: 工程师, 30岁"

    // 链中断: 查找不存在的人
    auto missing = find_id("Charlie")
        .and_then(find_profile);
    println("Charlie 查找结果: {}", missing.has_value() ? *missing : "未找到");
    // 预期: 未找到

    // 多级链式调用
    auto deep = find_id("Bob")
        .and_then(find_profile)
        .and_then([](const string& s) -> optional<string> {
            return s + " | VIP 会员";
        });
    println("多级链: {}", deep.value_or("失败"));
    // 预期: "Bob 的资料: 设计师, 25岁 | VIP 会员"

    // ── or_else: 为空时提供替代 ──
    // 如果 optional 有值，直接返回该值（不调用 f）
    // 如果 optional 为空，调用 f 并返回其结果的 optional
    // 签名: optional<T> or_else(F&& f) 其中 f: void -> optional<T>

    auto result = find_id("Unknown")
        .or_else([] { return optional<int>{0}; });  // 提供默认 ID
    println("or_else 默认: {}", result.value());  // 预期: 0

    // or_else 链: 多个数据源回退
    auto find_cache = [](const string& key) -> optional<string> {
        // 模拟缓存未命中
        return nullopt;
    };
    auto find_db = [](const string& key) -> optional<string> {
        return "DB 中的 " + key;
    };

    auto data = find_cache("user:42")
        .or_else([&] { return find_db("user:42"); })
        .or_else([] { return optional<string>{"默认值"}; });
    println("多级回退: {}", *data);
    // 预期: "DB 中的 user:42"

    // ── transform: 映射值 ──
    // 如果 optional 有值，返回 optional<f(value)>
    // 如果 optional 为空，返回空 optional
    // 签名: optional<U> transform(F&& f) 其中 f: T -> U (注意: 返回 T->U，不是 T->optional<U>)

    optional<int> num = 42;
    auto doubled = num.transform([](int x) { return x * 2; });
    println("42 * 2 = {}", doubled.value());  // 预期: 84

    auto str_len = num
        .transform([](int x) { return x * 3; })
        .transform([](int x) { return format("结果是: {}", x); });
    println("transform 链: {}", str_len.value());  // 预期: "结果是: 126"

    // 空 optional — transform 短路
    optional<int> empty;
    auto empty_result = empty.transform([](int x) { return x * 2; });
    println("空 transform: {}", empty_result.has_value());  // 预期: false

    // ── 三者对比 ──
    // and_then(f): f 返回 optional<U> — 用于可能失败的链式调用
    // or_else(f):   f 返回 optional<T> — 用于提供回退/替代
    // transform(f): f 返回 U       — 用于纯映射，不会引入新的"空"状态
    lesson::print_note("and_then: f 返回 optional (可能失败); transform: f 返回普通值 (不会失败)");
    lesson::print_note("or_else: 只在为空时调用 f, 用于回退链");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: charconv + optional 综合实战 — 安全解析流水线                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_combined() {
    lesson::print_subtitle("Part 4: charconv + optional — 安全解析流水线");

    // ── 将 from_chars 封装为返回 optional 的接口 ──
    // 这样就能和 optional 的单子操作组合起来使用

    // 安全解析 int: 返回 optional<int>，解析失败返回 nullopt
    auto safe_parse_int = [](string_view sv) -> optional<int> {
        // 跳过前导空白 (from_chars 不处理)
        auto start = sv.find_first_not_of(" \t\n\r");
        if (start == string_view::npos) return nullopt;
        sv.remove_prefix(start);

        int value = 0;
        auto [ptr, ec] = from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec != errc{}) return nullopt;
        // 可选: 检查是否完全消耗了输入
        if (ptr != sv.data() + sv.size()) {
            // 允许尾随空白
            auto trailing = string_view(ptr, sv.data() + sv.size() - ptr);
            if (trailing.find_first_not_of(" \t\n\r") != string_view::npos)
                return nullopt;  // 有非空白的剩余字符
        }
        return value;
    };

    auto safe_parse_double = [](string_view sv) -> optional<double> {
        auto start = sv.find_first_not_of(" \t\n\r");
        if (start == string_view::npos) return nullopt;
        sv.remove_prefix(start);

        double value = 0.0;
        auto [ptr, ec] = from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec != errc{}) return nullopt;
        // 允许尾随空白，但不允许非空白字符
        auto trailing = string_view(ptr, sv.data() + sv.size() - ptr);
        if (trailing.find_first_not_of(" \t\n\r") != string_view::npos)
            return nullopt;
        return value;
    };

    // ── 场景: 解析配置行 "矩形: 宽=12.5, 高=8.0" ──
    // 传统写法: 大量 if-else 嵌套
    // 单子写法: 清晰的管道

    struct Rect { double w, h; };

    auto parse_rect = [&](string_view line) -> optional<Rect> {
        // Step 1: 查找 "宽="
        auto w_pos = line.find("宽=");
        if (w_pos == string_view::npos) return nullopt;

        // Step 2: 提取值部分直到逗号
        auto val_start = w_pos + strlen("宽=");
        auto val_end = line.find(',', val_start);
        auto w_str = (val_end == string_view::npos)
            ? line.substr(val_start)
            : line.substr(val_start, val_end - val_start);

        // Step 3: 解析宽度
        auto w_opt = safe_parse_double(w_str);
        if (!w_opt) return nullopt;

        // Step 4: 查找并解析高度
        auto h_pos = line.find("高=");
        if (h_pos == string_view::npos) return nullopt;
        auto h_start = h_pos + strlen("高=");
        auto h_str = line.substr(h_start);
        auto h_opt = safe_parse_double(h_str);

        // Step 5: 组合结果
        return h_opt.transform([&](double h) {
            return Rect{*w_opt, h};
        });
    };

    auto r = parse_rect("矩形: 宽=12.5, 高=8.0");
    if (r) println("矩形: {:.1f} x {:.1f}, 面积 = {:.2f}", r->w, r->h, r->w * r->h);
    // 预期: 12.5 x 8.0, 100.00

    // 无效输入
    auto r2 = parse_rect("矩形: 宽=abc, 高=8.0");
    println("无效输入: {}", r2.has_value() ? "解析成功" : "解析失败");
    // 预期: 解析失败

    // ── 使用单子操作构建更优雅的解析流水线 ──
    // 辅助函数: 提取 key=value 中的数值
    auto extract_val = [](string_view s, string_view key) -> optional<string_view> {
        auto pos = s.find(key);
        if (pos == string_view::npos) return nullopt;
        auto start = pos + key.size();
        auto end = s.find(',', start);
        return (end == string_view::npos)
            ? s.substr(start)
            : s.substr(start, end - start);
    };

    auto parse_rect_monadic = [&](string_view line) -> optional<Rect> {
        auto w_val = extract_val(line, "宽=").and_then(safe_parse_double);
        auto h_val = extract_val(line, "高=").and_then(safe_parse_double);

        // 用 transform 组合两个值: 当两个都有值时构造 Rect
        return w_val.and_then([&](double w) {
            return h_val.transform([w](double h) {
                return Rect{w, h};
            });
        });
    };

    auto r3 = parse_rect_monadic("矩形: 宽=12.5, 高=8.0");
    if (r3) println("单子流水线: {:.1f} x {:.1f}, 面积 = {:.2f}", r3->w, r3->h, r3->w * r3->h);
    // 预期: 12.5 x 8.0, 100.00

    lesson::print_note("将 from_chars 封装为返回 optional 后，可以无缝衔接单子操作链");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void charconv_and_optional_monadic_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: to_chars 不会加 '\0'
    lesson::print_note("to_chars 不会自动添加 '\\0'，需要用返回的 ptr 确定长度或用 string_view");

    // 陷阱2: from_chars 不跳过前导空白
    lesson::print_note("from_chars 不会跳过前导空白，必须手动处理 (不同于 strtod/scanf)");

    // 陷阱3: and_then vs transform 的区别
    // and_then(f): f 返回 optional<U> — 用于"可能失败"的下一步
    // transform(f): f 返回 U — 用于"不会失败"的纯映射
    lesson::print_note("and_then 要求回调返回 optional (可能失败); transform 要求回调返回普通值 (不会失败)");

    // 陷阱4: or_else 的回调必须返回 optional<T>，不能返回 T
    // 错误写法: opt.or_else([]{ return 0; });         // 编译错误!
    // 正确写法: opt.or_else([]{ return optional(0); });
    lesson::print_note("or_else 的回调必须返回 optional<T>，不能直接返回 T");

    // 陷阱5: to_chars 缓冲区必须足够大
    // 整数最大值需要约 65 字节 (用于 base=2 的 64 位整数)
    // 浮点数需要至少 32 字节
    lesson::print_note("to_chars 缓冲区: 整数至少 65 字节 (二进制64位), 浮点至少 32 字节");

    // 陷阱6: 浮点 from_chars 不支持十六进制浮点格式 (C++17 限制)
    lesson::print_note("from_chars 对浮点数不支持 chars_format::hex (C++17 标准要求，部分实现可能有扩展)");

    // 陷阱7: to_chars/from_chars 性能优势的前提是不用 locale
    // 如果需要千位分隔符等 locale 相关的格式，必须用其他方法
    lesson::print_note("charconv 不支持 locale 格式化 (千位分隔符等)，需要时用 format 或 stringstream");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void charconv_and_optional_monadic_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 to_chars 实现一个高性能日志系统，支持整数和浮点数的格式化输出");
    println("2. 用 from_chars 解析 CSV 文件中的数值列，统计每列的最大值和最小值");
    println("3. 用 optional 的 and_then/or_else/transform 实现一个配置查找链:");
    println("   命令行参数 → 环境变量 → 配置文件 → 默认值");
    println("4. 将 from_chars 封装为返回 optional<T> 的安全解析函数，并用于解析 JSON 数值");
    println("5. 用 optional 单子操作重构一段有 3 层嵌套 if-else 的旧代码");
    println("6. 对比 to_chars 和 std::format / snprintf 的性能差异 (用 steady_clock 测量)");
    println("7. 为什么 from_chars 选择不跳过前导空白? 这个设计决策的考量是什么?");
}

} // namespace stl_learn
