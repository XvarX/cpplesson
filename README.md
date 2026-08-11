# C++ 学习代码模板 — 真实工程布局

从传统 C++ 到 C++23，14 个模块，87 个演示程序。真实工程的 `include/` + `src/` + `examples/` 三层结构。

## 学习方式：三层全敲

以 `02_classes` 为例，一个课时你要敲 3 个文件：

```
模板 (看不改)                          你敲的 (mywork/)
─────────────────────────────────      ─────────────────────────────
02_classes/
├── include/classes/
│   └── class_basics.hpp          →    mywork/include/classes/class_basics.hpp
│       ^^^ 类声明、成员函数、注释          ^^^ 照着敲：理解"声明了什么"
│
├── src/
│   └── class_basics.cpp          →    mywork/src/class_basics.cpp
│       ^^^ 非内联函数实现                 ^^^ 照着敲：理解"怎么实现的"
│
└── examples/
    └── 01_class_declaration_demo.cpp → mywork/examples/01_class_declaration_demo.cpp
        ^^^ 引入头文件，调用类，main()       ^^^ 照着敲：理解"怎么用"
```

敲完后，你 `mywork/` 下的代码就是你自己写的完整小工程。

## 快速开始

```bash
cd cpplesson

# 配置 (只需一次)
cmake -B build

# 编译你 mywork/ 下所有的代码
cmake --build build

# 运行你敲的示例
./build/bin/01_class_declaration_demo

# 只编译某一个
cmake --build build --target 01_class_declaration_demo
```

## 环境要求

| 工具 | 版本 |
|------|------|
| 编译器 | GCC 14+ / Clang 18+ / MSVC 2022 17.10+ |
| CMake | 3.20+ |

## 项目结构

```
cpplesson/
├── CMakeLists.txt
├── shared/lesson_utils.hpp       # 共享工具
│
├── 00_quickstart/     ( 1 课)    # 环境验证、CMake 基础
├── 01_fundamentals/   ( 6 课)    # 类型、const、数组、指针、函数、控制流
├── 02_classes/        ( 9 课)    # class/struct、构造/析构、继承、虚函数、RAII、union/volatile
├── 03_memory/         ( 6 课)    # 栈/堆、智能指针(unique/shared/weak)、分配器、内存池
├── 04_templates/      ( 7 课)    # 函数/类模板、特化、变参、折叠表达式、concepts
├── 05_stl/            (11 课)    # 全部容器、迭代器、算法、tuple/optional/variant、charconv
├── 06_streams_and_io/ ( 4 课)    # iostream、文件流、字符串流、format/print
├── 07_modern_syntax/  ( 8 课)    # auto、range-for、enum class、结构化绑定、CTAD、字面量、属性
├── 08_move_and_lambda/( 6 课)    # 移动语义、完美转发、lambda、function/bind、可调用对象进阶
├── 09_error_and_safety/( 5 课)   # 异常、noexcept、error_code、expected、stacktrace
├── 10_concurrency/    ( 7 课)    # thread、mutex、condition_variable、future、atomic、jthread、线程池
├── 11_cpp20_cpp23/    ( 9 课)    # concepts 进阶、ranges、协程、<=>、deducing this、constexpr增强
├── 12_patterns/       ( 5 课)    # Pimpl、CRTP、类型擦除、单例/工厂、观察者/访问者
├── 13_build_and_tooling/( 3 课)  # CMake 进阶、预处理器、调试/sanitizer
│
└── mywork/                       # 👈 你的代码 (照着模板敲到这里)
    ├── include/                  #     头文件
    ├── src/                      #     实现
    └── examples/                 #     演示程序
```

每个模块内部结构以 `02_classes` 为例：
```
02_classes/
├── CMakeLists.txt                # 定义 classes_lib 库 + 示例可执行文件
├── include/classes/              # 头文件 (.hpp) — 类声明、接口
├── src/                          # 源文件 (.cpp) — 实现
└── examples/                     # 示例 (.cpp) — 每个有 main()，引入库来演示
```

## 学习路线

按编号 00 → 01 → 02 → ... → 13 顺序学。后面依赖前面。

## License

MIT
