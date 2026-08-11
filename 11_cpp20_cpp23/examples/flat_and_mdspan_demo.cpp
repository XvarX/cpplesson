// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: std::flat_map/flat_set (C++23) 和 std::mdspan (C++23)                 ║
// ║  目标: 理解连续内存有序容器的性能优势，掌握多维视图的基本用法                 ║
// ║                                                                            ║
// ║  学习方式: SimpleFlatMap 和 print_matrix 定义在 flat_md.hpp 中。             ║
// ║  下面的代码演示 flat_map 的性能优势场景和 mdspan 的多维切片操作。             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/flat_md.hpp"
#include <print>
#include <vector>
#include <string>
#include <mdspan>     // C++23 mdspan

int main() {
    lesson::print_header("flat_map / flat_set & mdspan (C++23)");

    // ── Part 1: flat_map ──
    lesson::print_subtitle("Part 1: flat_map — 排序 vector 实现的有序映射");

    SimpleFlatMap<std::string, int> scores;
    scores.insert("Alice", 95);
    scores.insert("Bob", 87);
    scores.insert("Charlie", 92);
    scores.insert("Diana", 88);
    // 注意: Alice 已经存在，更新值
    scores.insert("Alice", 97);

    std::println("  flat_map 内容 (按键排序):");
    for (auto& [name, score] : scores) {
        std::println("    {}: {}", name, score);
    }

    lesson::print_separator();
    if (auto* s = scores.find("Charlie")) {
        std::println("  Charlie 的分数: {}", *s);
    }
    if (scores.find("Eve") == nullptr) {
        std::println("  Eve 不在表中 (find 返回 nullptr)");
    }

    // 性能说明
    lesson::print_separator();
    lesson::print_note("flat_map vs std::map:");
    std::println("  ┌──────────────────────────────┐");
    std::println("  │ flat_map 适用: 构建少,查询多       │");
    std::println("  │ 典型场景: 配置文件表, 字典, 查找表 │");
    std::println("  │ std::map 适用: 频繁插入/删除        │");
    std::println("  │ 典型场景: 在线数据结构, 实时更新     │");
    std::println("  └──────────────────────────────┘");

    // ── Part 2: mdspan — 多维视图 ──
    lesson::print_subtitle("Part 2: std::mdspan — 一维内存上的多维视图");

    // 原始数据: 一维数组表示 3x4 矩阵
    std::vector<int> raw_data = {
        1,  2,  3,  4,
        5,  6,  7,  8,
        9, 10, 11, 12
    };

    // 创建行优先 (row-major) 的 mdspan
    std::mdspan mat{raw_data.data(), 3uz, 4uz};
    // 等价于: std::mdspan<int, std::dextents<size_t, 2>> mat(raw_data.data(), 3, 4);

    std::println("  原始 3x4 矩阵 (行优先):");
    std::println("  索引方式: mat[行, 列]");
    std::println("  mat[0,0]={}, mat[2,3]={}, mat[1,2]={}",
        mat[0, 0], mat[2, 3], mat[1, 2]);

    std::println("");
    std::println("  遍历矩阵:");
    for (size_t i = 0; i < mat.extent(0); ++i) {
        std::print("    Row {}: ", i);
        for (size_t j = 0; j < mat.extent(1); ++j) {
            std::print("{:>3} ", mat[i, j]);
        }
        std::println("");
    }

    // 维度信息
    lesson::print_separator();
    std::println("  维度信息:");
    std::println("    rank (维数):      {}", mat.rank());
    std::println("    extent(0) 行数:   {}", mat.extent(0));
    std::println("    extent(1) 列数:   {}", mat.extent(1));
    std::println("    size (总元素数):   {}", mat.size());

    // ── 布局策略演示 ──
    lesson::print_separator();
    std::println("  布局策略:");
    std::println("    layout_right: 行优先 — mat[i][j] = data[i*cols + j] (C/C++ 默认)");
    std::println("    layout_left:  列优先 — mat[i][j] = data[i + j*rows] (Fortran/MATLAB)");
    std::println("    layout_stride: 自定义步长 — 支持子视图和切片");

    // ── Part 3: submdspan — 多维切片 ──
    lesson::print_subtitle("Part 3: submdspan — 多维切片");

    std::println("  子视图示例 (3x4 矩阵的子区域):");

    // 取行 0-1, 列 1-2 (2x2 子矩阵)
    auto sub = std::submdspan(mat,
        std::pair<size_t, size_t>{0, 2},   // 行: [0, 2)
        std::pair<size_t, size_t>{1, 3}    // 列: [1, 3)
    );

    std::println("  子矩阵 (行[0,2), 列[1,3)):");
    std::println("    sub.extent(0) = {}, sub.extent(1) = {}", sub.extent(0), sub.extent(1));
    std::println("    sub[0,0]={}, sub[0,1]={}, sub[1,0]={}, sub[1,1]={}",
        sub[0, 0], sub[0, 1], sub[1, 0], sub[1, 1]);

    // 验证子视图修改会影响原数据 (mdspan 只是视图)
    lesson::print_separator();
    std::println("  修改子视图 sub[0,0] = 999:");
    sub[0, 0] = 999;
    std::println("    mat[0,1] = {} (原数据也被修改了!)", mat[0, 1]);

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. flat_map: 插入是 O(n) — 不适合频繁插入的场景");
    std::println("  2. flat_map: 迭代器在插入后可能失效 (vector reallocation)");
    std::println("  3. mdspan: 不拥有数据 — 原数据销毁后 mdspan 悬空");
    std::println("  4. mdspan: layout_right 和 layout_left 的数学约定不同，混淆会出错");
    std::println("  5. submdspan 返回的视图和原视图共享数据 — 修改子视图会影响原视图");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 用 flat_map 实现一个简单的电话号码簿 (姓名 → 号码)");
    std::println("     对比 std::map 和 flat_map 的遍历性能");
    std::println("  2. 用 mdspan 表示一个 RGB 图像 (3 维: 高 × 宽 × 通道)");
    std::println("     实现一个水平翻转和提取红色通道的函数");
    std::println("  3. 用 submdspan 实现一个简单的滑动窗口，遍历矩阵的所有 2x2 子块");
}
