// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/sequential_containers.cpp — 顺序容器 教学演示函数实现                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/sequential_containers.hpp"
#include <algorithm>
#include <ranges>

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::vector — 动态数组，内存连续                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_vector() {
    lesson::print_subtitle("Part 1: std::vector — 动态数组，连续内存");

    vector<int> v{1, 2, 3};
    println("初始 size={}, capacity={}", v.size(), v.capacity());
    // 预期: size=3, capacity=3 (或大于)

    v.push_back(4);  // 尾部插入 O(1) 摊还，可能触发 reallocation
    println("push_back(4) 后: size={}, capacity={}", v.size(), v.capacity());

    // 随机访问 O(1)
    println("v[0]={}, v[3]={}", v[0], v[3]);

    // 在中间插入 O(n) — 需要移动元素
    v.insert(v.begin() + 2, 99);
    println("insert(pos2, 99) 后:");
    for (auto x : v) print("{} ", x);
    println("");
    // 预期: 1 2 99 3 4

    // reserve 预分配，避免多次 reallocation
    v.reserve(100);
    println("reserve(100) 后: capacity={}", v.capacity());
    // 预期: capacity >= 100
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::deque — 双端队列，分段连续内存                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_deque() {
    lesson::print_subtitle("Part 2: std::deque — 双端队列");

    deque<int> d{2, 3, 4};
    d.push_front(1);  // 头部插入 O(1) — vector 做不了
    d.push_back(5);   // 尾部插入 O(1)

    print("deque: ");
    for (auto x : d) print("{} ", x);
    println("");
    // 预期: 1 2 3 4 5

    println("d[2]={}", d[2]);  // 随机访问 O(1)，但比 vector 稍慢（需定位到块）
    // 预期: 3

    // 注意: deque 中间插入仍为 O(n)，且迭代器可能全部失效
    d.insert(d.begin() + 2, 99);
    print("insert(pos2, 99) 后: ");
    for (auto x : d) print("{} ", x);
    println("");
    // 预期: 1 2 99 3 4 5
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: std::list — 双向链表                                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_list() {
    lesson::print_subtitle("Part 3: std::list — 双向链表");

    list<int> lst{1, 2, 4, 5};
    // 在任意位置插入 O(1) (已知迭代器位置)
    auto it = next(lst.begin(), 2);  // 指向 4
    lst.insert(it, 3);

    print("list: ");
    for (auto x : lst) print("{} ", x);
    println("");
    // 预期: 1 2 3 4 5

    // splice — O(1) 拼接整个链表
    list<int> other{10, 11, 12};
    lst.splice(lst.end(), other);  // 将 other 所有元素移到 lst 末尾
    println("other 是否为空: {}", other.empty());
    println("拼接后 list 大小: {}", lst.size());
    // 预期: other 为空, list 大小为 8

    // 缺点: 不支持随机访问，内存不连续（缓存不友好）
    // 没有 operator[]
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: std::forward_list — 单向链表                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_forward_list() {
    lesson::print_subtitle("Part 4: std::forward_list — 单向链表");

    forward_list<int> fl{1, 2, 3};
    // 只能在前面插入（因为没有前驱指针）
    fl.push_front(0);

    // 在指定位置之后插入 (insert_after)
    fl.insert_after(fl.begin(), 99);

    print("forward_list: ");
    for (auto x : fl) print("{} ", x);
    println("");
    // 预期: 0 99 1 2 3

    // 没有 size() 方法! 只能用 distance
    println("元素个数: {}", distance(fl.begin(), fl.end()));
    // 预期: 5
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: std::array — 固定大小数组                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_array() {
    lesson::print_subtitle("Part 5: std::array — 固定大小数组");

    array<int, 5> arr{10, 20, 30, 40, 50};
    // 大小是编译期常量: std::array<int, 5>

    println("arr.size() = {}", arr.size());   // 预期: 5
    println("arr[2] = {}", arr[2]);           // 预期: 30

    // 支持范围 for、STL 算法
    ranges::sort(arr, greater{});
    print("降序排序: ");
    for (auto x : arr) print("{} ", x);
    println("");
    // 预期: 50 40 30 20 10

    // array 在栈上分配，没有堆开销，性能等于 C 数组
    // 注意: array<int,3> 与 array<int,4> 是不同类型！
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 6: 容器选择指南与复杂度对比                                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part6_choose_container() {
    lesson::print_subtitle("Part 6: 容器选择指南");

    println("┌─────────────────┬──────────┬──────────┬──────────┬──────────────┐");
    println("│    操作          │ vector   │  deque   │   list   │ forward_list │");
    println("├─────────────────┼──────────┼──────────┼──────────┼──────────────┤");
    println("│ 随机访问         │  O(1)    │  O(1)    │   —      │     —        │");
    println("│ 尾部插入/删除    │ O(1)*    │  O(1)    │  O(1)    │     —        │");
    println("│ 头部插入/删除    │   —      │  O(1)    │  O(1)    │   O(1)       │");
    println("│ 中间插入/删除    │  O(n)    │  O(n)    │  O(1)**  │  O(1)**      │");
    println("│ 内存连续性       │   是     │   分段    │   否     │    否        │");
    println("│ 迭代器失效(插入) │ 可能全部 │ 可能全部 │   不失效 │   不失效     │");
    println("└─────────────────┴──────────┴──────────┴──────────┴──────────────┘");
    println("* 摊还  ** 已知位置");

    println("\n快速决策表:");
    println("  默认选择        → vector");
    println("  需要头部插入    → deque");
    println("  频繁中间插入    → list");
    println("  内存敏感+单向   → forward_list");
    println("  编译期固定大小  → array");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void sequential_containers_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: 循环中修改 vector 导致迭代器失效
    if (false) {  // 用 false 保护危险代码
        vector<int> v{1, 2, 3, 4};
        for (auto it = v.begin(); it != v.end(); ++it) {
            if (*it == 2) v.erase(it);  // 迭代器失效! 应写 it = v.erase(it);
        }
    }
    lesson::print_note("循环中 erase: 必须写 it = v.erase(it);");

    // 陷阱2: vector<bool> 不是真正的容器，不返回 bool&
    vector<bool> vb{true, false};
    // auto& ref = vb[0];  // 编译错误! vector<bool>::reference 不是 bool&
    auto val = vb[0];  // OK: 拷贝
    lesson::print_note("vector<bool> 做了特化，不返回 bool& 而是代理对象");

    // 陷阱3: push_front 在 vector 上不可用
    // v.push_front(1);  // 编译错误! 用 deque 或 list
    lesson::print_note("vector 没有 push_front，可以用 v.insert(v.begin(), x) 但 O(n)");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void sequential_containers_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 vector<int> 实现一个简单的栈（push_back/pop_back/back）");
    println("2. 用 deque 模拟一个滑动窗口，维护窗口内最大值");
    println("3. 比较 vector 和 list 在中间插入 10000 个元素的性能差异");
    println("4. 实现一个使用 array<int, 26> 统计字母频率的函数");
    println("5. 什么时候应该用 forward_list 而不是 list? (思考内存开销)");
}

} // namespace stl_learn
