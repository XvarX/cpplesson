// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/algorithms_mod.cpp — 修改序列操作 教学演示函数实现                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/algorithms_mod.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: copy / copy_if / copy_n — 复制元素                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_copy() {
    lesson::print_subtitle("Part 1: copy / copy_if — 复制元素");

    vector<int> src{1, 2, 3, 4, 5, 6};

    // copy: 全部复制
    vector<int> dest;
    ranges::copy(src, back_inserter(dest));
    print("copy 全部: ");
    for (auto x : dest) print("{} ", x);
    println("");
    // 预期: 1 2 3 4 5 6

    // copy_if: 条件复制
    vector<int> evens;
    ranges::copy_if(src, back_inserter(evens),
                    [](int x) { return x % 2 == 0; });
    print("copy_if 偶数: ");
    for (auto x : evens) print("{} ", x);
    println("");
    // 预期: 2 4 6

    // copy_n: 复制前 N 个
    vector<int> first3;
    ranges::copy_n(src.begin(), 3, back_inserter(first3));
    print("copy_n 前3个: ");
    for (auto x : first3) print("{} ", x);
    println("");
    // 预期: 1 2 3
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: transform — 转换/映射元素                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_transform() {
    lesson::print_subtitle("Part 2: transform — 转换/映射");

    vector<int> nums{1, 2, 3, 4, 5};

    // 单范围 transform: 每个元素 -> 新值
    vector<int> squares(nums.size());
    ranges::transform(nums, squares.begin(),
                      [](int x) { return x * x; });
    print("平方: ");
    for (auto x : squares) print("{} ", x);
    println("");
    // 预期: 1 4 9 16 25

    // 双范围 transform: 两序列逐元素运算
    vector<int> a{1, 2, 3};
    vector<int> b{10, 20, 30};
    vector<int> sums(a.size());
    ranges::transform(a, b, sums.begin(), plus{});
    print("逐元素相加: ");
    for (auto x : sums) print("{} ", x);
    println("");
    // 预期: 11 22 33

    // 实际场景: 字符串转大写 (投影)
    string word = "hello";
    ranges::transform(word, word.begin(), ::toupper);
    println("转大写: {}", word);  // 预期: HELLO
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: remove / remove_if — "移除"元素 (重点: remove-erase 惯用法)        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_remove() {
    lesson::print_subtitle("Part 3: remove / remove_if — remove-erase 惯用法");

    vector<int> v{1, 2, 3, 2, 4, 2, 5};
    println("原始: size={}", v.size());  // 预期: 7

    // 仅 remove — 不改变 size!
    // 注意: ranges::remove 返回的是"被移除元素的尾段"子区间 [新逻辑末尾, end)
    auto removed = ranges::remove(v, 2);
    println("remove(2) 后 size 仍为: {} (只是重排了元素!)", v.size());  // 7

    // remove 之后、erase 之前的状态
    print("remove 后 [begin, 新逻辑末尾): ");
    for (auto it = v.begin(); it != removed.begin(); ++it) print("{} ", *it);
    println("");
    // 预期: 1 3 4 5

    // ☆ remove-erase 惯用法: 一步到位真正删除
    v = {1, 2, 3, 2, 4, 2, 5};  // 重置
    erase(v, 2);  // C++20: 一步完成 remove+erase, 或:
    // v.erase(remove(v.begin(), v.end(), 2), v.end());  // C++20前
    println("erase(2) 后 size={}", v.size());  // 预期: 4

    // remove_if: 按条件移除
    v = {1, 2, 3, 4, 5, 6, 7, 8};
    erase_if(v, [](int x) { return x % 2 == 0; });  // C++20
    print("移除偶数后: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 3 5 7
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: replace / replace_if / replace_copy — 替换元素                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_replace() {
    lesson::print_subtitle("Part 4: replace / replace_if — 替换元素");

    // replace: 将特定值替换
    vector<int> v{1, 2, 3, 2, 4};
    ranges::replace(v, 2, 99);
    print("replace(2->99): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 99 3 99 4

    // replace_if: 按条件替换
    v = {1, -2, 3, -4, 5};
    ranges::replace_if(v, [](int x) { return x < 0; }, 0);
    print("负数替换为 0: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 0 3 0 5

    // replace_copy: 替换并输出到新序列 (原序列不变)
    vector<int> src{1, 2, 3};
    vector<int> dest;
    ranges::replace_copy(src, back_inserter(dest), 2, 200);
    print("replace_copy: src 不变 = ");
    for (auto x : src) print("{} ", x);
    print(", dest = ");
    for (auto x : dest) print("{} ", x);
    println("");
    // 预期: src: 1 2 3, dest: 1 200 3
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: fill / generate / reverse / rotate / unique                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_more_mod() {
    lesson::print_subtitle("Part 5: fill / generate / reverse / rotate / unique");

    // fill: 填充相同值
    vector<int> v(5);
    ranges::fill(v, 42);
    print("fill(42): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 42 42 42 42 42

    // generate: 用函数生成值
    int counter = 0;
    ranges::generate(v, [&] { return counter += 10; });
    print("generate(+10): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 10 20 30 40 50

    // reverse: 反转序列
    v = {1, 2, 3, 4, 5};
    ranges::reverse(v);
    print("reverse: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 5 4 3 2 1

    // rotate: 循环左移/右移
    v = {1, 2, 3, 4, 5};
    ranges::rotate(v, v.begin() + 2);  // 将前2个元素移到末尾
    print("rotate(left 2): ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 3 4 5 1 2

    // unique: 去除相邻重复 (需要先排序)
    v = {1, 1, 2, 2, 2, 3, 3, 4};
    auto uniq_end = ranges::unique(v);
    v.erase(uniq_end.begin(), v.end());  // 真正删除
    print("unique 后: ");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 3 4
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void algorithms_mod_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: remove 不真正删除元素，必须配合 erase
    lesson::print_note("remove 只重排元素返回新末尾，必须配合 erase 才真正删除 (remove-erase idiom)");

    // 陷阱2: copy 时目标范围必须有足够空间
    vector<int> src{1, 2, 3};
    vector<int> small(1);  // 只有 1 个元素的空间!
    // ranges::copy(src, small.begin());  // UB! 目标不够大
    // 正确: 使用 back_inserter
    lesson::print_note("copy 到已有容器时，用 back_inserter 避免空间不足");

    // 陷阱3: transform 目标范围大小需要和源范围匹配
    // 对双范围 transform，目标至少和较小的源范围一样大
    lesson::print_note("transform 双范围版: 目标至少与较小的源范围一样大");

    // 陷阱4: unique 只移除相邻重复
    // 要移除所有重复，必须先排序
    lesson::print_note("unique 只去相邻重复，去所有重复要先 sort 再 unique");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void algorithms_mod_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 copy_if 从一个数字列表中提取所有质数到新列表");
    println("2. 用 transform 将 vector<string> 中所有单词转换为大写");
    println("3. 用 erase_if (C++20) 从 vector 中移除所有负数");
    println("4. 用 generate + lambda 生成前 20 个斐波那契数");
    println("5. 解释为什么 remove 不真正删除元素 (提示: 算法看不到容器)");
}

} // namespace stl_learn
