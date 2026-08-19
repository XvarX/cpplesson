// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/algorithms_mod.hpp — 修改序列操作                                       ║
// ║  主题: copy/transform/remove/replace/generate/fill/unique                    ║
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

// ── Part 1: copy / copy_if / copy_n — 复制元素 ────────────────────────────────
// 是什么: 将元素从一个范围复制到另一个范围
// 为什么: 安全复制（配合 back_inserter 自动扩容），比手写循环更清晰
// 什么时候用: 数据搬迁、过滤复制、备份
void part1_copy();

// ── Part 2: transform — 转换/映射元素 ──────────────────────────────────────────
// 是什么: 对每个元素应用函数，生成新的序列（单范围版/双范围版）
// 为什么: 函数式编程中的 map 操作; 替代手写的 for+push_back 模式
// 什么时候用: 数据转换、类型转换、元素映射
void part2_transform();

// ── Part 3: remove / remove_if — "移除"元素 (重点: remove-erase 惯用法) ────────
// 是什么: remove 并不真正删除元素! 它将"不移除"的元素移到前面，返回新的逻辑末尾
// 为什么: 算法看不到容器，只能重排元素; 真正的删除需要容器的 erase 方法
// 什么时候用: 配合 erase 从容器中移除满足条件的元素
void part3_remove();

// ── Part 4: replace / replace_if / replace_copy — 替换元素 ─────────────────────
// 是什么: 将满足条件的元素替换为新值
// 为什么: 批量替换比循环更简洁; replace_copy 可以原位替换或生成新序列
// 什么时候用: 数据清洗、标准化、脱敏
void part4_replace();

// ── Part 5: fill / generate / reverse / rotate / unique ────────────────────────
// 是什么: fill 填充定值; generate 填充函数生成值; reverse/rotate 重排; unique 去重
// 为什么: 这些是容器初始化、数据转换的常用操作
// 什么时候用: 初始化数组、打乱后的恢复、去除相邻重复
void part5_more_mod();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void algorithms_mod_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void algorithms_mod_exercises();

} // namespace stl_learn
