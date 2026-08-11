// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/algorithms_nonmod.hpp — 非修改序列操作                                  ║
// ║  主题: find/count/search/equal/mismatch/谓词判断                             ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
#pragma once

#include "shared/lesson_utils.hpp"
#include <algorithm>
#include <numeric>
#include <print>
#include <ranges>
#include <string>
#include <vector>

namespace stl_learn {

// ── Part 1: find / find_if / find_if_not — 查找元素 ───────────────────────────
// 是什么: 线性查找第一个匹配条件的元素，返回迭代器
// 为什么: 替代手写 for 循环，语义更清晰，编译器可能做优化
// 什么时候用: 需要在无序容器中查找元素; 需要自定义查找条件
void part1_find();

// ── Part 2: count / count_if — 统计元素数量 ──────────────────────────────────
// 是什么: 统计等于某值或满足某条件的元素个数
// 为什么: 比手写循环更简洁，且可以利用算法库的内部优化
// 什么时候用: 需要统计出现次数或满足条件的数量
void part2_count();

// ── Part 3: search / find_end / find_first_of — 子序列查找 ────────────────────
// 是什么: search 查找子序列第一次出现; find_end 查找最后一次; find_first_of 查找任意匹配
// 为什么: 需要在一个序列中匹配子模式
// 什么时候用: 文本匹配、字节流匹配、模式识别
void part3_search();

// ── Part 4: equal / mismatch — 比较两个序列 ───────────────────────────────────
// 是什么: equal 判断两序列是否相等; mismatch 找出第一个不匹配的位置
// 为什么: 安全比较（不会越界，可指定长度），比裸循环更清晰
// 什么时候用: 比较两个容器内容、验证算法输出
void part4_equal_mismatch();

// ── Part 5: all_of / any_of / none_of — 全称/存在量词判断 ──────────────────────
// 是什么: 判断序列中 ALL/ANY/NONE 元素满足某条件
// 为什么: 将数学量词直接映射为代码，比手写循环语义更清晰
// 什么时候用: 数据验证、输入检查、前置条件判断
void part5_predicates();

// ── Part 6: adjacent_find / lexicographical_compare ────────────────────────────
// 是什么: adjacent_find 查找相邻重复/满足关系的元素; lexicographical_compare 字典序比较
// 为什么: 检测序列中的局部模式，实现自定义排序规则比较
// 什么时候用: 检测重复、查找相邻满足某关系的元素
void part6_adjacent();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void part_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void part_exercises();

} // namespace stl_learn
