// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/unordered_containers.hpp — 无序关联容器                                ║
// ║  主题: unordered_set / unordered_map                                        ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
#pragma once

#include "shared/lesson_utils.hpp"
#include <functional>
#include <print>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace stl_learn {

// ── 可复用的类型：Point 及其哈希函数 ──────────────────────────────────────────
// 用于演示自定义类型作为 unordered 容器键的用法

/// @brief 二维坐标点，可作为 unordered_set / unordered_map 的键
struct Point {
    int x, y;
    bool operator==(const Point& p) const = default;  // C++20
};

/// @brief Point 的哈希函数对象：组合 x 和 y 的哈希值
struct PointHash {
    size_t operator()(const Point& p) const {
        size_t h1 = std::hash<int>{}(p.x);
        size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 << 1);  // 简单组合方法
    }
};

// ── Part 1: unordered_set — 哈希集合 ─────────────────────────────────────────
// 是什么: 基于哈希表的集合，元素唯一，不保证顺序
// 为什么: 平均 O(1) 的插入/删除/查找，比 set (O(log n)) 快
// 什么时候用: 只需要"存在性判断"，不需要排序；追求查找速度
void part1_unordered_set();

// ── Part 2: unordered_map — 哈希映射 ─────────────────────────────────────────
// 是什么: 键值对的哈希表，键唯一，不保证顺序
// 为什么: 平均 O(1) 的键值查找，比 map (O(log n)) 快
// 什么时候用: 缓存、字典、频率统计等不需要按 key 排序的场景
void part2_unordered_map();

// ── Part 3: Bucket 与 Load Factor ────────────────────────────────────────────
// 是什么: 哈希表内部由多个 bucket 组成，每个 bucket 存储哈希值相同的元素
//         load_factor = size / bucket_count，控制哈希碰撞概率
// 为什么: 了解内部机制帮助调优性能; load_factor 过高时查找退化为 O(n)
// 什么时候用: 性能调优时; 预估元素数量后预留 bucket
void part3_bucket();

// ── Part 4: 自定义哈希函数 ────────────────────────────────────────────────────
// 是什么: 对自定义类型提供哈希函数，使它们能存入 unordered 容器
// 为什么: unordered 容器需要计算键的哈希值; 默认 std::hash 不支持自定义类型
// 什么时候用: 使用自定义类型作为 unordered_map/set 的键时
void part4_custom_hash();

// ── Part 5: unordered vs ordered 容器对比 ─────────────────────────────────────
void part5_comparison();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void unordered_containers_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void unordered_containers_exercises();

} // namespace stl_learn
