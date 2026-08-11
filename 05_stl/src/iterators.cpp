// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/iterators.cpp — 迭代器 教学演示函数实现                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/iterators.hpp"
#include <typeinfo>

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: 迭代器类别 (Iterator Categories)                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_categories() {
    lesson::print_subtitle("Part 1: 迭代器类别 (Iterator Categories)");

    println("6 种迭代器类别 (功能递增):");
    println("  Input          — 只读单次遍历    (istream_iterator)");
    println("  Output         — 只写单次遍历    (ostream_iterator)");
    println("  Forward        — 可多次正向遍历  (forward_list::iterator)");
    println("  Bidirectional  — 可双向移动      (list/set/map::iterator)");
    println("  RandomAccess   — 可随机跳转      (vector/deque::iterator)");
    println("  Contiguous     — 连续内存        (vector/array::iterator, C++17)");

    println("");

    // 查看各容器迭代器的 category
    println("vector<int>::iterator 是 contiguous:         {}",
        same_as<iterator_traits<vector<int>::iterator>::iterator_category,
                contiguous_iterator_tag>);

    println("list<int>::iterator 是 bidirectional:        {}",
        same_as<iterator_traits<list<int>::iterator>::iterator_category,
                bidirectional_iterator_tag>);

    println("forward_list<int>::iterator 是 forward:      {}",
        same_as<iterator_traits<forward_list<int>::iterator>::iterator_category,
                forward_iterator_tag>);
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: iterator_traits — 提取迭代器属性                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_traits() {
    lesson::print_subtitle("Part 2: iterator_traits — 提取迭代器属性");

    using Iter = vector<int>::iterator;

    // 5 种 traits
    println("value_type:        {}", typeid(iterator_traits<Iter>::value_type).name());
    println("difference_type:   {}", typeid(iterator_traits<Iter>::difference_type).name());
    println("pointer:           {}", typeid(iterator_traits<Iter>::pointer).name());
    println("reference:         {}", typeid(iterator_traits<Iter>::reference).name());
    println("iterator_category: {}",
        typeid(iterator_traits<Iter>::iterator_category).name());

    // 实战: 使用 iterator_traits 编写泛型 advance
    // (标准库 std::advance 已经做了这件事)
    vector<int> v{1, 2, 3, 4, 5};
    auto it = v.begin();
    advance(it, 3);  // 对随机访问迭代器: O(1), 对 forward: O(n)
    println("advance(it, 3) 后: {}", *it);  // 预期: 4
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 迭代器辅助: 插入迭代器、流迭代器、反向迭代器                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_adaptors() {
    lesson::print_subtitle("Part 3: 迭代器适配器");

    // ── 插入迭代器 ──
    vector<int> src{1, 2, 3};
    vector<int> dest{10, 20};

    // back_inserter: 调用 push_back
    copy(src.begin(), src.end(), back_inserter(dest));
    print("back_inserter: ");
    for (auto x : dest) print("{} ", x);
    println("");
    // 预期: 10 20 1 2 3

    // front_inserter: 调用 push_front (仅 deque/list 支持)
    list<int> lst{10, 20};
    copy(src.begin(), src.end(), front_inserter(lst));
    print("front_inserter: ");
    for (auto x : lst) print("{} ", x);
    println("");
    // 预期: 3 2 1 10 20

    // inserter: 在指定位置调用 insert
    vector<int> dest2{10, 20};
    copy(src.begin(), src.end(), inserter(dest2, dest2.begin() + 1));
    print("inserter(pos=1): ");
    for (auto x : dest2) print("{} ", x);
    println("");
    // 预期: 10 1 2 3 20

    // ── 反向迭代器 ──
    vector<int> rv{1, 2, 3, 4, 5};
    print("反向遍历: ");
    for (auto it = rv.rbegin(); it != rv.rend(); ++it) {
        print("{} ", *it);
    }
    println("");
    // 预期: 5 4 3 2 1
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 自定义迭代器 (简化版 range 迭代器)                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_custom_iterator() {
    lesson::print_subtitle("Part 4: 自定义迭代器");

    // Range 类已在 iterators.hpp 中定义
    // 现在 Range 可以使用 range-for 和 STL 算法
    Range r(0, 5);
    print("Range(0,5): ");
    for (auto x : r) print("{} ", x);
    println("");
    // 预期: 0 1 2 3 4

    // 配合 STL 算法
    auto it = ranges::find(r, 3);
    if (it != r.end()) println("找到了 {}", *it);  // 预期: 找到了 3
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: C++20 ranges — 现代迭代器抽象                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_ranges() {
    lesson::print_subtitle("Part 5: C++20 ranges — 现代迭代器抽象");

    vector<int> v{5, 2, 8, 1, 9, 3, 7};

    // 传统方式: 迭代器对
    auto old_it = find(v.begin(), v.end(), 8);
    println("传统 find(8): {}", *old_it);

    // C++20 ranges: 直接传容器
    auto new_it = ranges::find(v, 8);
    println("ranges::find(8): {}", *new_it);

    // 视图 + 管道语法
    auto even = v | views::filter([](int x) { return x % 2 == 0; });
    print("偶数: ");
    for (auto x : even) print("{} ", x);
    println("");
    // 预期: 2 8

    // 组合多个视图
    print("前3个偶数: ");
    for (auto x : v | views::filter([](int x) { return x % 2 == 0; })
                    | views::take(3)) {
        print("{} ", x);
    }
    println("");
    // 预期: 2 8 (只有2个偶数)
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: end() 迭代器解引用
    vector<int> v{1, 2, 3};
    // println("{}", *v.end()); // 未定义行为!
    lesson::print_note("永远不要解引用 end() 迭代器 — 它是\"哨兵\"不是元素");

    // 陷阱2: 插入导致迭代器失效
    // auto it = v.begin(); v.push_back(4); // it 可能已失效
    lesson::print_note("修改容器后要小心迭代器是否已失效");

    // 陷阱3: 比较不同容器的迭代器是 UB
    vector<int> a{1, 2}, b{3, 4};
    // if (a.begin() != b.begin()) {} // UB! 只能比较同一容器的迭代器
    lesson::print_note("不要比较不同容器的迭代器 — 属于未定义行为");

    // 陷阱4: reverse_iterator 的 base() 偏移了一个位置
    vector<int> rv{1, 2, 3};
    auto rit = rv.rbegin();  // 指向 3
    auto bit = rit.base();   // 指向 3 之后的 end()!
    println("rbegin 指向 {}, 其 base() 指向 end 前一个位置", *rit);
    lesson::print_note("reverse_iterator::base() 指向的元素比原迭代器偏移了 1");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part_exercises() {
    lesson::print_subtitle("练习");

    println("1. 为一个简单的动态数组类实现随机访问迭代器");
    println("2. 用 ostream_iterator 将 vector<int> 的内容输出到 cout");
    println("3. 用 istream_iterator 从 cin 读取整数并存入 vector");
    println("4. 编写一个泛型函数 print_range，接受任意 range 并打印其元素");
    println("5. 解释为什么 set 的迭代器是双向迭代器而不是随机访问迭代器");
}

} // namespace stl_learn
