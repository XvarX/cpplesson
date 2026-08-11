// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: 结构化绑定 (C++17) —— 解构数组/tuple/成员变量                       ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握结构化绑定绑定到数组的语法                                        ║
// ║    2. 理解绑定到 tuple-like 类型 (std::pair/std::tuple)                    ║
// ║    3. 学会绑定到结构体/类的公有成员变量                                     ║
// ║    4. 掌握引用绑定 (auto& []) 与 range-for 的配合                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/structured_bindings.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <array>
#include <utility> // std::pair

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: 结构化绑定到数组                                                    ║
// ║  WHAT: auto [a, b, c] = array; —— 将数组元素分别绑定到命名变量            ║
// ║  WHY:  替代手动写 arr[0]/arr[1]/arr[2]; 语义更清晰; 减少下标错误          ║
// ║  WHEN: 函数返回多个值用数组; 固定大小的数据拆分                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_bind_to_array() {
    lesson::print_header("Part 1: 绑定到数组");

    // ── 1.1 绑定原生数组 ──
    {
        lesson::print_subtitle("1.1  绑定到原生数组");
        int values[] = {10, 20, 30};
        auto [x, y, z] = values;            // 按值拷贝, x=10 y=20 z=30
        std::println("x={}  y={}  z={}", x, y, z);

        // 修改 x 不影响原数组
        x = 999;
        std::println("修改 x 后: values[0]={}  x={}", values[0], x);
        // 输出: values[0]=10  x=999  (被拷贝了)
    }

    // ── 1.2 绑定到 std::array ──
    {
        lesson::print_subtitle("1.2  绑定到 std::array");
        std::array<double, 3> coords = {1.5, 2.5, 3.5};
        auto [a, b, c] = coords;            // 编译器知道元素数量 = 3
        std::println("a={:.1f}  b={:.1f}  c={:.1f}", a, b, c);
    }

    // ── 1.3 引用绑定 —— 修改原数组 ──
    {
        lesson::print_subtitle("1.3  引用绑定: 零拷贝 & 可修改原值");
        int data[] = {100, 200, 300};
        auto& [p, q, r] = data;             // p/q/r 是 data 元素的引用!
        p = 111;
        q = 222;
        r = 333;
        std::println("data = [{}, {}, {}]", data[0], data[1], data[2]);
        // 输出: data = [111, 222, 333]
    }

    // ── 1.4 const 引用绑定 —— 只读 ──
    {
        lesson::print_subtitle("1.4  const 引用绑定: 零拷贝只读");
        // 注: 原生数组绑定: 元素数量必须完全匹配
        lesson::print_note("原生数组绑定: 元素数量必须完全匹配");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 绑定到 tuple-like 类型 (pair/tuple)                               ║
// ║  WHAT: auto [key, val] = pair; —— 对任何实现了 tuple 协议的类型解构      ║
// ║  WHY:  map insert 返回 pair<iterator,bool>; 函数返回多值用 tuple          ║
// ║  WHEN: 遍历 map; 多个返回值; 解析复合数据                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_bind_to_tuple() {
    lesson::print_header("Part 2: 绑定到 std::pair / std::tuple");

    // ── 2.1 绑定 std::pair ──
    {
        lesson::print_subtitle("2.1  绑定 std::pair (map::insert)");
        std::map<std::string, int> ages;
        // insert 返回 pair<iterator, bool>
        auto [it, inserted] = ages.insert({"艾克斯", 25});
        if (inserted) {
            std::println("插入成功: {} → {}", it->first, it->second);
        }

        // 再次插入同一个 key
        auto [it2, inserted2] = ages.insert({"艾克斯", 30});
        if (!inserted2) {
            std::println("插入失败: '{}' 已存在, 值为 {}", it2->first, it2->second);
        }
    }

    // ── 2.2 绑定 std::tuple ──
    {
        lesson::print_subtitle("2.2  绑定 std::tuple (多返回值)");

        // 函数返回多个值用 tuple
        auto get_user_info = []() -> std::tuple<std::string, int, double> {
            return {"小明", 20, 178.5};
        };

        auto [name, age, height] = get_user_info();
        std::println("姓名: {}  年龄: {}  身高: {:.1f}cm", name, age, height);
    }

    // ── 2.3 std::tie 绑定到已有变量 ──
    {
        lesson::print_subtitle("2.3  std::tie 绑定到已有变量");
        std::string name;
        int age = 0;
        double score = 0.0;

        // std::tie 将 tuple 元素赋值到已有变量 (C++11 就有)
        std::tie(name, age, score) = std::make_tuple("小红", 22, 92.5);
        std::println("{} 年龄{} 成绩{:.1f}", name, age, score);
        lesson::print_note("std::tie: 已有变量的解构赋值 (vs 结构化绑定: 声明新变量)");
    }

    // ── 2.4 忽略某些字段 ──
    {
        lesson::print_subtitle("2.4  忽略不需要的绑定值");
        auto get_result = []() -> std::tuple<int, std::string, bool> {
            return {200, "OK", true};
        };
        auto [code, msg, _] = get_result(); // 用 _ 忽略第三个值 (C++26 有更好的忽略方式)
        std::println("状态码: {}  消息: {}", code, msg);
        // C++17 传统: 用 std::ignore 配合 std::tie
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 绑定到结构体/类的公有成员变量                                     ║
// ║  WHAT: auto [m1, m2, ...] = struct_obj; —— 按声明顺序绑定非静态成员       ║
// ║  WHY:  解构 POD 类型, 从数据类提取字段, 模式匹配                          ║
// ║  WHEN: 简单的数据聚合类型; 配置解析; 返回值拆包                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_bind_to_members() {
    lesson::print_header("Part 3: 绑定到结构体成员");

    using namespace modern_syntax;

    // ── 3.1 基本用法 ──
    {
        lesson::print_subtitle("3.1  解构结构体");
        Person p{"艾克斯", 25, 99999.9};
        auto [n, a, s] = p;     // n='艾克斯'  a=25  s=99999.9
        std::println("姓名: {}  年龄: {}  薪水: {:.1f}", n, a, s);

        // 注意: 绑定的是拷贝还是引用?
        // auto [n, a, s] = p;      → 按值拷贝 (修改不影响 p)
        // auto& [n, a, s] = p;    → 按引用绑定 (修改会影响 p)
        n = "修改后";              // 只改了局部变量 n，不影响 p.name
        std::println("p.name 仍然 = {}", p.name);
    }

    // ── 3.2 引用绑定修改原对象 ──
    {
        lesson::print_subtitle("3.2  引用绑定: 修改原对象");
        Person p{"原始名", 30, 5000.0};
        auto& [name, age, salary] = p;
        name = "新名字";
        age  = 31;
        std::println("p: name='{}' age={}", p.name, p.age);
        // 输出: p: name='新名字' age=31
    }

    // ── 3.3 只能绑定公有成员 ──
    {
        lesson::print_subtitle("3.3  仅适用于公有成员");
        // 如果 Person 有私有成员, 结构化绑定会编译失败
        // 解决方案: 实现 std::tuple_size + std::tuple_element + get<>
        // 参考 modern_syntax::PrivatePerson —— 已实现 tuple 协议

        PrivatePerson pp{"测试", 99};
        auto [pp_name, pp_age] = pp;    // 通过 tuple 协议工作!
        std::println("PrivatePerson: name='{}' age={}", pp_name, pp_age);

        lesson::print_note("私有成员: 需要实现 tuple 协议才能用结构化绑定");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 结构化绑定 + range-for 组合技                                     ║
// ║  WHAT: for (auto& [k, v] : map) —— 在循环中同时遍历键和值                 ║
// ║  WHY:  避免 .first/.second 的笨拙写法; 直接命名语义清晰的变量              ║
// ║  WHEN: 遍历 map/unordered_map; 遍历 zip 视图; 遍历 enumerate               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_range_for_combo() {
    lesson::print_header("Part 4: 结构化绑定 + range-for 组合技");

    // ── 4.1 遍历 map 最优雅的方式 ──
    {
        lesson::print_subtitle("4.1  遍历 std::map (标准姿势)");
        std::map<int, std::string> id_to_name = {
            {1, "艾克斯"}, {2, "小明"}, {3, "小红"}
        };
        // 直接解构 pair<const Key, Value>
        for (const auto& [id, name] : id_to_name) {
            std::println("  ID: {:>2}  →  {}", id, name);
        }
    }

    // ── 4.2 遍历结构体数组 ──
    {
        lesson::print_subtitle("4.2  遍历结构体数组");
        using namespace modern_syntax;
        std::vector<Person> team = {
            {"张三", 28, 8000.0},
            {"李四", 32, 12000.0},
            {"王五", 25, 7000.0}
        };
        // 在循环中解构每个元素
        for (const auto& [name, age, salary] : team) {
            std::println("  {}  ({}岁)  ¥{:.0f}", name, age, salary);
        }
    }

    // ── 4.3 与 C++23 zip 视图配合 ──
    {
        lesson::print_subtitle("4.3  同时遍历两个容器 (C++23 zip)");
        std::vector<int>    ids  = {101, 102, 103};
        std::vector<double> scores = {85.5, 92.0, 78.5};
        // C++23: auto zipped = std::views::zip(ids, scores);
        // for (auto [id, score] : zipped) { ... }
        // 手动模拟:
        for (size_t i = 0; i < ids.size(); ++i) {
            auto [id, score] = std::pair{ids[i], scores[i]};
            std::println("  {} → {:.1f}分", id, score);
        }
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: 绑定数量不匹配
    {
        lesson::print_subtitle("陷阱1: 绑定数量必须与元素数一致");
        std::tuple<int, double, std::string> t{1, 2.0, "三"};
        auto [a, b, c] = t;     // OK: 3 对 3
        // auto [x, y] = t;     // 错误! 2 个绑定 vs 3 个元素
        std::println("a={}  b={:.1f}  c={}", a, b, c);
        lesson::print_warn("绑定数量必须精确匹配 —— 不能多也不能少");
    }

    // 陷阱2: 非公有成员不可直接绑定
    {
        lesson::print_subtitle("陷阱2: 只能绑定公有非静态成员");
        // 有私有成员的类 → 结构化绑定默认不可用
        lesson::print_warn("私有成员/继承/虚函数 → 默认不支持结构化绑定");
        lesson::print_note("需要实现 tuple 协议(std::tuple_size, get<I>)才能支持");
    }

    // 陷阱3: 引用绑定与临时对象
    {
        lesson::print_subtitle("陷阱3: 绑定到临时对象的引用 (生命周期)");
        auto make_pair = [] { return std::pair{42, std::string("hello")}; };
        auto [n, s] = make_pair();      // OK: 按值接收, 延长临时对象生命周期
        // auto& [rn, rs] = make_pair();// 危险! 引用绑定到临时对象
        std::println("n={}  s={}", n, s);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 写一个返回 tuple<string, int, bool> 的函数
    //        用结构化绑定接收并打印
    lesson::print_subtitle("练习1: 结构化绑定接收 tuple");

    // 练习2: 用 range-for + 结构化绑定遍历 map<string, vector<int>>
    //        打印每个学生的所有成绩
    lesson::print_subtitle("练习2: 遍历嵌套容器");

    // 练习3: 定义一个 Point3D 结构体，用引用绑定解构后修改原值
    lesson::print_subtitle("练习3: 引用绑定解构修改原值");
}

int main() {
    part1_bind_to_array();
    part2_bind_to_tuple();
    part3_bind_to_members();
    part4_range_for_combo();

    pitfalls();
    exercises();
    return 0;
}
