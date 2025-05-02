# 项目结构介绍

- CMakeLists 用于构建整个c++项目
- src 源代码目录
  - main 主函数
  - Core 核心
    - lexer.l Flex进行词法分析
    - parser.y Bison 进行语法分析
    - PascalParse Pascal解析器，将输入的Pascal代码解析成内部数据
    - AST 抽象语法树目录
      - 在AST中定义了语法节点，用这些语法结点生成C语言
    - （待做）接入LLVM生成目标平台汇编
  - （估计不需要这个玩意了）web 网页显示目录
- docs 文档目录
- 3rdparty windows上需要的三方库。
- build cmake构建目录，exe在这里面找，每个人都不一样所以在gitignore中。
- open_set 里是公开的测试用例，.pas文件是源程序，.in文件是程序的输入（如果需要的话），.out文件是程序执行的结果。脚本run_pas.sh编译并执行每个程序得到了结果。
- last_test_result 这里放最终的测试结果
# 注意
好像需要改成linux版本才能弄到头哥上，所以需要迁移一下
FLex + Bison进行词法分析和语法分析

# 快速开始
确保安装过（并且在环境变量里）：
  - Cmake
  - clang（Linux）
  - spdlog

导航到CmakeLists.txt目录下：
```
mkdir build
cd build
cmake ..


```

# 示例
在项目根目录运行：
```
sudo chmod +x ./run_tests.sh
./run_tests.sh
```

结果会保存在 /result/中
./last_test_result保存了最新的测试结果