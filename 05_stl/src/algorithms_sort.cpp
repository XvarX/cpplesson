// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/algorithms_sort.cpp — 排序与二分查找 教学演示函数实现                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/algorithms_sort.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: sort / stable_sort — 排序核心                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_sort() {
    lesson::print_subtitle("Part 1: sort / stable_sort — 排序");

    // sort: 默认升序，不保证稳定性
    vector<int> v{5, 2, 8, 1, 9, 3, 7};
    ranges::sort(v);
    print("sort 升序: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 3 5 7 8 9

    // sort 降序: 使用 greater<>{} 或 lambda
    ranges::sort(v, greater{});
    print("sort 降序: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 9 8 7 5 3 2 1

    // 自定义排序: 按绝对值排序
    v = {-5, 2, -8, 1, 9, -3, 7};
    ranges::sort(v, [](int a, int b) { return abs(a) < abs(b); });
    print("按绝对值排序: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 -3 -5 7 -8 9

    // stable_sort: 保持相等元素的相对顺序
    vector<Item> items{{"C", 1}, {"A", 1}, {"B", 2}, {"D", 1}};
    // 按 rank 排序，但 rank 相同时保留原始顺序
    ranges::stable_sort(items, {}, &Item::rank);
    print("stable_sort (保持原顺序): ");
    for (auto& it : items) print("{}({}) ", it.name, it.rank);
    println("");
    // 预期: C(1) A(1) D(1) B(2) — 同 rank=1 保持 C, A, D 的原顺序
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: partial_sort / nth_element — 部分排序                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_partial() {
    lesson::print_subtitle("Part 2: partial_sort / nth_element — 部分排序");

    // partial_sort: 找出最小的 3 个并排好序
    vector<int> v{5, 2, 8, 1, 9, 3, 7, 4, 6};
    ranges::partial_sort(v, v.begin() + 3);
    print("partial_sort(前3个有序): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 3 ... (后续无序)

    // nth_element: 找到第 4 小的元素 (索引 3), 左侧都 <= 它, 右侧都 >= 它
    v = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    auto nth = v.begin() + 4;
    ranges::nth_element(v, nth);
    println("nth_element(第5小={}): ", *nth);
    print("  左侧: ");
    for (auto it = v.begin(); it != nth; ++it) print("{} ", *it);
    print(" 右侧: ");
    for (auto it = nth + 1; it != v.end(); ++it) print("{} ", *it);
    println("");
    // 预期: *nth = 5, 左侧都 ≤ 5, 右侧都 ≥ 5
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: binary_search / lower_bound / upper_bound / equal_range           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_binary_search() {
    lesson::print_subtitle("Part 3: binary_search / lower_bound / upper_bound");

    vector<int> v{1, 3, 5, 5, 5, 7, 9};  // 注意: 5 出现了 3 次
    // 必须已经有序!

    // binary_search: 判断是否存在
    println("存在 5? {}", ranges::binary_search(v, 5));   // 预期: true
    println("存在 6? {}", ranges::binary_search(v, 6));   // 预期: false

    // lower_bound: 第一个 >= target 的位置
    auto lo = ranges::lower_bound(v, 5);
    println("lower_bound(5) 位置: {}", distance(v.begin(), lo));  // 预期: 2

    // upper_bound: 第一个 > target 的位置
    auto hi = ranges::upper_bound(v, 5);
    println("upper_bound(5) 位置: {}", distance(v.begin(), hi));   // 预期: 5

    // equal_range: 返回 [lower_bound, upper_bound)
    auto [l, h] = ranges::equal_range(v, 5);
    println("5 出现了 {} 次", distance(l, h));  // 预期: 3

    // 实际应用: 在有序数组中查找插入位置
    auto pos = ranges::lower_bound(v, 6);
    println("6 应该插入在位置: {}", distance(v.begin(), pos));  // 预期: 5
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: merge / inplace_merge — 合并两个有序序列                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_merge() {
    lesson::print_subtitle("Part 4: merge / inplace_merge — 合并有序序列");

    // merge: 合并两个有序序列
    vector<int> a{1, 4, 7};
    vector<int> b{2, 3, 5, 8};
    vector<int> merged;
    ranges::merge(a, b, back_inserter(merged));
    print("merge: ");
    for (auto x : merged) print("{} ", x);
    println("");
    // 预期: 1 2 3 4 5 7 8

    // inplace_merge: 原地合并 (效率低于 merge，需要额外内存)
    vector<int> v{1, 4, 7,   2, 3, 5, 8};
    //             前半有序    后半有序
    ranges::inplace_merge(v, v.begin() + 3);
    print("inplace_merge: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 3 4 5 7 8
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: 堆操作 — push_heap / pop_heap / make_heap / sort_heap             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_heap() {
    lesson::print_subtitle("Part 5: 堆操作 — make_heap / push_heap / pop_heap");

    vector<int> v{3, 1, 5, 2, 8, 4};

    // make_heap: 将 vector 变为堆
    ranges::make_heap(v);
    println("make_heap 后 top: {}", v.front());  // 预期: 8 (最大)

    // push_heap: 先 push_back，再上滤
    v.push_back(10);
    ranges::push_heap(v);
    println("push_heap(10) 后 top: {}", v.front());  // 预期: 10

    // pop_heap: 将 top 移到末尾，其余重新堆化
    ranges::pop_heap(v);
    int top = v.back();
    v.pop_back();
    println("pop_heap 取出: {}, 新 top: {}", top, v.front());
    // 预期: 取出 10, 新 top: 8

    // sort_heap: 不断 pop_heap 直到全部有序
    v = {3, 1, 5, 2, 8, 4};
    ranges::make_heap(v);
    ranges::sort_heap(v);  // 堆排序: 原地，不稳定
    print("sort_heap (堆排序结果): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 3 4 5 8

    // is_heap: 判断是否为堆
    println("是否仍为堆: {}", ranges::is_heap(v));  // 预期: false (sort 后不是堆)
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 6: partition / stable_partition — 分区                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part6_partition() {
    lesson::print_subtitle("Part 6: partition / stable_partition — 分区");

    vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // partition: 偶数在前，奇数在后 (不保证稳定性)
    auto is_even = [](int x) { return x % 2 == 0; };
    auto mid = ranges::partition(v, is_even);

    print("partition (偶数在前): ");
    for (auto x : v) print("{} ", x);
    println("");
    println("分界点在索引: {}", distance(v.begin(), mid.begin()));
    // 预期: 分界点索引 = 5 (5个偶数)

    // is_partitioned: 检查是否已分区
    println("已分区: {}", ranges::is_partitioned(v, is_even));  // 预期: true

    // stable_partition: 保持相对顺序的分区
    v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    ranges::stable_partition(v, is_even);
    print("stable_partition (偶数在前, 保持原顺序): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 2 4 6 8 10 1 3 5 7 9
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void algorithms_sort_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: binary_search 需要在有序序列上使用
    vector<int> unsorted{3, 1, 4, 1, 5};
    // ranges::binary_search(unsorted, 1);  // UB! 序列无序
    lesson::print_note("二分查找族 (binary_search/lower_bound/upper_bound) 要求序列已排序");

    // 陷阱2: sort 的比较器必须满足严格弱序
    // 错误: [](int a, int b) { return a <= b; }  // 不是严格弱序
    lesson::print_note("sort 的比较器必须是严格弱序 (必须用 < 而非 <=)");

    // 陷阱3: nth_element 只保证第 n 个位置正确，两侧不保证有序
    lesson::print_note("nth_element 只保证第 n 个位置的元素正确，两侧不保证排序");

    // 陷阱4: pop_heap 后 top 被移到末尾，必须手动 pop_back
    lesson::print_note("pop_heap 不会自动 pop_back — 记得手动删除末尾元素");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void algorithms_sort_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 sort + unique 实现对一个 vector 的完全去重（无重复元素）");
    println("2. 用 partial_sort 找出 vector<int> 中最大的 5 个元素");
    println("3. 用 nth_element 找出一组数据的中位数");
    println("4. 用堆操作实现一个不使用 priority_queue 的优先队列");
    println("5. 分析 sort 和 stable_sort 在时间复杂度上的区别（为什么 stable_sort 更慢?)");
}

} // namespace stl_learn
