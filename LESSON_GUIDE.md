# C++ 学习指南 — 87 课时速览

## 学习流程

每课时三步：敲头文件 → 敲实现 → 敲示例 → 编译运行

```
模板                                      你敲到
02_classes/include/classes/xxx.hpp   →   mywork/include/classes/xxx.hpp
02_classes/src/xxx.cpp               →   mywork/src/classes/xxx.cpp
02_classes/examples/xxx_demo.cpp     →   mywork/examples/classes/xxx_demo.cpp
```

敲完在 `mywork/CMakeLists.txt` 中取消对应章节的注释，然后 `cmake -B build && cmake --build build`。

---

## 00 — 环境就绪 (1 课时)

| # | 课时 | 要点 |
|---|------|------|
| 0 | hello | 验证 C++23 编译器、CMake 基础、std::print/std::println |

---

## 01 — 语言基础 (6 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 类型与字面量 | bool/int/long/float/double、sizeof、auto、字面量后缀、定长类型(int32_t)、static_cast |
| 2 | const 与 static | const 变量/指针/引用、constexpr、constinit(C++20)、static 局部/全局、extern、inline 变量(C++17) |
| 3 | 数组与 C 字符串 | C 数组、std::array、C 字符串、指针算术、数组退化为指针 |
| 4 | 指针与引用 | 指针、引用、nullptr、const 指针/引用、函数指针、void* |
| 5 | 函数基础 | 声明/定义、重载、默认参数、inline、[[nodiscard]]、尾置返回类型 |
| 6 | 控制流 | if/else、switch(含[[fallthrough]])、for/while/do-while、break/continue、[[likely]]/[[unlikely]] |

---

## 02 — 类与面向对象 (8 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 类的声明 | class vs struct、public/private/protected、this 指针、static 成员、const 成员函数 |
| 2 | 构造与析构 | 构造/析构、初始化列表、委托构造、=default/=delete、explicit |
| 3 | 拷贝与移动控制 | 拷贝/移动构造与赋值、三五零法则、swap 惯用法、noexcept 移动 |
| 4 | 运算符重载 | 算术/比较/下标/调用/类型转换、成员 vs 非成员、<=>三路比较(C++20) |
| 5 | 继承 | 继承语法、访问控制、using 声明、多重继承、虚基类、final 类 |
| 6 | 虚函数与多态 | 虚函数、纯虚函数、抽象类、override/final、虚析构、dynamic_cast |
| 7 | RAII 与资源管理 | RAII 惯用法、资源获取即初始化、scope_guard 思想、文件 RAII 包装 |
| 8 | 友元与嵌套类 | 友元函数/类、嵌套类、局部类、前向声明 |
| 9 | union 与 volatile | union(活跃成员/匿名union/非平凡成员)、volatile(MMIO/信号处理/非多线程)、与variant/atomic对比 |

---

## 03 — 内存管理 (6 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 栈与堆 | 栈 vs 堆分配、new/delete、new[]/delete[]、内存泄漏、operator new/delete |
| 2 | placement new | placement new、alignas/alignof、std::aligned_storage、手动内存管理 |
| 3 | unique_ptr | std::unique_ptr、std::make_unique、所有权转移、自定义删除器 |
| 4 | shared_ptr 与 weak_ptr | std::shared_ptr、make_shared、引用计数、weak_ptr、循环引用、enable_shared_from_this |
| 5 | 分配器入门 | std::allocator、自定义分配器、pmr::polymorphic_allocator(C++17) |
| 6 | 内存池 | 固定大小 MemoryPool — freelist 实现、placement new 实战、与 allocator 的关系 |

---

## 04 — 模板 (7 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 函数模板 | 函数模板、类型推导、隐式/显式实例化、auto 模板参数(C++20) |
| 2 | 类模板 | 类模板、成员函数定义、非类型模板参数(NTTP)、auto NTTP(C++17) |
| 3 | 特化 | 全特化、偏特化、变量模板(C++14)、traits 类模板 |
| 4 | 变参模板 | 参数包、sizeof...、递归展开、逗号展开技巧 |
| 5 | 折叠表达式 | C++17 折叠表达式(一元/二元)、左折/右折 |
| 6 | type_traits 入门 | <type_traits>常用工具、SFINAE 概念、std::enable_if、void_t、if constexpr |
| 7 | Concepts | C++20 concepts、requires子句/表达式、预定义concept、自定义concept |

---

## 05 — 标准模板库 (10 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 顺序容器 | vector/deque/list/forward_list/array — 特性、复杂度、使用场景 |
| 2 | 关联容器 | set/map/multiset/multimap — 红黑树、有序性、lower_bound/upper_bound |
| 3 | 无序容器 | unordered_set/map — 哈希表、bucket、load_factor、自定义哈希 |
| 4 | 容器适配器 | stack/queue/priority_queue — 适配器模式、自定义比较器 |
| 5 | 迭代器 | 5 种迭代器类别、iterator_traits、自定义迭代器 |
| 6 | 非修改算法 | find/count/search/all_of/any_of/none_of/adjacent_find |
| 7 | 修改算法 | copy/transform/remove/replace/generate/fill/unique/reverse/rotate |
| 8 | 排序与二分 | sort/stable_sort/partial_sort/binary_search/lower_bound/heap/partition |
| 9 | pair/tuple/optional/variant | pair、tuple(tie/结构化绑定)、optional(C++17)、variant/visit、any |
| 10 | chrono 与随机数 | duration/time_point/clock、<random>、engine/distribution |
| 11 | charconv 与 optional 单子 | to_chars/from_chars 高性能转换、optional 单子操作(and_then/or_else/transform, C++23) |

---

## 06 — I/O 与格式化 (4 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | iostream 基础 | cin/cout、流状态(fail/eof)、getline、操纵符(boolalpha/hex/setw) |
| 2 | 文件流 | ifstream/ofstream、打开模式、二进制读写、seekg/seekp |
| 3 | 字符串流 | istringstream/ostringstream、格式化到字符串、从字符串解析 |
| 4 | format 与 print | std::format(C++20)、std::print/println(C++23)、格式说明符、自定义 formatter |

---

## 07 — 现代语法糖 (6 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | auto 与 decltype | auto 推导规则、decltype、decltype(auto)、尾置返回类型 |
| 2 | range-for 与 if-init | range-for(C++11)、if/switch 带初始化器(C++17)、[[likely]]/[[unlikely]](C++20) |
| 3 | enum class | 限定作用域枚举、底层类型、using enum(C++20) |
| 4 | 统一初始化与指定初始化 | {}统一初始化、initializer_list、窄化阻止、指定初始化(C++20) |
| 5 | 结构化绑定 | 结构化绑定(C++17)—绑定数组/tuple/成员、引用绑定、与 range-for 配合 |
| 6 | CTAD | 类模板参数推导(C++17)、推导指引、聚合 CTAD(C++20)、常见陷阱 |
| 7 | 用户定义字面量 | operator""、_km/_deg 自定义后缀、std::to_underlying(C++23)、标准库字面量 |
| 8 | 属性与条件 explicit | [[no_unique_address]](C++20)空成员优化、explicit(bool)(C++20)、hardware_destructive_interference_size(C++17) |

---

## 08 — 移动语义与 Lambda (6 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 右值与移动语义 | 左值/右值/将亡值、std::move、移动构造/赋值、noexcept 移动、RVO/NRVO |
| 2 | 完美转发 | std::forward、万能引用(T&&/auto&&)、引用折叠、emplace 系列 |
| 3 | Lambda 基础 | 捕获列表(=、&、this)、mutable、返回类型推导、IILE |
| 4 | Lambda 进阶 | 泛型 lambda(auto)、constexpr lambda、捕获初始化(C++14)、模板 lambda(C++20) |
| 5 | functional | std::function、std::bind、std::mem_fn、std::invoke(C++17)、reference_wrapper |
| 6 | 可调用对象进阶 | std::function_ref(C++23 非拥有)、std::move_only_function(C++23)、std::invocable/predicate concept |

---

## 09 — 错误处理与安全 (5 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 异常 | throw/try/catch、标准异常层次、栈展开、catch(...)、异常安全 |
| 2 | noexcept 与契约 | noexcept 说明符/运算符、异常安全保证(基本/强/不抛出)、[[nodiscard]] |
| 3 | error_code | std::error_code、error_condition、system_error、自定义 error_category |
| 4 | expected | std::expected<T,E>(C++23)、and_then/or_else/transform、vs 异常 vs optional |
| 5 | stacktrace | std::stacktrace(C++23)—捕获调用栈、stacktrace_entry、与异常/日志集成 |

---

## 10 — 并发编程 (7 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | 线程基础 | std::thread、join/detach、线程 ID、hardware_concurrency |
| 2 | 互斥量与锁 | mutex、lock_guard、unique_lock、scoped_lock(C++17)、shared_mutex、死锁避免 |
| 3 | 条件变量 | condition_variable、notify_one/all、带谓词 wait、虚假唤醒、生产者-消费者 |
| 4 | future 与 promise | future/promise、async、packaged_task、shared_future、wait_for、异常传播 |
| 5 | 原子操作与内存序 | atomic、CAS、内存序(relaxed/acquire/release/seq_cst)、atomic_ref(C++20)、fence |
| 6 | jthread 与并行原语 | jthread(C++20)、stop_token、latch/barrier(C++20)、counting_semaphore |
| 7 | 线程池 | 完整 ThreadPool — 任务队列、工作线程(jthread)、stop_token 优雅关闭、counting_semaphore 限流 |

---

## 11 — C++20/23 新特性 (9 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | Concepts 进阶 | requires 表达式(简单/类型/复合/嵌套)、concept 组合、约束的偏序 |
| 2 | Ranges | range 概念、views(transform/filter/take/drop)、管道操作符\|、ranges::to(C++23) |
| 3 | 协程 | co_await/co_yield/co_return、promise_type、generator<T>、awaiter 概念 |
| 4 | 三路比较 | <=>运算符、strong/weak/partial_ordering、=default 生成、自定义<=> |
| 5 | span 与 source_location | std::span(C++20)—数组视图、static/dynamic extent、source_location(C++20) |
| 6 | bit_cast 与位操作 | std::bit_cast(C++20)、byteswap(C++23)、bit_ceil/floor、popcount(C++20) |
| 7 | deducing this | deducing this(C++23)—显式对象参数、简化 CRTP、递归 lambda、值类别感知 |
| 8 | flat_map 与 mdspan | std::flat_map/flat_set(C++23)、std::mdspan(C++23)—多维视图 |
| 9 | constexpr 能力增强 | constexpr new/delete(C++20)、std::is_constant_evaluated、constexpr 析构、std::unreachable(C++23) |

---

## 12 — 惯用法与设计模式 (5 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | Pimpl | 指针实现 — 编译隔离、ABI 稳定、unique_ptr + 前向声明的特殊处理 |
| 2 | CRTP | 奇异递归模板模式 — 静态多态、mixin 模式、与 deducing this 对比 |
| 3 | 类型擦除 | std::function 原理、手写 AnyDrawable、值语义 |
| 4 | 单例与工厂 | Meyers 单例(static 局部变量)、工厂模式(注册式/模板式)、依赖注入概念 |
| 5 | 观察者与访问者 | 观察者(signal/slot)、std::variant+visit 访问者、overloaded 模式 |

---

## 13 — 工程实践 (3 课时)

| # | 课时 | 要点 |
|---|------|------|
| 1 | CMake 进阶 | target 属性(PUBLIC/PRIVATE/INTERFACE)、install/export、find_package、CTest |
| 2 | 预处理器 | #define/#undef、条件编译、#pragma once vs include guard、__has_include(C++17) |
| 3 | 调试与 Sanitizer | assert/static_assert、AddressSanitizer(-fsanitize=address)、UBsan、NDEBUG |

---

## 进度建议

- **每天 2-3 课时**，约 7-9 周完成
- **00-01** 快速过（2-3 天）
- **02-03** 类+内存池（1.5 周）
- **04-05** 模板和 STL（1.5 周）
- **06-07** 语法糖+属性（1 周）
- **08** 移动语义+可调用对象（1 周）
- **09-10** 错误处理+线程池+并发（1.5 周）
- **11** C++20/23 精华+constexpr增强（1 周）
- **12-13** 模式+工程（1 周）
