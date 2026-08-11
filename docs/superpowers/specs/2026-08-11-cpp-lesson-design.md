# C++ 学习代码模板 — 设计文档

**日期**: 2026-08-11
**作者**: 艾克斯 & Claude
**状态**: 进行中

---

## 1. 目标

为有 C++ 理论基础但缺乏动手实践的学习者，提供一套覆盖传统 C++ 到 C++23 的完整代码模板。每个文件可独立编译运行，注释即教程，学习者照着敲一遍即可系统掌握 C++。

---

## 2. 学习者画像

- 大学学过 C++，有理论基础
- 工作后较少动手写 C++
- 希望系统性覆盖: C 风格基础 → OOP → STL → 现代 C++ → C++20/23
- 偏好：照着模板敲代码学习，通过动手加深记忆

---

## 3. 技术选型

| 项目 | 选择 | 理由 |
|------|------|------|
| 语言标准 | C++23 | 覆盖最广，最新标准 |
| 构建系统 | CMake 3.20+ | 主流工业标准 |
| 编译器 | GCC 14+ / Clang 18+ / MSVC 2022+ | C++23 支持 |
| 文件组织 | 按主题分文件，独立编译 | 灵活学习 |
| 工程结构 | 模仿真实项目目录分层 | 贴近实战 |

---

## 4. 项目结构

```
cpplesson/
├── CMakeLists.txt              # 顶层 CMake
├── README.md
├── shared/lesson_utils.h
│
├── 00_fundamentals/      (6)   语言基础
├── 01_classes/           (8)   类与 OOP
├── 02_memory/            (5)   内存管理
├── 03_templates/         (7)   模板
├── 04_stl/              (10)   标准模板库
├── 05_streams_and_io/    (4)   I/O 与格式化
├── 06_modern_syntax/     (6)   现代语法糖
├── 07_move_and_lambda/   (5)   移动语义与 Lambda
├── 08_error_and_safety/  (4)   错误处理
├── 09_concurrency/       (6)   并发编程
├── 10_cpp20_cpp23/       (8)   C++20/23 新特性
├── 11_patterns/          (5)   惯用法与设计模式
└── 12_build_and_tooling/ (3)   工程实践
```

**总计: 77 个 .cpp 文件 + 1 个 modules 子目录**

---

## 5. 文件模板规范

每个 `.cpp` 文件遵循统一结构:

```cpp
// MARK: - 文件头部注释(模块名、主题、前置知识、学习目标)

// MARK: - 头文件包含

// MARK: - 辅助结构/函数

// MARK: - Part 1: 核心概念 1
//   - 是什么(定义)
//   - 为什么(动机)
//   - 什么时候用(适用场景)
//   - 示例代码(带输出注释)

// MARK: - Part 2-N: 更多概念...

// MARK: - 常见陷阱与最佳实践

// MARK: - 练习(注释形式的挑战题)

// MARK: - main() 入口
```

---

## 6. 各模块详细内容

### 00_fundamentals — 语言基础 (6 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | types_and_literals | 基本类型、字面量、auto、类型别名、sizeof |
| 2 | const_and_static | const、constexpr、static、extern、inline |
| 3 | arrays_and_strings_c | C 数组、C 字符串、指针算术 |
| 4 | pointers_and_refs | 指针、引用、const 修饰、函数指针 |
| 5 | function_basics | 声明/定义、重载、默认参数、内联 |
| 6 | control_flow | if/switch、for/while、break/continue/goto |

### 01_classes — 类与 OOP (8 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | class_declaration | 类定义、访问控制、this、static 成员 |
| 2 | ctor_dtor | 构造/析构、初始化列表、委托构造、默认/删除 |
| 3 | copy_move_control | 拷贝/移动构造与赋值、三五零法则 |
| 4 | operator_overload | 算术/比较/下标/类型转换运算符 |
| 5 | inheritance | 继承、访问控制、using 声明、多重继承 |
| 6 | virtual_and_polymorphism | 虚函数、纯虚、抽象类、override/final |
| 7 | raii_and_resource | RAII 惯用法、scope_guard 思想 |
| 8 | friend_and_nested | 友元函数/类、嵌套类、局部类 |

### 02_memory — 内存管理 (5 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | stack_vs_heap | 栈 vs 堆、new/delete 底层、内存泄漏 |
| 2 | placement_new | placement new、对齐、operator new 重载 |
| 3 | unique_ptr | unique_ptr、make_unique、所有权转移 |
| 4 | shared_weak_ptr | shared_ptr、weak_ptr、循环引用、make_shared |
| 5 | custom_deleter_allocator | 自定义删除器、allocator 入门 |

### 03_templates — 模板 (7 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | function_template | 函数模板、类型推导、显式实例化 |
| 2 | class_template | 类模板、成员函数、非类型参数 |
| 3 | specialization | 全特化、偏特化、traits 入门 |
| 4 | variadic_template | 变参模板、参数包、递归展开 |
| 5 | fold_expressions | C++17 折叠表达式 |
| 6 | type_traits_basics | type_traits、SFINAE、enable_if |
| 7 | concepts | C++20 concepts、requires 子句 |

### 04_stl — 标准模板库 (10 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | sequential_containers | vector/deque/list/forward_list/array |
| 2 | associative_containers | set/map/multiset/multimap |
| 3 | unordered_containers | unordered_set/map、bucket、hash |
| 4 | container_adaptors | stack/queue/priority_queue |
| 5 | iterators | 迭代器类别、iterator_traits、自定义迭代器 |
| 6 | algorithms_nonmod | find/count/search/equal/mismatch 等 |
| 7 | algorithms_mod | copy/transform/remove/replace/generate 等 |
| 8 | algorithms_sort_merge | sort/merge/binary_search/heap/partition |
| 9 | tuple_pair_optional | pair/tuple/optional/variant/any |
| 10 | regex_and_random | regex、random 库、chrono |

### 05_streams_and_io — I/O 与格式化 (4 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | iostream_basics | cin/cout、流状态、操纵符、locale |
| 2 | file_streams | fstream、二进制读写、seek |
| 3 | string_streams | stringstream、ostringstream、格式化技巧 |
| 4 | format_and_print | std::format (C++20)、std::print (C++23) |

### 06_modern_syntax — 现代语法糖 (6 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | auto_and_decltype | auto、decltype、尾置返回类型 |
| 2 | range_for_and_init_if | range-for、if/switch with init (C++17) |
| 3 | enum_class | enum class、using enum (C++20) |
| 4 | list_init_and_designated | 统一初始化、指定初始化 (C++20) |
| 5 | structured_bindings | 结构化绑定 (C++17) |
| 6 | ctad | 类模板参数推导 (C++17) |

### 07_move_and_lambda — 移动语义与 Lambda (5 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | rvalue_and_move | 左值/右值、std::move、移动语义 |
| 2 | forward_and_perfect | std::forward、万能引用、完美转发 |
| 3 | lambda_basics | 捕获列表、mutable、返回类型推导 |
| 4 | lambda_advanced | 泛型 lambda、constexpr lambda、init capture |
| 5 | functional | std::function、std::bind、可调用对象 |

### 08_error_and_safety — 错误处理 (4 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | exceptions | throw/try/catch、异常层次、栈展开 |
| 2 | noexcept_and_contracts | noexcept、异常安全保证级别 |
| 3 | error_code | std::error_code、error_condition、system_error |
| 4 | expected | std::expected (C++23) |

### 09_concurrency — 并发编程 (6 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | thread_basics | std::thread、join/detach、线程 ID |
| 2 | mutex_and_lock | mutex、lock_guard、unique_lock、scoped_lock |
| 3 | condition_variable | condition_variable、notify、等待谓词 |
| 4 | future_and_promise | future/promise/packaged_task/async |
| 5 | atomic_and_memory | atomic、内存序、fence |
| 6 | jthread_and_latch | jthread、stop_token、latch/barrier (C++20) |

### 10_cpp20_cpp23 — C++20/23 新特性 (8 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | concepts_deep | 预定义 concept、requires 表达式、concept 组合 |
| 2 | ranges | ranges、views、管道、适配器 |
| 3 | coroutines | co_yield、generator、co_await 基础 |
| 4 | three_way_comparison | <=> 运算符、ordering 类型 |
| 5 | modules (子目录) | export module、import、module partition |
| 6 | bit_and_source_location | bit_cast、source_location (C++20) |
| 7 | deducing_this | deducing this (C++23)、简化 CRTP |
| 8 | flat_and_mdspan | flat_map/set、mdspan (C++23) |

### 11_patterns — 惯用法与设计模式 (5 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | pimpl | Pimpl 惯用法、编译隔离 |
| 2 | crtp | CRTP、静态多态、mixin |
| 3 | type_erasure | 类型擦除、std::function 原理 |
| 4 | singleton_and_factory | 单例（现代实现）、工厂模式 |
| 5 | observer_and_visitor | 观察者、std::variant + visit |

### 12_build_and_tooling — 工程实践 (3 文件)

| # | 文件 | 知识点 |
|---|------|--------|
| 1 | cmake_fundamentals | CMake 核心概念、target、属性 |
| 2 | preprocessor | 宏、条件编译、#pragma、PCH |
| 3 | debug_and_sanitizer | 断言、ASan、UBsan、valgrind 概念 |

---

## 7. 非目标

- 不覆盖 C 语言基础（假设已有大学基础）
- 不深入编译器内部实现
- 不包含 GUI 编程
- 不覆盖特定平台 API（Win32、POSIX 等）
- C++26 草案内容不在本次范围

---

## 8. 验收标准

- 77 个文件全部可独立编译运行
- 每个文件包含完整注释说明
- CMake 一键编译所有目标
- README 包含学习路线建议
