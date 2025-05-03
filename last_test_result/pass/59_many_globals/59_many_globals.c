
#include <stdio.h> 
#include <string.h>

void write_int(int x)        { printf("%d", x); }
void write_longlong(long long x)        { printf("%lld", x); }
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
    long long: write_longlong, \
    float: write_float, \
    double: write_double, \
    char: write_char, \
    char*: write_cstr, \
    const char*: write_cstr \
)(x)


#define writeln(x) do { write(x); putchar('\n'); } while(0)

// 为不同类型实现read函数
void read_int(int* x)        { scanf("%d", x); }
void read_longlong(long long* x)        { scanf("%lld", x); }
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
    long long*: read_longlong, \
    float*: read_float, \
    double*: read_double, \
    char*: read_char, \
    const char*: read_cstr \
)(x)

// 字符串需要特殊处理，因为需要指定最大长度
#define read_string(str, max_len) read_cstr(str, max_len)
// 确保模运算结果为正数

// 确保模运算结果为正数
int mod(int a, int m) {
  int result = a % m;
  return result >= 0 ? result : result + m;
}

long long a0;
long long a1;
long long a2;
long long a3;
long long a4;
long long a5;
long long a6;
long long a7;
long long a8;
long long a9;

long long a10;
long long a11;
long long a12;
long long a13;
long long a14;
long long a15;
long long a16;
long long a17;
long long a18;
long long a19;

long long a20;
long long a21;
long long a22;
long long a23;
long long a24;
long long a25;
long long a26;
long long a27;
long long a28;
long long a29;

long long a30;
long long a31;
long long a32;
long long a33;
long long a34;
long long a35;
long long a36;
long long a37;
long long a38;
long long a39;

long long testParam8(long long a0, long long a1, long long a2, long long a3, long long a4, long long a5, long long a6, long long a7) {
return a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7;
}

long long testParam16(long long a0, long long a1, long long a2, long long a3, long long a4, long long a5, long long a6, long long a7, long long a8, long long a9, long long a10, long long a11, long long a12, long long a13, long long a14, long long a15) {
return a0 + a1 + a2 - a3 - a4 - a5 - a6 - a7 + a8 + a9 + a10 + a11 + a12 + a13 + a14 + a15;
}

long long testParam32(long long a0, long long a1, long long a2, long long a3, long long a4, long long a5, long long a6, long long a7, long long a8, long long a9, long long a10, long long a11, long long a12, long long a13, long long a14, long long a15, long long a16, long long a17, long long a18, long long a19, long long a20, long long a21, long long a22, long long a23, long long a24, long long a25, long long a26, long long a27, long long a28, long long a29, long long a30, long long a31) {
return a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9 + a10 + a11 + a12 + a13 + a14 + a15 + a16 + a17 - a18 - a19 - a20 - a21 - a22 + a23 + a24 + a25 + a26 + a27 + a28 + a29 + a30 + a31;
}

int main() {
a0 = 0;
a1 = 1;
a2 = 2;
a3 = 3;
a4 = 4;
a5 = 5;
a6 = 6;
a7 = 7;
a8 = 8;
a9 = 9;
a10 = 0;
a11 = 1;
a12 = 2;
a13 = 3;
a14 = 4;
a15 = 5;
a16 = 6;
a17 = 7;
a18 = 8;
a19 = 9;
a20 = 0;
a21 = 1;
a22 = 2;
a23 = 3;
a24 = 4;
a25 = 5;
a26 = 6;
a27 = 7;
a28 = 8;
a29 = 9;
a30 = 0;
a31 = 1;
a32 = 4;
a33 = 5;
a34 = 6;
a35 = 7;
a36 = 8;
a37 = 9;
a38 = 0;
a39 = 1;
a0 = testParam8(a0, a1, a2, a3, a4, a5, a6, a7);
write(a0);

a0 = testParam16(a32, a33, a34, a35, a36, a37, a38, a39, a8, a9, a10, a11, a12, a13, a14, a15);
write(a0);

a0 = testParam32(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31);
write(a0);

}

