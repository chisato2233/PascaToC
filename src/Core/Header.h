#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include "spdlog/spdlog.h"

struct TokenInfo {
    int token_type;
    std::string token_text;
    int line;
    int column;
    union {
        int int_value;
        double real_value;
        char* str_value;
    } value;
};

// 全局Token流记录
inline static std::vector<TokenInfo> token_stream;
