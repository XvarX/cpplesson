#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/algorithms_sort.hpp — 排序与二分查找                                    ║
// ║  主题: sort/stable_sort/merge/heap/partition/binary_search                   ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include <algorithm>
#include <cassert>
#include <print>
#include <ranges>
#include <string>
#include <vector>

namespace stl_learn {

// ── 可复用的类型：Item — 用于演示 stable_sort 的稳定性 ────────────────────────

/// @brief 用于演示 stable_sort 保持相对顺序的物品结构体
struct Item {
    std::string name;
    int rank;
};

// ── Part 1: sort / stable_sort — 排序核心 ────────────────────────────────────
// 是什么: sort 为快速排序(内省排序), O(n log n); stable_sort 保持等值元素相对顺序
// 为什么: sort 是默认选择(快)，stable_sort 用于需要保持稳定性的场景
// 什么时候用: 默认用 sort; 需要稳定性(如先按姓名后按年龄)用 stable_sort
void part1_sort();

// ── Part 2: partial_sort / nth_element — 部分排序 ────────────────────────────
// 是什么: partial_sort 只排前 N 个; nth_element 只保证第 N 个位置正确
// 为什么: 不需要全排序时可以节省时间: partial_sort O(n log k), nth_element O(n)
// 什么时候用: Top-K 问题用 partial_sort; 中位数/分位数用 nth_element
void part2_partial();

// ── Part 3: binary_search / lower_bound / upper_bound / equal_range ───────────
// 是什么: 在有序序列上做二分查找, O(log n)
// 为什么: 比线性查找快, 是"排序后多次查询"的标准模式
// 什么时候用: 有序序列中的查找、插入位置定位、范围查询
void part3_binary_search();

// ── Part 4: merge / inplace_merge — 合并两个有序序列 ──────────────────────────
// 是什么: merge 合并两个有序序列; inplace_merge 原地合并同一容器的两个有序部分
// 为什么: O(n+m) 的高效合并; 归并排序的核心操作
// 什么时候用: 合并两个有序列表; 将两个连续有序段合并为一
void part4_merge();

// ── Part 5: 堆操作 — push_heap / pop_heap / make_heap / sort_heap ──────────────
// 是什么: 在 vector 上维护堆结构 (默认最大堆)，优先队列的底层实现
// 为什么: 比 priority_queue 更灵活（可以修改中间元素后重新堆化）
// 什么时候用: 需要更灵活的堆操作；实现自定义优先队列；图算法
void part5_heap();

// ── Part 6: partition / stable_partition — 分区 ──────────────────────────────
// 是什么: 将序列按谓词分成两部分: 满足条件的在前，不满足的在后
// 为什么: 快速排序的核心; 将数据按性质分组
// 什么时候用: 分离有效/无效数据、奇偶分组、快排实现
void part6_partition();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void algorithms_sort_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void algorithms_sort_exercises();

} // namespace stl_learn
