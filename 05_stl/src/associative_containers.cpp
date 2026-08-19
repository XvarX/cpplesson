// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/associative_containers.cpp — 关联容器 教学演示函数实现                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/associative_containers.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::set — 有序唯一元素集合                                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_set() {
    lesson::print_subtitle("Part 1: std::set<int> — 有序唯一集合");

    set<int> s{5, 2, 8, 2, 1, 5};  // 重复的 2 和 5 会被忽略
    print("set 自动去重并排序: ");
    for (auto x : s) print("{} ", x);
    println("");
    // 预期: 1 2 5 8

    // 插入 O(log n)
    auto [it, inserted] = s.insert(3);
    println("插入 3: 成功={}, 值={}", inserted, *it);   // 预期: true, 3
    auto [it2, inserted2] = s.insert(3);
    println("再次插入 3: 成功={}, 值={}", inserted2, *it2); // 预期: false, 3

    // 查找 O(log n)
    if (s.contains(5)) println("5 在集合中");  // C++20
    // 预期: 5 在集合中

    // 删除
    s.erase(2);
    println("删除 2 后 size={}", s.size());  // 预期: 3
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::map — 有序键值对                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_map() {
    lesson::print_subtitle("Part 2: std::map<string, int> — 有序键值对");

    map<string, int> scores{
        {"Charlie", 85},
        {"Alice", 92},
        {"Bob", 78},
    };
    // 自动按键 (string) 字典序排列

    println("map 内容 (自动按 key 排序):");
    for (auto& [name, score] : scores) {
        println("  {} -> {}", name, score);
    }
    // 预期: Alice->92, Bob->78, Charlie->85

    // 插入方式
    scores["David"] = 88;                     // 方式1: operator[] (不存在则插入默认值)
    scores.insert({"Eve", 95});               // 方式2: insert
    scores.try_emplace("Frank", 73);          // 方式3: C++17 try_emplace (不存在才构造)

    // operator[] vs at()
    println("Alice 分数: {}", scores["Alice"]);    // 不存在会插入默认值 0!
    println("Alice 分数: {}", scores.at("Alice")); // 不存在会抛 std::out_of_range

    // 查找
    if (auto it = scores.find("Bob"); it != scores.end()) {
        println("找到 Bob: {}", it->second);  // 预期: 78
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: std::multiset / std::multimap — 允许重复键                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_multi() {
    lesson::print_subtitle("Part 3: multiset / multimap — 允许重复");

    // multiset: 允许重复元素
    multiset<int> ms{3, 1, 3, 2, 3};
    print("multiset (允许重复): ");
    for (auto x : ms) print("{} ", x);
    println("");
    // 预期: 1 2 3 3 3

    println("元素 3 出现了 {} 次", ms.count(3));  // 预期: 3

    // equal_range 返回所有匹配元素的范围
    auto [lower, upper] = ms.equal_range(3);
    print("equal_range(3): ");
    for (auto it = lower; it != upper; ++it) print("{} ", *it);
    println("");
    // 预期: 3 3 3

    // multimap: 允许重复键
    multimap<string, int> mm;
    mm.insert({"Alice", 85});
    mm.insert({"Alice", 92});  // 同一个 key 可以有多个值
    mm.insert({"Bob", 78});

    println("Alice 的成绩有 {} 个", mm.count("Alice"));  // 预期: 2
    auto [lo, hi] = mm.equal_range("Alice");
    for (auto it = lo; it != hi; ++it) {
        println("  Alice: {}", it->second);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 自定义比较器                                                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_comparator() {
    lesson::print_subtitle("Part 4: 自定义比较器");

    // 降序 set: 用 greater<int>
    set<int, greater<int>> desc_set{1, 5, 2, 8};
    print("降序 set: ");
    for (auto x : desc_set) print("{} ", x);
    println("");
    // 预期: 8 5 2 1

    // 自定义比较器: 按字符串长度排序
    auto cmp = [](const string& a, const string& b) {
        return a.size() < b.size();  // 短的在前面
    };
    // 注意: map 的第三个模板参数是比较器类型
    map<string, int, decltype(cmp)> len_map(cmp);
    len_map["aaa"] = 1;
    len_map["b"] = 2;
    len_map["ccccc"] = 3;
    len_map["dd"] = 4;

    println("按长度排序的 map:");
    for (auto& [k, v] : len_map) println("  '{}' -> {}", k, v);
    // 预期: b->2, dd->4, aaa->1, ccccc->3 (按长度升序)
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: lower_bound / upper_bound / equal_range                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_bound() {
    lesson::print_subtitle("Part 5: lower_bound / upper_bound / equal_range");

    set<int> s{10, 20, 30, 40, 50, 60, 70};
    print("集合: ");
    for (auto x : s) print("{} ", x);
    println("");

    // lower_bound: 第一个 >= 35 的元素
    auto lb = s.lower_bound(35);
    println("lower_bound(35) = {}", *lb);  // 预期: 40

    // upper_bound: 第一个 > 35 的元素
    auto ub = s.upper_bound(35);
    println("upper_bound(35) = {}", *ub);  // 预期: 40 (同上，因为 35 不在集合中)

    // 当值存在时
    println("lower_bound(40) = {}", *s.lower_bound(40));  // 预期: 40
    println("upper_bound(40) = {}", *s.upper_bound(40));  // 预期: 50

    // equal_range: 获取区间 [30, 60) 的所有元素
    auto [lo, hi] = s.equal_range(35);  // 等价于 {lower_bound, upper_bound}
    println("区间 [30, 60):");
    for (auto it = s.lower_bound(30); it != s.upper_bound(60); ++it) {
        print("{} ", *it);
    }
    println("");
    // 预期: 30 40 50 60
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void associative_containers_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: map的operator[]会在key不存在时插入默认值
    map<string, int> m{{"a", 1}};
    println("m.count('b') = {}", m.count("b"));  // 预期: 0
    // println("m['b'] = {}", m["b"]);  // 意外插入! 现在 m 中有 "b"->0
    // 正确做法: 用 find() 或 contains() 判断
    lesson::print_note("用 m.contains(key) 或 m.find(key) 判断存在性，不要用 m[key]");

    // 陷阱2: set/map 的迭代器在删除当前元素后失效
    // 错误写法:
    // for (auto it = s.begin(); it != s.end(); ++it) { s.erase(it); }
    // 正确写法 C++11+:
    set<int> s2{1, 2, 3, 4, 5};
    for (auto it = s2.begin(); it != s2.end(); ) {
        if (*it % 2 == 0) it = s2.erase(it);  // erase 返回下一个迭代器
        else ++it;
    }
    print("删除偶数后: ");
    for (auto x : s2) print("{} ", x);
    println("");
    // 预期: 1 3 5

    // 陷阱3: 自定义比较器必须满足严格弱序 (strict weak ordering)
    // 不能写: cmp(a,a)==true 或 cmp(a,b)==true && cmp(b,a)==true
    lesson::print_note("自定义比较器必须满足严格弱序（strict weak ordering）");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void associative_containers_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 map<string, vector<int>> 实现一个成绩管理表（姓名->多个成绩）");
    println("2. 实现一个用 set 维护的排行榜（自动按分数排序，支持插入和查询 Top-K）");
    println("3. 用 multiset 统计一段文本中所有单词的出现次数");
    println("4. 写一个自定义比较器，让 map 按 key 长度降序排列");
    println("5. 解释为什么 map 的迭代器是双向迭代器而不是随机访问迭代器");
}

} // namespace stl_learn
