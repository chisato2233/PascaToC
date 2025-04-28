#!/bin/bash
# 测试目录
TEST_DIR="open_set"
# 统计计数
total=0
passed=0

# 确保脚本退出后清除生成的临时文件（根据需要，可以选择不清除）
cleanup() {
    for f in *.c; do
        [ -f "$f" ] && rm -f "$f"
    done
    for f in *.result; do
        [ -f "$f" ] && rm -f "$f"
    done
    for f in $(ls -1 | grep -E '^[0-9][0-9]_'); do
        [ -f "$f" ] && rm -f "$f"
    done
}
# 可选：脚本结束后清理生成文件
# trap cleanup EXIT

for pas_file in "$TEST_DIR"/*.pas; do
    total=$((total+1))
    # 去掉目录和扩展名，得到测试用例基本名（如 00_main）
    base=$(basename "$pas_file" .pas)
    echo "测试用例：$base"

    # 将 pas 文件翻译为 .c 文件，生成的 xxx.c 放在当前目录下
    pascc -i "$pas_file"
    if [ $? -ne 0 ]; then
        echo "  pascc 翻译失败: $pas_file"
        continue
    fi

    # 利用 gcc 编译生成的 C 文件（这里假设生成的文件名与 .pas 同名）
    gcc "$base.c" -o "$base"
    if [ $? -ne 0 ]; then
        echo "  编译 $base.c 失败"
        continue
    fi

    # 执行生成的可执行文件
    # 如果存在与用例同名的 .in 文件，则作为输入
    if [ -f "$TEST_DIR/$base.in" ]; then
        ./"$base" < "$TEST_DIR/$base.in" > "$base.result"
    else
        ./"$base" > "$base.result"
    fi

    # 比较执行结果和预期输出
    diff -q "$base.result" "$TEST_DIR/$base.out" > /dev/null
    if [ $? -eq 0 ]; then
        echo "  测试通过"
        passed=$((passed+1))
    else
        echo "  测试失败：输出与预期结果不匹配"
    fi

    echo "-------------------------"
done

echo "总测试数： $total, 通过数： $passed"
