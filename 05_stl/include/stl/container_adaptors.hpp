// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/container_adaptors.hpp — 容器适配器                                    ║
// ║  主题: stack / queue / priority_queue                                       ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
#pragma once

#include "shared/lesson_utils.hpp"
#include <deque>
#include <functional>
#include <list>
#include <print>
#include <queue>
#include <stack>
#include <string>
#include <vector>

namespace stl_learn {

// ── 可复用的类型：Task — 用于优先队列自定义比较器演示 ─────────────────────────

/// @brief 任务结构体，包含名称和优先级（数字越大优先级越高）
struct Task {
    std::string name;
    int priority;
};

// ── Part 1: std::stack — 栈 (LIFO: 后进先出) ──────────────────────────────────
// 是什么: 容器适配器，包装底层容器只暴露栈操作: push/pop/top
// 为什么: 限制接口即明确意图; 栈语义让代码更清晰，减少误用
// 什么时候用: 函数调用栈模拟、括号匹配、表达式求值、撤销操作、DFS
void part1_stack();

// ── Part 2: std::queue — 队列 (FIFO: 先进先出) ─────────────────────────────────
// 是什么: 容器适配器，包装底层容器只暴露队列操作: push/pop/front/back
// 为什么: 限制接口即明确意图; FIFO 语义用于按序处理
// 什么时候用: 任务调度、消息队列、BFS、生产者-消费者模式
void part2_queue();

// ── Part 3: std::priority_queue — 优先队列 (最大/最小堆) ───────────────────────
// 是什么: 容器适配器，保证 top() 始终是"优先级最高"的元素（默认最大元素）
// 为什么: 需要自动维护优先级顺序; 基于堆实现 O(log n) 的插入和 O(1) 取最值
// 什么时候用: 任务调度（按优先级）、Dijkstra 算法、Top-K 问题、事件驱动模拟
void part3_priority_queue();

// ── Part 4: 优先队列 — 自定义比较器 ──────────────────────────────────────────
// 是什么: 通过比较器自定义优先级规则（如按结构体字段排序）
// 为什么: 默认 greater/less 不足以表达复杂优先级
// 什么时候用: 自定义类型的优先级队列; 需要非标准优先级规则
void part4_custom_pq();

// ── Part 5: 适配器模式总结 ────────────────────────────────────────────────────
void part5_summary();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void part_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void part_exercises();

} // namespace stl_learn
