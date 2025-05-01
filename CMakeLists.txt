cmake_minimum_required(VERSION 3.14)
project(PascalToC CXX)

# 设置使用Clang编译器
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加Clang特定编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 启用导出编译命令，帮助IDE提供代码补全
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ─────────────────────────────────────────────────────────────────────────────
# 1. 根据平台不同设置 Flex/Bison 可执行文件路径
# ─────────────────────────────────────────────────────────────────────────────
if(WIN32)
    # Windows 平台使用项目内部的 win_flex_bison
    set(FLEX_EXECUTABLE "${CMAKE_SOURCE_DIR}/3rdparty/win_flex_bison/win_flex.exe")
    set(BISON_EXECUTABLE "${CMAKE_SOURCE_DIR}/3rdparty/win_flex_bison/win_bison.exe")

    if(NOT EXISTS ${FLEX_EXECUTABLE})
        message(FATAL_ERROR "win_flex.exe not found at ${FLEX_EXECUTABLE}")
    endif()
    if(NOT EXISTS ${BISON_EXECUTABLE})
        message(FATAL_ERROR "win_bison.exe not found at ${BISON_EXECUTABLE}")
    endif()

    message(STATUS "Using Flex: ${FLEX_EXECUTABLE}")
    message(STATUS "Using Bison: ${BISON_EXECUTABLE}")
else()
    # Linux/Unix 平台使用系统安装的 flex/bison
    # 让 CMake 自动查找系统中的 flex 和 bison 可执行文件
    # CMake 的 FindFLEX 和 FindBISON 模块会设置 FLEX_EXECUTABLE 和 BISON_EXECUTABLE
endif()

# ─────────────────────────────────────────────────────────────────────────────
# 2. 调用 FindFLEX / FindBISON 模块
#    这样可以使用 BISON_TARGET / FLEX_TARGET / ADD_FLEX_BISON_DEPENDENCY 宏
# ─────────────────────────────────────────────────────────────────────────────
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

message(STATUS "Flex executable: ${FLEX_EXECUTABLE}")
message(STATUS "Bison executable: ${BISON_EXECUTABLE}")

# ─────────────────────────────────────────────────────────────────────────────
# 3. 生成解析器 Parser (Bison) 和 词法分析器 Lexer (Flex)
# ─────────────────────────────────────────────────────────────────────────────
# 指定：Parser 生成 parser.cpp / parser.tab.h
BISON_TARGET(Parser
    ${CMAKE_SOURCE_DIR}/src/Core/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
    COMPILE_FLAGS "--defines"  # 这样会生成 location.hh
)
FLEX_TARGET(Lexer
    ${CMAKE_SOURCE_DIR}/src/Core/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# ─────────────────────────────────────────────────────────────────────────────
# 4. 包含生成文件头文件路径
# ─────────────────────────────────────────────────────────────────────────────
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/Core/AST
    ${CMAKE_SOURCE_DIR}/include
)

# 仅在 Windows 平台包含 win_flex_bison 目录
if(WIN32)
    include_directories(
        ${CMAKE_SOURCE_DIR}/3rdparty/win_flex_bison
    )
endif()

# 对于WSL，确保使用系统安装的flex/bison
if(NOT WIN32)
    # 确保使用系统安装的flex和bison
    message(STATUS "Using system Flex and Bison")
endif()

# ─────────────────────────────────────────────────────────────────────────────
# 5. 定义最终可执行文件 compiler，并将 Bison/Flex 输出添加进去
# ─────────────────────────────────────────────────────────────────────────────
add_executable(compiler
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/Core/PascalParse.cpp

    # Bison / Flex 宏生成的变量
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# ─────────────────────────────────────────────────────────────────────────────
# 6. 设置依赖顺序: 先生成 Parser / Lexer，再编译 compiler
# ─────────────────────────────────────────────────────────────────────────────