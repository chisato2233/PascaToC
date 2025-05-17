// 在ErrorHandler.cpp中
#include "ErrorHandler.h"
#include <iostream>

ErrorHandler* g_error_handler = nullptr;

void setErrorHandler(ErrorHandler* handler) {
    g_error_handler = handler;
}

void reportError(const char* message, int line, int column) {
    if (g_error_handler) {
        g_error_handler->logError(message, line, column);
    } else {
        std::cerr << "Error at " << line << ":" << column << ": " << message << std::endl;
    }
}