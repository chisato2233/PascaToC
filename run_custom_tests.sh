#!/bin/bash
# 测试目录
TEST_DIR="custom_test"
# 统计计数
total=0
passed=0

# 创建结果目录，使用当前时间戳
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULT_DIR="result/custom_test${TIMESTAMP}"
PASS_DIR="$RESULT_DIR/pass"
FAIL_DIR="$RESULT_DIR/fail"
mkdir -p "$RESULT_DIR" "$PASS_DIR" "$FAIL_DIR"
echo "测试结果将保存在: $RESULT_DIR"

# 检查是否有build目录和pascc可执行文件
if [ ! -d "build" ] || [ ! -f "build/pascc" ]; then
    echo "正在构建pascc..."
    
    # 创建build目录
    mkdir -p build
    cd build
    
    # 配置CMake项目
    cmake ..
    if [ $? -ne 0 ]; then
        echo "CMAKE配置失败"
        exit 1
    fi
    
    # 编译项目
    make
    if [ $? -ne 0 ]; then
        echo "编译失败"
        exit 1
    fi
    
    cd ..
    echo "构建完成"
fi

# 创建临时工作目录
TEMP_DIR=$(mktemp -d)
echo "使用临时目录: $TEMP_DIR"

# 脚本结束时清理临时目录
cleanup() {
    echo "清理临时目录..."
    rm -rf "$TEMP_DIR"
}
trap cleanup EXIT

# 创建summary文件并写入标题
echo "测试时间: $(date)" > "$RESULT_DIR/summary.txt"
echo "-----------------------------------------------------------" >> "$RESULT_DIR/summary.txt"
echo "| 测试用例 | 转换结果 | 编译结果 | 执行结果 | 总体结果 |" >> "$RESULT_DIR/summary.txt"
echo "-----------------------------------------------------------" >> "$RESULT_DIR/summary.txt"

for pas_file in "$TEST_DIR"/*.pas; do
    total=$((total+1))
    # 去掉目录和扩展名，得到测试用例基本名（如 00_main）
    base=$(basename "$pas_file" .pas)
    echo "测试用例：$base"
    
    # 初始化测试结果状态
    convert_status="失败"
    compile_status="未执行" 
    run_status="未执行"
    overall_status="失败"
    test_success=false

    # 进入临时目录进行处理
    cd "$TEMP_DIR"
    
    # 创建logs目录用于保存当前测试的日志
    mkdir -p logs
    
    # 复制原始Pascal文件到临时目录
    cp "$OLDPWD/$pas_file" .
    
    # 将 pas 文件翻译为 .c 文件，使用当前目录生成c文件
    "$OLDPWD/build/pascc" -i "$(basename "$pas_file")" -d -o "$base.c"
    if [ $? -ne 0 ]; then
        echo "  pascc 翻译失败: $pas_file"
        convert_status="失败"
    else
        convert_status="成功"
        
        # 利用 clang 编译生成的 C 文件
        clang "$base.c" -o "$base"
        if [ $? -ne 0 ]; then
            echo "  编译 $base.c 失败"
            compile_status="失败"
        else
            compile_status="成功"
    
            # 执行生成的可执行文件
            # 如果存在与用例同名的 .in 文件，则作为输入
            if [ -f "$OLDPWD/$TEST_DIR/$base.in" ]; then
                "./$base" < "$OLDPWD/$TEST_DIR/$base.in" > "$base.result"
            else
                "./$base" > "$base.result"
            fi
    
            # 比较执行结果和预期输出
            diff -q "$base.result" "$OLDPWD/$TEST_DIR/$base.out" > /dev/null
            if [ $? -eq 0 ]; then
                echo "  测试通过"
                run_status="通过"
                overall_status="通过"
                test_success=true
                passed=$((passed+1))
            else
                echo "  测试失败：输出与预期结果不匹配"
                run_status="失败"
                # 生成差异文件
                diff "$base.result" "$OLDPWD/$TEST_DIR/$base.out" > "$base.diff"
            fi
        fi
    fi
    
    # 根据测试结果决定存放目录
    if [ "$test_success" = true ]; then
        TARGET_DIR="$OLDPWD/$PASS_DIR/$base"
    else
        TARGET_DIR="$OLDPWD/$FAIL_DIR/$base"
    fi
    
    # 创建目标目录
    mkdir -p "$TARGET_DIR"
    
    # 复制所有文件到目标目录
    cp "$OLDPWD/$pas_file" "$TARGET_DIR/"
    [ -f "$base.c" ] && cp "$base.c" "$TARGET_DIR/"
    [ -f "$base" ] && cp "$base" "$TARGET_DIR/"
    [ -f "$base.result" ] && cp "$base.result" "$TARGET_DIR/"
    [ -f "$base.diff" ] && cp "$base.diff" "$TARGET_DIR/"
    
    # 检查并复制所有可能的日志文件
    mkdir -p "$TARGET_DIR/logs"
    # 1. 检查基本日志
    if [ -f "logs/pascc.log" ]; then
        cp "logs/pascc.log" "$TARGET_DIR/logs/"
    fi
    # 2. 检查基于文件名的日志
    if [ -d "logs/$base" ]; then
        cp -r "logs/$base"/* "$TARGET_DIR/logs/"
    fi
    # 3. 检查当前目录下的日志
    if [ -f "compile.log" ]; then
        cp "compile.log" "$TARGET_DIR/logs/"
    fi
    
    # 如果有输入文件，也复制
    if [ -f "$OLDPWD/$TEST_DIR/$base.in" ]; then
        cp "$OLDPWD/$TEST_DIR/$base.in" "$TARGET_DIR/"
    fi
    
    # 复制预期输出
    cp "$OLDPWD/$TEST_DIR/$base.out" "$TARGET_DIR/"
    
    # 记录结果到summary
    echo "| $base | $convert_status | $compile_status | $run_status | $overall_status |" >> "$OLDPWD/$RESULT_DIR/summary.txt"
    
    # 返回原目录
    cd "$OLDPWD"
    echo "-------------------------"
done

echo "-----------------------------------------------------------" >> "$RESULT_DIR/summary.txt"
echo "总测试数: $total, 通过数: $passed, 失败数: $((total-passed))" >> "$RESULT_DIR/summary.txt"

echo "总测试数： $total, 通过数： $passed, 失败数: $((total-passed))"
echo "通过的测试保存在: $PASS_DIR"
echo "失败的测试保存在: $FAIL_DIR"
echo "测试摘要保存在: $RESULT_DIR/summary.txt"
