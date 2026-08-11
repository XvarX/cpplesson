#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: std::flat_map/flat_set (C++23) 和 std::mdspan (C++23)                 ║
// ║  目标: 理解连续内存有序容器的性能优势，掌握多维视图的基本用法                 ║
// ║                                                                            ║
// ║  flat_map / flat_set (C++23): 连续内存的有序关联容器                         ║
// ║                                                                            ║
// ║  WHY: std::map/set 是红黑树，节点分散在堆上 → 缓存不友好                    ║
// ║       flat_map/set 用排序的 vector 存储 → 连续内存 → 缓存友好               ║
// ║                                                                            ║
// ║  对比:                                                                      ║
// ║  ┌──────────────┬───────────────┬───────────────┐                           ║
// ║  │  操作        │  std::map     │ flat_map      │                           ║
// ║  ├──────────────┼───────────────┼───────────────┤                           ║
// ║  │  查找        │  O(log n)     │  O(log n) ✓   │                           ║
// ║  │  遍历        │  指针跳转     │  连续扫描 ✓   │                           ║
// ║  │  插入        │  O(log n) ✓   │  O(n) 慢!     │                           ║
// ║  │  内存占用    │  节点开销大   │  只存数据 ✓   │                           ║
// ║  └──────────────┴───────────────┴───────────────┘                           ║
// ║                                                                            ║
// ║  WHEN: 构建一次、查询多次的场景 (如配置表、字典、查找表)                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <vector>
#include <utility>
#include <algorithm>
#include <ranges>
#include <mdspan>     // C++23 mdspan
#include <print>

// ============================================================================
// Part 1: flat_map — 排序 vector 实现的有序映射
// ============================================================================
// 核心思想: 用两个并列的 vector (keys + values) 或一个 vector<pair> 存储
//           保持按键排序，二分查找 O(log n)，遍历 O(1) 缓存命中

template<typename K, typename V>
struct SimpleFlatMap {
    std::vector<std::pair<K, V>> data;

    // 插入: O(n) — 需要移动元素保持排序
    void insert(K key, V value) {
        auto it = std::ranges::lower_bound(data, key,
            std::less<>{}, [](auto& p) { return p.first; });
        if (it != data.end() && it->first == key) {
            it->second = std::move(value);  // 键已存在，更新值
        } else {
            data.insert(it, {std::move(key), std::move(value)});
        }
    }

    // 查找: O(log n) — 二分查找
    V* find(const K& key) {
        auto it = std::ranges::lower_bound(data, key,
            std::less<>{}, [](auto& p) { return p.first; });
        if (it != data.end() && it->first == key)
            return &it->second;
        return nullptr;
    }

    auto begin() { return data.begin(); }
    auto end()   { return data.end(); }

    size_t size() const { return data.size(); }
};

// ============================================================================
// Part 2: std::mdspan — 多维视图 (C++23)
// ============================================================================
// mdspan 是一维连续内存上的多维"镜头"，不拥有数据。
// 核心模板参数: mdspan<T, Extents, LayoutPolicy, AccessorPolicy>
//
// Layout 策略:
//   layout_right  — 行优先 (C/C++ 默认): 最后一维连续
//   layout_left   — 列优先 (Fortran/MATLAB): 第一维连续
//   layout_stride — 自定义步长 (支持子视图)
//
// 关键操作:
//   mdspan(x, y) — 访问元素 (operator[])
//   submdspan()  — 创建子视图 (切片)

// ============================================================================
// Part 3: submdspan — 多维切片 (C++23)
// ============================================================================
// submdspan 从 mdspan 创建子视图，支持:
//   单个索引    → 在该维固定，维度减 1
//   区间(range) → 在该维取子区间
//   full_extent → 保留整个维度

// 打印矩阵的辅助函数
template<typename T, size_t Rows, size_t Cols>
void print_matrix(std::mdspan<T, std::extents<size_t, Rows, Cols>, std::layout_right> mat) {
    for (size_t i = 0; i < mat.extent(0); ++i) {
        std::print("  ");
        for (size_t j = 0; j < mat.extent(1); ++j) {
            std::print("{:>5} ", mat[i, j]);
        }
        std::println("");
    }
}
