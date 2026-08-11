# ------------------------------------------------------------------------------
# 12_patterns — 设计模式
#
# 本模块讲解 C++ 中的经典设计模式及其现代实现方式，涵盖:
#   - Pimpl 惯用法 (编译防火墙)
#   - CRTP 静态多态 (编译期绑定, 零虚函数开销)
#   - 类型擦除 (外部多态, Concept/Model 模式)
#   - 单例模式 (Meyers 单例) 与工厂模式 (简单工厂 + 注册式工厂)
#   - 观察者模式 (信号/槽) 与访问者模式 (std::variant + std::visit)
#
# Library:  patterns_lib
#   Sources: src/pimpl.cpp
#            src/singleton_and_factory.cpp
#            src/type_erasure.cpp
#   Headers: include/patterns/pimpl.hpp
#            include/patterns/crtp.hpp
#            include/patterns/type_erasure.hpp
#            include/patterns/singleton_and_factory.hpp
#            include/patterns/observer_and_visitor.hpp
#   Links:   lesson_utils (PUBLIC — 所有模块共享的基础工具库)
#
# Examples:
#   01_pimpl_demo                  → links patterns_lib
#   02_crtp_demo                   → links patterns_lib
#   03_type_erasure_demo           → links patterns_lib
#   04_singleton_and_factory_demo  → links patterns_lib
#   05_observer_and_visitor_demo   → links patterns_lib
# ------------------------------------------------------------------------------

# ---- Library ----
# patterns_lib: 静态库, 包含 pimpl 移动语义、Meyers 单例初始化、注册工厂、
#               Concept/Model 类型擦除拷贝语义的具体实现
add_library(patterns_lib STATIC
    src/pimpl.cpp
    src/singleton_and_factory.cpp
    src/type_erasure.cpp
)

# PUBLIC: 所有链接此库的目标 (包括 example 可执行文件) 都能通过
#         #include "patterns/xxx.hpp" 访问本模块头文件
target_include_directories(patterns_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: lesson_utils 是跨模块共享的基础工具库
# 本模块头文件仅依赖 C++ 标准库 (<memory>, <string>, <vector>, <functional>,
# <variant>, <print>, <map>, <stdexcept> 等), 无其他模块依赖
target_link_libraries(patterns_lib PUBLIC
    lesson_utils
)

# ---- Examples ----
# 每个示例程序对应一个设计模式主题, 各自包含独立的 main() 函数

# 01: Pimpl 惯用法 — 编译防火墙, 隐藏实现细节, 加速编译
add_executable(01_pimpl_demo examples/01_pimpl_demo.cpp)
target_link_libraries(01_pimpl_demo PRIVATE patterns_lib)

# 02: CRTP 静态多态 — 编译期多态, Mixin 模式, 自动生成比较运算符
add_executable(02_crtp_demo examples/02_crtp_demo.cpp)
target_link_libraries(02_crtp_demo PRIVATE patterns_lib)

# 03: 类型擦除 — std::function 演示、手写 AnyDrawable (Concept/Model)、轻量级擦除
add_executable(03_type_erasure_demo examples/03_type_erasure_demo.cpp)
target_link_libraries(03_type_erasure_demo PRIVATE patterns_lib)

# 04: 单例模式与工厂模式 — Meyer's 单例 (线程安全) + 简单工厂 + 注册式工厂 (插件扩展)
add_executable(04_singleton_and_factory_demo examples/04_singleton_and_factory_demo.cpp)
target_link_libraries(04_singleton_and_factory_demo PRIVATE patterns_lib)

# 05: 观察者模式与访问者模式 — 泛型 Signal/Slot + std::variant AST 求值/打印/计数
add_executable(05_observer_and_visitor_demo examples/05_observer_and_visitor_demo.cpp)
target_link_libraries(05_observer_and_visitor_demo PRIVATE patterns_lib)
