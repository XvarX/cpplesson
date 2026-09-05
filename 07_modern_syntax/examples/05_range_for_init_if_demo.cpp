// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: range-for / 带初始化器的 if&switch / [[likely]] [[unlikely]]        ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握 range-for 循环的语法与应用                                       ║
// ║    2. 理解 C++17 带初始化器的 if/switch，缩小变量作用域                     ║
// ║    3. 学会使用 [[likely]] / [[unlikely]] 向编译器提供分支预测提示          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/range_for_init_if.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <vector>
#include <map>
#include <string>
#include <memory>   // std::shared_ptr
#include <cstdlib>  // std::rand

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: range-for 循环 (C++11)                                             ║
// ║  WHAT: for (decl : range) —— 用声明变量遍历可迭代范围                      ║
// ║  WHY:  消除写迭代器的样板代码; 避免越界错误; 代码更简洁清晰               ║
// ║  WHEN: 遍历容器/数组/initializer_list/C++20 range; 不需要索引进度时       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_range_for() {
    lesson::print_header("Part 1: range-for 循环");

    // ── 1.1 基本语法: 遍历数组 ──
    {
        lesson::print_subtitle("1.1  遍历数组");
        int arr[] = {10, 20, 30, 40, 50};
        // 原型: for (范围声明 : 范围表达式) 语句
        for (int v : arr) {                     // 按值拷贝每个元素
            std::print("{} ", v);
        }
        std::println("");
    }

    // ── 1.2 三种遍历方式: 拷贝 / 引用 / const引用 ──
    {
        lesson::print_subtitle("1.2  三种遍历方式对比");
        std::vector<int> nums = {1, 2, 3};

        // 方式A: 按值拷贝 —— 修改不影响原容器，但有拷贝开销
        for (auto v : nums) { v *= 2; }        // nums 不变
        std::println("按值拷贝后: nums[0] = {}", nums[0]); // 输出: 1

        // 方式B: 按引用 —— 零拷贝，可以修改元素 (最常用)
        for (auto& v : nums) { v *= 2; }       // nums 被修改
        std::println("按引用修改后: nums[0] = {}", nums[0]);  // 输出: 2

        // 方式C: 按 const 引用 —— 零拷贝，但不能修改 (只读遍历)
        for (const auto& v : nums) {
            std::print("{} ", v);
            // v *= 2;  // 编译错误! v 是 const
        }
        std::println("");
    }

    // ── 1.3 遍历 map ──
    {
        lesson::print_subtitle("1.3  遍历 std::map (结构化绑定组合技)");
        std::map<std::string, int> scores = {{"艾克斯", 95}, {"小明", 87}, {"小红", 92}};
        for (const auto& [name, score] : scores) {  // C++17 结构化绑定 + range-for
            std::println("  {}: {} 分", name, score);
        }
    }

    // ── 1.4 C++20: range-for 带初始化器 ──
    {
        lesson::print_subtitle("1.4  C++20 range-for 初始化器");
        // 在 range-for 前执行一段初始化，变量生命周期与循环绑定
        std::vector<int> data = {5, 3, 8, 1, 9};
        for (auto sz = data.size(); auto v : data) {  // sz 只在循环范围内可见
            std::print("{}/{}  ", v, sz);
        }
        std::println("");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 带初始化器的 if / switch (C++17)                                   ║
// ║  WHAT: if (init; condition) / switch (init; expr)                           ║
// ║  WHY:  将变量的作用域限制在 if/else 分支内; 防止变量逃逸后被误用           ║
// ║  WHEN: 锁守卫 (std::lock_guard); 查找-判断模式; 资源作用域控制             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_init_if_switch() {
    lesson::print_header("Part 2: 带初始化器的 if/switch (C++17)");

    // ── 2.1 init-if 基础──
    {
        lesson::print_subtitle("2.1  init-if 基础模式");
        // 旧式写法: 变量泄漏到外层作用域
        // auto it = map.find(key);
        // if (it != map.end()) { ... }
        // // it 仍然可见，可能被误用！

        std::map<std::string, int> book = {{"C++", 2023}, {"Python", 1991}};

        // 新式写法 (C++17): 变量 it 仅在 if/else 块内可见
        if (auto it = book.find("C++"); it != book.end()) {
            std::println("找到 C++: 诞生于 {}", it->second);
        } else {
            std::println("未找到 C++");
        }
        // it 在此处不可见 —— 作用域安全!
    }

    // ── 2.2 init-if 与动态类型检查 ──
    {
        lesson::print_subtitle("2.2  init-if 配合 dynamic_cast");
        using namespace modern_syntax;

        // 实际应用中这里可能是工厂函数返回的指针
        std::unique_ptr<Animal> pet = std::make_unique<Dog>();

        // init-if 把转换和判断写在一行
        if (auto* dog = dynamic_cast<Dog*>(pet.get()); dog != nullptr) {
            dog->bark();
        } else if (auto* cat = dynamic_cast<Cat*>(pet.get()); cat != nullptr) {
            cat->meow();
        }
    }

    // ── 2.3 init-if 锁守卫 (最经典的用法) ──
    {
        lesson::print_subtitle("2.3  经典的锁守卫模式");
        // 伪代码示意，不实际加锁:
        // if (std::lock_guard lock(mutex); shared_data.ready) {
        //     process(shared_data);
        // }  // lock 在此析构，自动释放 —— RAII + init-if 的完美结合
        lesson::print_note("init-if + lock_guard = 自动作用域锁 (最推荐用法)");
    }

    // ── 2.4 init-switch (C++17) ──
    {
        lesson::print_subtitle("2.4  带初始化器的 switch");
        using namespace modern_syntax;

        // 在 switch 前计算一次，避免在 case 中重复
        auto compute = [](Op op, int a, int b) {
            switch (auto result = 0; op) { // init-switch! result 仅 switch 内可见
            case Op::Add: result = a + b; break;
            case Op::Sub: result = a - b; break;
            case Op::Mul: result = a * b; break;
            case Op::Div: result = (b != 0) ? a / b : 0; break;
            }
            // result 在此不可见 —— 可惜 C++ 中 switch-case 共享作用域
            // 实际限制: result 在 switch 块内可见但写在初始化里主要用于表达意图
        };
        lesson::print_note("init-switch 主要用于语义清晰和一次性计算");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: [[likely]] / [[unlikely]] (C++20)                                 ║
// ║  WHAT: 属性标签，告诉编译器哪个分支更可能被执行                              ║
// ║  WHY:  帮助编译器优化分支预测和指令布局(cache locality); 提升热路径性能     ║
// ║  WHEN: 性能敏感的 if/switch; 错误处理; 明确知道某分支高频时                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_likely_unlikely() {
    lesson::print_header("Part 3: [[likely]] / [[unlikely]] (C++20)");

    // ── 3.1 基本语法 ──
    {
        lesson::print_subtitle("3.1  if 语句中使用");
        int x = 42;
        if (x > 0) [[likely]] {
            // 正常路径: 编译器会把这段代码放在连续的缓存行里
            std::println("正常: x > 0  (大概率走这里)");
        } else [[unlikely]] {
            // 异常路径: 编译器可能把这段代码移出热路径
            std::println("异常: x <= 0  (几乎不会走这里)");
        }
    }

    // ── 3.2 switch 中使用 ──
    {
        lesson::print_subtitle("3.2  switch 语句中使用");
        int error_code = 0; // 模拟正常情况
        switch (error_code) {
        case 0:
            [[likely]] std::println("一切正常");
            break;
        case 1:
            [[unlikely]] std::println("轻微错误");
            break;
        default:
            [[unlikely]] std::println("严重错误!");
            break;
        }
    }

    // ── 3.3 典型场景: 错误处理 ──
    {
        lesson::print_subtitle("3.3  典型场景: 提前返回的错误检查");
        auto process = [](int id) -> bool {
            // 无效 ID 极少出现 -> [[unlikely]]
            if (id <= 0) [[unlikely]] {
                return false;
            }
            // 正常处理路径
            std::println("处理 id={} 的请求...", id);
            return true;
        };
        process(100);
    }

    lesson::print_note("[[likely]]/[[unlikely]] 不影响程序正确性，仅影响性能");
    lesson::print_note("滥用反而会降低性能 —— 只在 profile 确认的热路径上使用");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: range-for 中修改容器自身
    {
        lesson::print_subtitle("陷阱1: range-for 循环中增删容器元素");
        std::vector<int> v = {1, 2, 3, 4, 5};
        // ❌ 危险: 在 range-for 中向 v 添加元素 → 迭代器失效!
        // for (auto& x : v) { if (x == 3) v.push_back(6); }
        lesson::print_warn("range-for 中不要增删元素 —— 会导致迭代器失效");

        // ✅ 正确: 使用索引循环或先标记，后处理
        std::vector<int> to_add;
        for (auto& x : v) {
            if (x == 3) to_add.push_back(6);
        }
        v.insert(v.end(), to_add.begin(), to_add.end());
        std::print("安全添加后: ");
        for (auto x : v) std::print("{} ", x);
        std::println("");
    }

    // 陷阱2: 临时对象的 range-for
    {
        lesson::print_subtitle("陷阱2: 临时对象的生命周期");
        auto make_vec = [] { return std::vector<int>{1, 2, 3}; };
        // ✅ C++20 起 range-for 会延长临时对象生命周期
        for (auto x : make_vec()) {                    // 安全: 临时对象生命周期延长
            std::print("{} ", x);
        }
        std::println("");
        lesson::print_note("C++20 range-for 延长了临时范围的寿命(Senior 级安全)");
    }

    // 陷阱3: [[likely]]/[[unlikely]] 不是魔法
    {
        lesson::print_subtitle("陷阱3: 属性标签不能挽救糟糕的算法");
        lesson::print_warn("[[likely]] 不改变逻辑 —— O(n^2) 不会因为标签变成 O(n)");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 用 range-for + 结构化绑定遍历 std::map，打印所有键值对
    lesson::print_subtitle("练习1: range-for 遍历 map");

    // 练习2: 写一个文件读取函数，用 init-if 在 if 语句中打开文件并检查是否成功
    lesson::print_subtitle("练习2: init-if 模式");
    // 提示: if (auto file = fopen(...); file != nullptr) { ... } else { ... }

    // 练习3: 在排序算法的比较分支中使用 [[likely]] 标注常见情况
    lesson::print_subtitle("练习3: [[likely]]/[[unlikely]] 实际场景");
}

int main() {
    part1_range_for();
    part2_init_if_switch();
    part3_likely_unlikely();

    pitfalls();
    exercises();
    return 0;
}
