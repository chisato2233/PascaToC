cmake_minimum_required(VERSION 3.14)
project(PascalToC CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────────────────────────────────────────
# 1. 手动指定 Flex/Bison 可执行文件路径
#    请确保这两个可执行文件确实存在
# ─────────────────────────────────────────────────────────────────────────────
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

# ─────────────────────────────────────────────────────────────────────────────
# 2. 调用 FindFLEX / FindBISON 模块
#    这样可以使用 BISON_TARGET / FLEX_TARGET / ADD_FLEX_BISON_DEPENDENCY 宏
# ─────────────────────────────────────────────────────────────────────────────
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

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
    ${CMAKE_SOURCE_DIR}/3rdparty/win_flex_bison
)

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
# 6. 设置依赖顺序: 先生成 MyParser / MyLexer，再编译 compiler
# ─────────────────────────────────────────────────────────────────────────────