// 在单独的头文件 ErrorHandler.h 中
#pragma once

class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual void logError(const char* message, int line, int column) = 0;
};

// 全局错误处理器指针
extern ErrorHandler* g_error_handler;

// 设置错误处理器的函数
void setErrorHandler(ErrorHandler* handler);

// 供yyerror使用的错误处理函数
void reportError(const char* message, int line, int column);