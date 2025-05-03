
#include <stdio.h> 
#include <string.h>

void write_int(int x)        { printf("%d", x); }
void write_float(float x)    { printf("%f", x); }

void write_char(char x)      { putchar(x); }
void write_cstr(const char* s) { printf("%s", s); }

void write_double(double x) { 
    // 使用E科学计数法，但自行处理指数部分
    char buffer[64];
    sprintf(buffer, " %.16E", x);
    
    // 查找"E+"或"E-"的位置
    char* e_pos = strstr(buffer, "E+");
    if (!e_pos) e_pos = strstr(buffer, "E-");
    
    if (e_pos) {
        // 找到最后两位指数
        char* exp_start = e_pos + 2; // 跳过"E+"或"E-"
        int exp_len = strlen(exp_start);
        
        // 如果指数只有两位，添加前导0
        if (exp_len == 2) {
            // 右移一位
            memmove(exp_start + 1, exp_start, 3); // 包括null终止符
            exp_start[0] = '0'; // 添加前导0
        }
    }
    
    printf("%s", buffer);
}

#define write(x) _Generic((x), \
    int: write_int, \
    float: write_float, \
    double: write_double, \
    char: write_char, \
    char*: write_cstr, \
    const char*: write_cstr \
)(x)


#define writeln(x) do { write(x); putchar('\n'); } while(0)

// 为不同类型实现read函数
void read_int(int* x)        { scanf("%d", x); }
void read_float(float* x)    { scanf("%f", x); }
void read_double(double* x)  { scanf("%lf", x); }
void read_char(char* x)      { 
    // 跳过前导空白字符并读取一个字符
    scanf(" %c", x); 
}
void read_cstr(char* s, int max_len) { 
    // 读取一行字符串，避免缓冲区溢出
    fgets(s, max_len, stdin);
    
    // 移除末尾的换行符（如果有）
    int len = strlen(s);
    if (len > 0 && s[len-1] == '\n') {
        s[len-1] = '\0';
    }
}

// 使用_Generic简化read函数调用
// 注意：由于read需要传递指针，使用方式与write不同
#define read(x) _Generic((x), \
    int*: read_int, \
    float*: read_float, \
    double*: read_double, \
    char*: read_char \
)(x)

// 字符串需要特殊处理，因为需要指定最大长度
#define read_string(str, max_len) read_cstr(str, max_len)

int main() 
{
static const char* u5X8l2s8I2w6D3f2a8C6d7l0H9b6q7K4p8Z6y8O5z2G5s6V8n1d7v8C3g9S9H4k8l8F9x1Q1k2p9Y1w9G5S2h2L6a0X5n5W5e[] = "a";
int U01t9EaA7ZoJ5E8dZfRbP7iKfrW9bLXtxnMDh1QsZnB3S4v0wNvKxvFea8m7uNTjA2U6l5kPzFe8q2CfP0XNq6Pv2r6sX1vG7i2V0KcJ8I0v4Z0r8pY8gE3s5g8b9M5;
double l7J9H5Q3iD7yL0O3bT8oF6uJ2eZ4rN6wV0zA7nU8Y4fM1E2W3uK4R8sZ6c1qX8pG8m4a6S9f2N7xX4m7R3D7m9R3I5n1J2;
bool T2e5I2v7T2o9P9n9h7Q1s7b8g7S6k0x7Z7Y1W4Q0f5G2q7j6c2N7S9t8i5e7z5b9H4x7e6q1W1J1T7k6u7i4K2l4v8o7M6j7;
U01t9EaA7ZoJ5E8dZfRbP7iKfrW9bLXtxnMDh1QsZnB3S4v0wNvKxvFea8m7uNTjA2U6l5kPzFe8q2CfP0XNq6Pv2r6sX1vG7i2V0KcJ8I0v4Z0r8pY8gE3s5g8b9M5 = 0;
l7J9H5Q3iD7yL0O3bT8oF6uJ2eZ4rN6wV0zA7nU8Y4fM1E2W3uK4R8sZ6c1qX8pG8m4a6S9f2N7xX4m7R3D7m9R3I5n1J2 = 0;
T2e5I2v7T2o9P9n9h7Q1s7b8g7S6k0x7Z7Y1W4Q0f5G2q7j6c2N7S9t8i5e7z5b9H4x7e6q1W1J1T7k6u7i4K2l4v8o7M6j7 = 0;
if ((T2e5I2v7T2o9P9n9h7Q1s7b8g7S6k0x7Z7Y1W4Q0f5G2q7j6c2N7S9t8i5e7z5b9H4x7e6q1W1J1T7k6u7i4K2l4v8o7M6j7 == 0)){
write(U01t9EaA7ZoJ5E8dZfRbP7iKfrW9bLXtxnMDh1QsZnB3S4v0wNvKxvFea8m7uNTjA2U6l5kPzFe8q2CfP0XNq6Pv2r6sX1vG7i2V0KcJ8I0v4Z0r8pY8gE3s5g8b9M5,l7J9H5Q3iD7yL0O3bT8oF6uJ2eZ4rN6wV0zA7nU8Y4fM1E2W3uK4R8sZ6c1qX8pG8m4a6S9f2N7xX4m7R3D7m9R3I5n1J2,u5X8l2s8I2w6D3f2a8C6d7l0H9b6q7K4p8Z6y8O5z2G5s6V8n1d7v8C3g9S9H4k8l8F9x1Q1k2p9Y1w9G5S2h2L6a0X5n5W5e);
}

}

