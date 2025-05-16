#!/bin/bash
# 错误处理测试脚本

# 测试目录
ERROR_TEST_DIR="error_test"
# 统计计数
total=0
parsed=0  # 成功解析（即使有错误报告）
failed_parse=0  # 解析完全失败

# 创建结果目录，使用当前时间戳
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULT_DIR="result/error_test_${TIMESTAMP}"
PARSED_DIR="$RESULT_DIR/parsed"  # 成功解析的测试（即使有错误）
FAILED_DIR="$RESULT_DIR/failed"  # 解析失败的测试
mkdir -p "$RESULT_DIR" "$PARSED_DIR" "$FAILED_DIR"
echo "错误处理测试结果将保存在: $RESULT_DIR"

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
echo "错误处理测试时间: $(date)" > "$RESULT_DIR/summary.txt"
echo "----------------------------------------------------" >> "$RESULT_DIR/summary.txt"
echo "| 测试用例 | 解析结果 | 检测到的错误数 | 详细信息 |" >> "$RESULT_DIR/summary.txt"
echo "----------------------------------------------------" >> "$RESULT_DIR/summary.txt"

for pas_file in "$ERROR_TEST_DIR"/*.pas; do
    total=$((total+1))
    # 去掉目录和扩展名，得到测试用例基本名
    base=$(basename "$pas_file" .pas)
    echo "测试错误处理：$base"
    
    # 初始化测试结果状态
    parse_status="失败"
    error_count=0
    parse_success=false

    # 进入临时目录进行处理
    cd "$TEMP_DIR"
    
    # 创建logs目录用于保存当前测试的日志
    mkdir -p logs
    
    # 复制原始Pascal文件到临时目录
    cp "$OLDPWD/$pas_file" .
    
    "$OLDPWD/build/pascc" -i "$(basename "$pas_file")" > "$base.errors" 2>&1
    return_code=$?
    
    # 统计检测到的错误数量（可以根据实际输出格式调整）
    error_count=$(grep -c "错误" "$base.errors" || grep -c "error" "$base.errors")
    
    # 判断解析结果
    if [ $return_code -eq 0 ] || [ $return_code -eq 1 ]; then
        # 返回值0表示成功，返回值1表示有错误但继续解析
        echo "  解析成功（检测到 $error_count 个错误）"
        parse_status="成功"
        parse_success=true
        parsed=$((parsed+1))
    else
        # 返回值大于1表示解析完全失败
        echo "  解析完全失败"
        parse_status="完全失败"
        failed_parse=$((failed_parse+1))
    fi
    
    # 根据测试结果决定存放目录
    if [ "$parse_success" = true ]; then
        TARGET_DIR="$OLDPWD/$PARSED_DIR/$base"
    else
        TARGET_DIR="$OLDPWD/$FAILED_DIR/$base"
    fi
    
    # 创建目标目录
    mkdir -p "$TARGET_DIR"
    
    # 复制所有相关文件到目标目录
    cp "$OLDPWD/$pas_file" "$TARGET_DIR/"
    cp "$base.errors" "$TARGET_DIR/"
    
    # 如果有日志文件，创建logs目录并复制
    if [ -f "logs/pascc.log" ]; then
        mkdir -p "$TARGET_DIR/logs"
        cp "logs/pascc.log" "$TARGET_DIR/logs/"
    fi
    
    # 提取简短详细信息（最多前5个错误）
    error_details=$(head -n 5 "$base.errors" | tr '\n' ' ' | cut -c 1-100)
    if [ ${#error_details} -ge 100 ]; then
        error_details="${error_details}..."
    fi
    
    # 记录结果到summary
    echo "| $base | $parse_status | $error_count | $error_details |" >> "$OLDPWD/$RESULT_DIR/summary.txt"
    
    # 返回原目录
    cd "$OLDPWD"
    echo "-------------------------"
done

echo "----------------------------------------------------" >> "$RESULT_DIR/summary.txt"
echo "总测试数: $total, 成功解析: $parsed, 完全失败: $failed_parse" >> "$RESULT_DIR/summary.txt"

echo "总测试数: $total, 成功解析: $parsed, 完全失败: $failed_parse"
echo "成功解析的测试保存在: $PARSED_DIR"
echo "解析失败的测试保存在: $FAILED_DIR"
echo "测试摘要保存在: $RESULT_DIR/summary.txt"
