#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  stl/chrono_random.hpp — 时间库与随机数库                                   ║
// ║  主题: duration/time_point/clock、字面量、随机引擎与分布                      ║
// ║  声明教学演示函数，供示例程序调用                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include <algorithm>
#include <chrono>
#include <map>
#include <print>
#include <random>
#include <ratio>
#include <string>
#include <thread>
#include <vector>

namespace stl_learn {

// ── Part 1: std::chrono::duration — 时间段 ────────────────────────────────────
// 是什么: 编译期类型安全的时间长度，模板参数为 Rep(数值类型) 和 Period(秒的分数)
// 为什么: 防止单位混淆 (不会把秒当毫秒用); 编译期单位转换; 类型安全
// 什么时候用: 任何需要表示时间间隔的地方 (替代裸 int/float)
void part1_duration();

// ── Part 2: time_point 与 clock ──────────────────────────────────────────────
// 是什么: time_point = 某个时钟的某个时刻; clock = 时间源
// 为什么: 统一的时间测量接口，可跨平台; 比 time()/gettimeofday 更现代化
// 什么时候用: 测量性能、记录时间戳、定时器
void part2_time_point();

// ── Part 3: C++14 时间字面量 ─────────────────────────────────────────────────
// 是什么: 类型后缀，让时间表示更自然: 5s, 100ms, 2h, 30min
// 为什么: 代码可读性大幅提升; 编译器自动转换为正确的 duration 类型
// 什么时候用: 任何时间相关的字面量
void part3_literals();

// ── Part 4: <random> — 随机数引擎 (Engine) ────────────────────────────────────
// 是什么: 确定性的伪随机数生成器; 引擎产生原始随机位，分布将其映射到所需范围
// 为什么: 替换 rand() (rand() 质量差、范围不均匀、线程不安全)
// 什么时候用: 任何需要随机数的场景 (游戏、模拟、采样、密码学非安全场景)
void part4_random_engine();

// ── Part 5: 常用分布 (distributions) ──────────────────────────────────────────
// 是什么: 将引擎的均匀输出转换为特定概率分布
// 为什么: 真实场景很少需要均匀分布; 正态分布、伯努利等才符合实际需求
// 什么时候用: 模拟真实世界的随机现象; 游戏中的伤害概率; 蒙特卡洛方法
void part5_distributions();

// ── Part 6: std::ratio — 编译期有理数 ────────────────────────────────────────
// 是什么: 编译期有理数常量, 如 ratio<1, 1000> 表示 1/1000 (毫)
// 为什么: chrono::duration 的 Period 参数; 任何需要编译期比例的地方
// 什么时候用: 定义自定义 duration; 编译期单位转换
void part6_ratio();

// ── 常见陷阱 ─────────────────────────────────────────────────────────────────
void chrono_random_pitfalls();

// ── 练习 ────────────────────────────────────────────────────────────────────
void chrono_random_exercises();

} // namespace stl_learn
