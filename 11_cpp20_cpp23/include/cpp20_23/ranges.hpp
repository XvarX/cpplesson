#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: Ranges 库 — views、管道操作符、range adaptors、ranges::to             ║
// ║                                                                            ║
// ║  Ranges 核心思想:                                                            ║
// ║  传统 STL: sort(v.begin(), v.end())  — 迭代器对，繁琐                      ║
// ║  Ranges:   std::ranges::sort(v)       — 传容器，简洁                        ║
// ║  View 管道: v | filter(...) | transform(...) | take(...) — 懒求值流水线     ║
// ║                                                                            ║
// ║  View 不会立即执行，只在迭代时才计算 (lazy evaluation):                      ║
// ║   std::views::filter    — 过滤满足条件的元素                                 ║
// ║   std::views::transform — 对每个元素做映射                                   ║
// ║   std::views::take      — 取前 N 个元素                                     ║
// ║   std::views::drop      — 跳过前 N 个元素                                   ║
// ║   std::views::zip       — 将多个 range "拉链"组合  (C++23)                  ║
// ║   std::views::enumerate — 给每个元素附加索引       (C++23)                   ║
// ║                                                                            ║
// ║  管道操作符 | — 将多个 view 串成处理流水线，从数据源流向最终结果。          ║
// ║  对比: 传统 for 循环是"怎么做"，管道是"要什么"。                             ║
// ║                                                                            ║
// ║  ranges::to<T>() (C++23): 把懒求值的 view 管线结果收集到具体容器中。        ║
// ║   auto result = v | views::filter(...) | std::ranges::to<std::vector>();   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ranges 库的主要用法都在 <ranges> 头文件中，无需额外的自定义类型。
// 本文件主要作为模块的导入入口，实际使用中直接 include <ranges> 即可。
// 学习者应在 examples/ranges_demo.cpp 中跟着敲代码感受 ranges 的声明式风格。
