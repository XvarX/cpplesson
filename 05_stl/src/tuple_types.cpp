// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/tuple_types.cpp — 多值与可选类型 教学演示函数实现                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/tuple_types.hpp"
#include <algorithm>
#include <numeric>
#include <ranges>

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::pair — 键值对基础                                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_pair() {
    lesson::print_subtitle("Part 1: std::pair — 键值对");

    // 构造
    pair<string, int> p1{"Alice", 30};
    auto p2 = make_pair("Bob", 25);       // 自动推导
    auto p3 = pair{"Charlie", 35};        // C++17 CTAD

    println("{}: {}岁", p1.first, p1.second);

    // 结构化绑定 (C++17) — 解构 pair
    auto [name, age] = p1;
    println("结构化绑定: {} = {}岁", name, age);

    // 实际用途: map 的 insert 返回 pair<iterator, bool>
    map<string, int> m;
    auto [it, inserted] = m.insert({"Dave", 40});
    println("插入成功: {}", inserted);

    // 作为函数返回值
    auto min_max = [](const vector<int>& v) -> pair<int, int> {
        return {*ranges::min_element(v), *ranges::max_element(v)};
    };
    auto [lo, hi] = min_max({3, 1, 5, 2, 8});
    println("min={}, max={}", lo, hi);  // 预期: 1, 8
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::tuple — N 个值的聚合                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_tuple() {
    lesson::print_subtitle("Part 2: std::tuple — N 元组");

    // 构造
    tuple<string, int, double> t1{"张三", 25, 178.5};
    auto t2 = make_tuple("李四", 30, 165.0);
    auto t3 = tuple{"王五", 22, 180.0};  // C++17 CTAD

    // 访问: get<N> (编译期索引，类型安全)
    println("姓名: {}, 年龄: {}, 身高: {:.1f}cm",
            get<0>(t1), get<1>(t1), get<2>(t1));

    // 按类型访问 (如果类型唯一)
    println("姓名: {}", get<string>(t1));

    // 结构化绑定
    auto [name, age, height] = t1;
    println("{}, {}岁, {:.1f}cm", name, age, height);

    // tie: 将已有变量绑定到 tuple
    string n;
    int a;
    tie(n, a, ignore) = t1;  // ignore 跳过 height
    println("tie: {} 今年 {} 岁", n, a);

    // tuple_cat: 拼接两个 tuple
    auto t4 = tuple_cat(tuple{42}, tuple{"hello"});
    println("tuple_cat: {} {}", get<0>(t4), get<1>(t4));  // 预期: 42 hello

    // 实际用途: 函数返回多个值
    auto stats = [](const vector<int>& v) {
        double sum = accumulate(v.begin(), v.end(), 0.0);
        return tuple{v.size(), sum, sum / v.size()};
    };
    auto [cnt, total, avg] = stats({1, 2, 3, 4, 5});
    println("个数={}, 总和={}, 平均={:.1f}", cnt, total, avg);
    // 预期: 5, 15, 3.0
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: std::optional — 可能没有值                                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_optional() {
    lesson::print_subtitle("Part 3: std::optional — 可选值");

    // 创建
    optional<int> maybe;            // 空 optional
    optional<int> maybe42{42};      // 包含值
    auto opt = make_optional(3.14); // 自动推导

    // 判断是否有值
    if (maybe42) {  // 或 maybe42.has_value()
        println("maybe42 = {}", *maybe42);  // 解引用
        println("maybe42 = {}", maybe42.value());  // 有检查的访问 (空时抛异常)
    }

    // value_or: 提供默认值
    println("maybe = {}", maybe.value_or(-1));  // 预期: -1 (空, 返回默认值)

    // 实际用途1: 安全的查找
    auto safe_find = [](const vector<int>& v, int target) -> optional<size_t> {
        auto it = ranges::find(v, target);
        if (it != v.end()) return distance(v.begin(), it);
        return nullopt;  // 明确表示"未找到"
    };

    auto idx = safe_find({10, 20, 30}, 20);
    if (idx) println("找到了，位置: {}", *idx);  // 预期: 1

    auto idx2 = safe_find({10, 20, 30}, 999);
    println("查找 999: {}", idx2.value_or(999));  // 预期: 999

    // 实际用途2: 配置项
    optional<string> config = "production";
    string env = config.value_or("development");
    println("环境: {}", env);  // 预期: production

    // and_then / or_else / transform (C++23 单子操作)
    optional<int> doubled = maybe42.and_then(
        [](int x) { return optional{x * 2}; });
    println("42*2 = {}", *doubled);  // 预期: 84
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: std::variant — 类型安全的联合体                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_variant() {
    lesson::print_subtitle("Part 4: std::variant — 类型安全联合体");

    // 定义: 可以是 int, double, 或 string
    variant<int, double, string> v;

    v = 42;                       // 持有 int
    println("holds int: {}", holds_alternative<int>(v));     // 预期: true
    println("get<int>: {}", get<int>(v));                     // 预期: 42

    v = 3.14;                     // 持有 double
    v = string("hello");          // 持有 string

    // get 如果类型不匹配会抛 bad_variant_access
    // println("{}", get<int>(v));  // 异常!

    // get_if: 安全访问，返回指针
    if (auto* p = get_if<string>(&v)) {
        println("*p = {}", *p);  // 预期: hello
    }
    if (auto* p = get_if<int>(&v)) {
        // 不会执行，因为当前 v 持有 string
    } else {
        println("v 不是 int");
    }

    // ── std::visit: 对所有可能的类型应用同一个函数 ──
    auto printer = [](const auto& val) {
        println("visit: {}", val);
    };
    visit(printer, v);  // 预期: visit: hello

    // 返回值的 visit — 实际中最常用的模式
    auto to_string = [](const auto& val) -> string {
        using T = decay_t<decltype(val)>;
        if constexpr (is_same_v<T, int>) {
            return format("整数: {}", val);
        } else if constexpr (is_same_v<T, double>) {
            return format("浮点数: {:.2f}", val);
        } else {
            return format("字符串: '{}'", val);
        }
    };
    v = 42;
    println("{}", visit(to_string, v));   // 预期: 整数: 42
    v = string("cpp");
    println("{}", visit(to_string, v));   // 预期: 字符串: 'cpp'

    // 实际用途: Result 类型 (成功 | 错误)
    using Result = variant<string, int>;  // string=错误信息, int=结果值
    auto divide = [](int a, int b) -> Result {
        if (b == 0) return Result{"除数不能为零"};
        return Result{a / b};
    };
    auto r = divide(10, 0);
    visit([](const auto& v) { println("结果: {}", v); }, r);
    // 预期: 结果: 除数不能为零
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: std::any — 任意类型的容器                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_any() {
    lesson::print_subtitle("Part 5: std::any — 任意类型容器");

    any a = 42;
    println("a 是 int: {} -> {}", a.type().name(), any_cast<int>(a));

    a = string("hello");
    println("a 现为 string: {}", any_cast<string>(a));

    a = 3.14;
    // any_cast 类型不匹配会抛 bad_any_cast
    try {
        println("{}", any_cast<int>(a));  // 现在是 double
    } catch (const bad_any_cast& e) {
        println("any_cast 失败: {}", e.what());
    }

    // 实际用途: 通用设置值存储
    map<string, any> settings;
    settings["volume"] = 75;
    settings["name"] = string("主播放器");
    settings["enabled"] = true;

    if (auto* vol = any_cast<int>(&settings["volume"])) {
        println("音量: {}", *vol);  // 预期: 75
    }

    // 注意: variant 比 any 更优 (编译期类型检查、无堆分配)
    lesson::print_note("优先使用 std::variant 而非 std::any — variant 类型安全且无堆分配");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void tuple_types_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: optional 的 operator* 在空时是 UB (未定义行为)
    optional<int> empty;
    // println("{}", *empty);  // UB! 先检查 has_value()
    lesson::print_note("optional 解引用前必须先检查 has_value() 或用 value_or()");

    // 陷阱2: variant 的 get<T> 类型不匹配会抛异常，用 get_if<T> 安全
    lesson::print_note("variant 用 get_if<T> 代替 get<T> 避免异常");

    // 陷阱3: any_cast 必须类型完全匹配，没有隐式转换
    any a = 42;
    // any_cast<double>(a);  // 错误! 42 是 int, 不是 double
    lesson::print_note("any_cast 要求类型完全匹配，没有隐式转换");

    // 陷阱4: tie 不能绑定到临时变量
    // tie(x, y) = make_tuple(1, 2);  // 错误! tie 要求左值引用
    lesson::print_note("std::tie 需要左值引用，用结构化绑定绑定临时量");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void tuple_types_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 pair 实现一个函数，返回 vector 中的最小值和最大值");
    println("2. 用 tuple 实现一个学生信息结构（姓名、学号、各科成绩），并用结构化绑定解构");
    println("3. 用 optional<string> 实现一个缓存查找函数（缓存命中返回缓存的字符串）");
    println("4. 用 variant<int, string, ErrorCode> 实现一个错误处理系统");
    println("5. 比较 variant 和 any 的优缺点，什么场景下应该用 any?");
}

} // namespace stl_learn
