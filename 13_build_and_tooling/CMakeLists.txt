# ------------------------------------------------------------------------------
# 13_build_and_tooling — 工程实践
#
# 本模块用 C++ 类模拟 CMake/CTest/预处理器/编译器消毒器的核心概念，帮助学习:
#   - CMake target 属性传递 (PUBLIC / PRIVATE / INTERFACE)
#   - CTest 测试注册与运行
#   - 宏定义、条件编译、平台检测
#   - assert / static_assert / AddressSanitizer / UBSan
#
# Library:  build_tooling_lib
#   Sources: src/cmake_sim.cpp
#            src/ctest_sim.cpp
#   Headers: include/build_tooling/cmake_sim.hpp
#            include/build_tooling/ctest_sim.hpp
#            include/build_tooling/debug_log.hpp         (header-only macros)
#            include/build_tooling/macro_utils.hpp       (header-only macros)
#            include/build_tooling/platform_detect.hpp   (header-only macros / constexpr)
#   Links:   lesson_utils (PUBLIC — 所有模块的共享工具)
#
#   ⚠ debug_log.hpp、macro_utils.hpp、platform_detect.hpp 是纯头文件，
#     没有对应的 .cpp，无需加入 add_library 的源文件列表。
#
# Examples:
#   01_cmake_deep_dive_demo  — 模拟 CMake target 属性传递 + CTest 运行
#   02_debug_sanitizer_demo  — assert / ASan / UBSan / NDEBUG
#   03_preprocessor_demo     — 宏、条件编译、__has_include、平台检测
# ------------------------------------------------------------------------------

# ---- Library ----
#
# build_tooling_lib 包含两个 .cpp 文件的编译产物。
# 头文件 debug_log.hpp、macro_utils.hpp、platform_detect.hpp 是纯宏/constexpr，
# 由示例程序直接 include，不参与库编译。
#
add_library(build_tooling_lib STATIC
    src/cmake_sim.cpp
    src/ctest_sim.cpp
)

# PUBLIC: 所有链接本库的 target 都能通过 #include "build_tooling/xxx.hpp" 找到头文件
target_include_directories(build_tooling_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: lesson_utils 是所有模块的公共基础设施;
#         本模块头文件不依赖其他模块的类型, 无需额外 PUBLIC 依赖
target_link_libraries(build_tooling_lib PUBLIC
    lesson_utils
)

# ---- Examples ----
#
# 每个示例程序独立编译，PRIVATE 链接 build_tooling_lib 即可获得:
#   - build_tooling 的头文件搜索路径 (通过 PUBLIC include)
#   - lesson_utils 的头文件搜索路径  (通过 PUBLIC 传递)

# 01: CMake target 属性传递模拟 + CTest 运行器演示
add_executable(01_cmake_deep_dive_demo
    examples/cmake_deep_dive_demo.cpp
)
target_link_libraries(01_cmake_deep_dive_demo PRIVATE
    build_tooling_lib
)

# 02: 调试与消毒器 — assert / ASan / UBSan / NDEBUG 演示
add_executable(02_debug_sanitizer_demo
    examples/debug_sanitizer_demo.cpp
)
target_link_libraries(02_debug_sanitizer_demo PRIVATE
    build_tooling_lib
)

# 03: 预处理器 — 宏、条件编译、__has_include、平台检测
add_executable(03_preprocessor_demo
    examples/preprocessor_demo.cpp
)
target_link_libraries(03_preprocessor_demo PRIVATE
    build_tooling_lib
)
