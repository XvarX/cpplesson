// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/unordered_containers.cpp — 无序关联容器 教学演示函数实现                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/unordered_containers.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: unordered_set — 哈希集合                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_unordered_set() {
    lesson::print_subtitle("Part 1: unordered_set — 哈希集合");

    unordered_set<int> us{5, 2, 8, 2, 1, 5};  // 重复被去重
    print("unordered_set 内容 (无序!): ");
    for (auto x : us) print("{} ", x);
    println("");
    // 预期: 顺序不确定，例如: 1 2 8 5

    // 插入/查找/删除 — 平均 O(1)
    us.insert(10);
    println("contains(5) = {}", us.contains(5));    // 预期: true
    println("contains(99) = {}", us.contains(99));   // 预期: false

    us.erase(2);
    println("删除 2 后 size={}", us.size());
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: unordered_map — 哈希映射                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_unordered_map() {
    lesson::print_subtitle("Part 2: unordered_map — 哈希映射");

    unordered_map<string, int> dict{
        {"apple", 5},
        {"banana", 3},
        {"cherry", 8},
    };

    // 查找 O(1) 平均
    if (auto it = dict.find("banana"); it != dict.end()) {
        println("banana: {}", it->second);  // 预期: 3
    }

    // 插入
    dict["date"] = 6;                  // operator[] 插入
    dict.insert({"elderberry", 2});    // insert
    dict.try_emplace("fig", 7);        // C++17: 不存在才构造

    println("dict 大小: {}", dict.size());  // 预期: 6

    // 遍历 (顺序不确定!)
    print("遍历顺序 (不确定): ");
    for (auto& [key, val] : dict) print("{}:{} ", key, val);
    println("");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: Bucket 与 Load Factor                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_bucket() {
    lesson::print_subtitle("Part 3: Bucket 与 Load Factor");

    unordered_set<int> us;

    // 预分配 bucket 数量，减少 rehash
    us.reserve(100);  // 预分配至少 100 个槽位 (C++11 rehash, C++20 reserve)
    println("reserve(100) 后: bucket_count={}", us.bucket_count());

    // 插入元素
    for (int i = 0; i < 20; ++i) us.insert(i);

    println("size={}, bucket_count={}", us.size(), us.bucket_count());
    println("load_factor = {:.3f}", us.load_factor());
    println("max_load_factor = {}", us.max_load_factor());

    // 查看某个元素在哪个 bucket
    println("元素 5 在 bucket {}", us.bucket(5));

    // 遍历某个 bucket 的内容
    auto idx = us.bucket(5);
    print("bucket {} 中的元素: ", idx);
    for (auto it = us.begin(idx); it != us.end(idx); ++it) {
        print("{} ", *it);
    }
    println("");

    // 手动 rehash — 调整 bucket 数量
    us.rehash(200);
    println("rehash(200) 后: bucket_count={}", us.bucket_count());
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 自定义哈希函数                                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_custom_hash() {
    lesson::print_subtitle("Part 4: 自定义哈希函数");

    // 使用 stl_learn::Point 和 stl_learn::PointHash (在头文件中定义)

    // 方式1: 结构体特化 std::hash
    // (在全局作用域中无法在这里特化，所以用方式2)

    // 方式2: 自定义哈希函数对象 — 推荐方式
    // PointHash 已在 unordered_containers.hpp 中定义

    // 使用自定义哈希
    unordered_set<Point, PointHash> points;
    points.insert({1, 2});
    points.insert({3, 4});
    points.insert({1, 2});  // 重复，不会插入

    println("points 大小: {}", points.size());   // 预期: 2
    println("contains({{1,2}}): {}", points.contains({1, 2})); // 预期: true

    // unordered_map 的用法
    unordered_map<Point, string, PointHash> labels;
    labels[{0, 0}] = "原点";
    labels[{1, 0}] = "X轴单位点";
    println("原点: {}", labels[{0, 0}]);
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: unordered vs ordered 容器对比                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_comparison() {
    lesson::print_subtitle("Part 5: unordered vs ordered 容器对比");

    println("┌──────────────────┬───────────────────┬───────────────────┐");
    println("│      特性         │  set / map        │ unordered_set/map │");
    println("├──────────────────┼───────────────────┼───────────────────┤");
    println("│ 底层数据结构      │  红黑树           │  哈希表           │");
    println("│ 插入/删除/查找    │  O(log n)         │  O(1) 平均       │");
    println("│ 最坏情况          │  O(log n)         │  O(n)             │");
    println("│ 元素顺序          │  有序(默认 <)     │  无序             │");
    println("│ 内存开销          │  较小             │  较大(bucket)     │");
    println("│ 缓存友好度        │  较差(指针链接)   │  较好(连续bucket) │");
    println("│ 迭代器失效(insert)│  不失效           │  可能(若rehash)   │");
    println("└──────────────────┴───────────────────┴───────────────────┘");

    println("\n快速决策:");
    println("  需要有序遍历       → set / map");
    println("  追求查找速度       → unordered_set / unordered_map");
    println("  键是自定义类型     → 需要提供 operator== 和 hash 函数");
    println("  内存敏感           → set / map (红黑树内存开销更小)");
    println("  需要范围查询       → set / map (支持 lower_bound)");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void unordered_containers_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: 插入后迭代器可能因为 rehash 失效
    unordered_map<int, string> m;
    m[1] = "one";
    auto it = m.find(1);
    // 大量插入可能触发 rehash，导致 it 失效
    for (int i = 2; i < 100; ++i) m[i] = "value";  // 可能触发 rehash
    // it 可能已失效! 应重新获取
    lesson::print_note("大量插入后要重新获取迭代器，rehash 会导致之前迭代器失效");

    // 陷阱2: 未定义 == 和 hash 导致编译错误
    // unordered_set<MyStruct> s;  // 编译错误: 需要 operator== 和 hash<MyStruct>
    lesson::print_note("自定义类型作为 unordered 容器的键时，必须提供 operator== 和哈希函数");

    // 陷阱3: unordered_map 不支持 lower_bound/upper_bound
    // 如果需要范围查询，使用 map
    lesson::print_note("unordered 容器不支持 lower_bound/upper_bound，需要范围查询请用 map/set");

    // 陷阱4: 不要对指针类型使用默认哈希
    unordered_set<int*> ptr_set;
    int a = 5, b = 5;
    ptr_set.insert(&a);
    ptr_set.insert(&b);
    println("两个值相同的 int 放入 ptr_set: size={} (因为哈希的是地址不是值)", ptr_set.size());
    // 预期: 2 (因为 &a != &b)
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void unordered_containers_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 unordered_map 统计一段文本中每个单词出现的频率");
    println("2. 设计一个简单的 LRU 缓存 (提示: 需要 unordered_map + list)");
    println("3. 对 10 万个随机整数分别用 set 和 unordered_set 做插入+查找，比较性能");
    println("4. 自定义一个 Person{{name, age}} 结构，以 name 为键存入 unordered_map");
    println("5. 解释 load_factor 设置过高或过低分别有什么后果");
}

} // namespace stl_learn
