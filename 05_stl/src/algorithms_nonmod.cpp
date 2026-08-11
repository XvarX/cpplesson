// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/algorithms_nonmod.cpp — 非修改序列操作 教学演示函数实现                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/algorithms_nonmod.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: find / find_if / find_if_not — 查找元素                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_find() {
    lesson::print_subtitle("Part 1: find / find_if — 查找元素");

    vector<int> v{3, 7, 2, 9, 1, 5, 2, 8};

    // find: 查找等于指定值的元素
    auto it = ranges::find(v, 2);
    if (it != v.end()) {
        println("find(2) 位置: {}", distance(v.begin(), it));  // 预期: 2 (第0开始)
    }

    // find_if: 按条件查找
    auto it2 = ranges::find_if(v, [](int x) { return x > 7; });
    println("第一个 >7 的元素: {}", *it2);  // 预期: 9

    // find_if_not: 查找第一个不满足条件的
    auto it3 = ranges::find_if_not(v, [](int x) { return x % 2 == 1; });
    println("第一个偶数: {}", *it3);  // 预期: 2

    // 没找到返回 end()
    auto it4 = ranges::find(v, 999);
    println("find(999) 返回 end: {}", it4 == v.end());  // 预期: true
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: count / count_if — 统计元素数量                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_count() {
    lesson::print_subtitle("Part 2: count / count_if — 统计元素");

    vector<int> v{1, 2, 3, 2, 4, 2, 5, 2};

    // count: 统计等于指定值的数量
    println("元素 2 出现了 {} 次", ranges::count(v, 2));  // 预期: 4

    // count_if: 统计满足条件的数量
    auto is_even = [](int x) { return x % 2 == 0; };
    println("偶数有 {} 个", ranges::count_if(v, is_even));  // 预期: 5

    // 组合使用: 统计字符串中元音字母
    string text = "hello world from cpp";
    auto is_vowel = [](char c) {
        return string_view{"aeiouAEIOU"}.find(c) != string::npos;
    };
    println("'{}' 中有 {} 个元音", text,
            ranges::count_if(text, is_vowel));  // 预期: 5
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: search / find_end / find_first_of — 子序列查找                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_search() {
    lesson::print_subtitle("Part 3: search / find_first_of — 子序列查找");

    vector<int> v{1, 2, 3, 4, 1, 2, 3, 5};
    vector<int> pattern{2, 3};

    // search: 查找子序列第一次出现
    auto it = ranges::search(v, pattern);
    println("pattern {2,3} 首次出现在位置: {}", distance(v.begin(), it));
    // 预期: 1 (索引从0开始)

    // find_first_of: 查找 pattern 中任意元素第一次出现
    vector<int> targets{8, 9, 3};  // 查找 3 (因为在 v 中最早出现)
    auto it2 = ranges::find_first_of(v, targets);
    println("targets 中 {8,9,3} 第一个匹配: {}", *it2);  // 预期: 3

    // 实际场景: 在字符串中查找子串
    string msg = "Error: something went wrong at line 42";
    string needle = "line";
    if (ranges::search(msg, needle).begin() != msg.end()) {
        println("在消息中找到了 'line'");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: equal / mismatch — 比较两个序列                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_equal_mismatch() {
    lesson::print_subtitle("Part 4: equal / mismatch — 比较序列");

    vector<int> a{1, 2, 3, 4, 5};
    vector<int> b{1, 2, 3, 4, 5};
    vector<int> c{1, 2, 9, 4, 5};

    // equal: 判断两序列是否完全相等
    println("a == b: {}", ranges::equal(a, b));  // 预期: true
    println("a == c: {}", ranges::equal(a, c));  // 预期: false

    // mismatch: 找出第一个不匹配的位置
    auto [it_a, it_c] = ranges::mismatch(a, c);
    if (it_a != a.end()) {
        println("第一个差异: a[{}]={}, c[{}]={}",
                distance(a.begin(), it_a), *it_a,
                distance(c.begin(), it_c), *it_c);
        // 预期: a[2]=3, c[2]=9
    }

    // 比较两个长度不同的序列 (安全!)
    vector<int> shorter{1, 2, 3};
    println("equal(a, shorter): {}", ranges::equal(a, shorter));
    // 预期: false (长度不同)
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: all_of / any_of / none_of — 全称/存在量词判断                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_predicates() {
    lesson::print_subtitle("Part 5: all_of / any_of / none_of — 谓词判断");

    vector<int> scores{65, 72, 88, 91, 55, 79};

    // 全部及格?
    bool all_pass = ranges::all_of(scores, [](int s) { return s >= 60; });
    println("全部及格: {}", all_pass);  // 预期: false (55 不及格)

    // 有人满分?
    bool any_full = ranges::any_of(scores, [](int s) { return s == 100; });
    println("有满分: {}", any_full);  // 预期: false

    // 没有人低于 0?
    bool none_negative = ranges::none_of(scores, [](int s) { return s < 0; });
    println("没有负数: {}", none_negative);  // 预期: true

    // 实战: 验证字符串是否全部为数字
    string input = "12345";
    bool all_digit = ranges::all_of(input, ::isdigit);
    println("'{}' 全是数字: {}", input, all_digit);  // 预期: true
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 6: adjacent_find / lexicographical_compare                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part6_adjacent() {
    lesson::print_subtitle("Part 6: adjacent_find / lexicographical_compare");

    // adjacent_find: 查找第一组相等的相邻元素
    vector<int> v{1, 3, 5, 5, 7, 9};
    auto it = ranges::adjacent_find(v);
    if (it != v.end()) {
        println("第一组相邻相等: {} 和 {} (位置 {})",
                *it, *(it + 1), distance(v.begin(), it));
        // 预期: 5 和 5 (位置 2)
    }

    // adjacent_find 带自定义条件: 查找第一个降序的相邻元素
    vector<int> v2{1, 3, 5, 4, 7, 9};
    auto it2 = ranges::adjacent_find(v2, greater{});
    println("第一组相邻降序: {} > {}", *it2, *(it2 + 1));
    // 预期: 5 > 4

    // lexicographical_compare: 字典序比较
    string sa = "apple", sb = "banana";
    println("'{}' < '{}': {}", sa, sb,
            ranges::lexicographical_compare(sa, sb));
    // 预期: true
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: find 找不到时返回 end()，不解引用
    vector<int> v{1, 2, 3};
    auto it = ranges::find(v, 999);
    if (it != v.end()) {
        // println("{}", *it);  // 只有确认不是 end 后才解引用
    } else {
        println("未找到 999");  // 安全处理
    }
    lesson::print_note("总是检查 find 返回值是否为 end() 再解引用");

    // 陷阱2: equal 要求第二个范围至少和第一个一样长 (C++17 前)
    // C++14: equal(a.begin(), a.end(), b.begin());  // 如果 b 较短则 UB
    // C++17+: ranges::equal(a, b) 安全，自动检查长度
    lesson::print_note("使用 C++20 ranges::equal 代替传统双迭代器版本，它更安全");

    // 陷阱3: 对空容器调用 all_of 返回 true (vacuous truth)
    vector<int> empty;
    println("空容器 all_of: {}", ranges::all_of(empty, [](int x) { return false; }));
    // 预期: true! 数学上"空集的所有元素都满足任何性质"为真
    lesson::print_note("空容器上 all_of 总是返回 true (空真)，有时不符合直觉");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 find_if 在一个 vector<string> 中查找长度大于 5 的第一个单词");
    println("2. 用 count_if 统计一个整数数组中质数的个数");
    println("3. 用 search 判断一个序列是否为另一个序列的子序列");
    println("4. 用 all_of + any_of 验证一个密码字符串的复杂度规则");
    println("5. 用 adjacent_find 检查一个排序后的序列中是否存在重复元素");
}

} // namespace stl_learn
