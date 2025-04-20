#!/bin/bash

TIMEOUT=5  # 每个程序最大运行秒数

# 检查 fpc 是否安装
if ! command -v fpc &> /dev/null; then
    echo "错误：请先安装 Free Pascal Compiler（fpc）"
    echo "你可以运行：sudo apt install fpc"
    exit 1
fi

for file in *.pas; do
    if [ -f "$file" ]; then
        base="${file%.pas}"
        executable="./$base"
        input_file="$base.in"
        output_file="$base.out"

        echo "编译 $file ..."
        fpc "$file" -o"$base" > /dev/null

        if [ $? -eq 0 ]; then
            echo "执行 $executable (最大 ${TIMEOUT}s)..."

            if [ -f "$input_file" ]; then
                timeout $TIMEOUT "$executable" < "$input_file" > "$output_file" 2>&1
            else
                timeout $TIMEOUT "$executable" > "$output_file" 2>&1
            fi

            case $? in
                124)
                    echo "⚠️  $file 执行超时（>${TIMEOUT}s），结果已记录到 $output_file"
                    echo "[超时] 程序运行超过 ${TIMEOUT}s 被终止" >> "$output_file"
                    ;;
                0)
                    echo "✅ 执行完成，结果已保存到 $output_file"
                    ;;
                *)
                    echo "⚠️  $file 执行出错，退出码：$?"
                    echo "[运行错误] 程序运行时发生错误，退出码：$?" >> "$output_file"
                    ;;
            esac
        else
            echo "❌ 编译失败：$file"
            echo "[编译失败]" > "$output_file"
        fi

        rm -f "$executable"
    fi
done
