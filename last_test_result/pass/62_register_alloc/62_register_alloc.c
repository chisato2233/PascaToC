
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

long long a;
long long b;

long long func(long long a, long long b) {
long long i;
long long c1;
long long c2;
long long c3;
long long c4;
long long d1;
long long d2;
long long d3;
long long d4;
long long e1;
long long e2;
long long e3;
long long e4;
long long f1;
long long f2;
long long f3;
long long f4;
long long g1;
long long g2;
long long g3;
long long g4;
long long h1;
long long h2;
long long h3;
long long h4;
long long i1;
long long i2;
long long i3;
long long i4;
long long j1;
long long j2;
long long j3;
long long j4;
long long k1;
long long k2;
long long k3;
long long k4;
c1 = 1;
c2 = 2;
c3 = 3;
c4 = 4;
d1 = 1 + c1 + a1;
d2 = 2 + c2 + a2;
d3 = 3 + c3 + a3;
d4 = 4 + c4 + a4;
e1 = 1 + d1 + a5;
e2 = 2 + d2 + a6;
e3 = 3 + d3 + a7;
e4 = 4 + d4 + a8;
f1 = 1 + e1 + a9;
f2 = 2 + e2 + a10;
f3 = 3 + e3 + a11;
f4 = 4 + e4 + a12;
g1 = 1 + f1 + a13;
g2 = 2 + f2 + a14;
g3 = 3 + f3 + a15;
g4 = 4 + f4 + a16;
h1 = 1 + g1 + a17;
h2 = 2 + g2 + a18;
h3 = 3 + g3 + a19;
h4 = 4 + g4 + a20;
i1 = 1 + h1 + a21;
i2 = 2 + h2 + a22;
i3 = 3 + h3 + a23;
i4 = 4 + h4 + a24;
j1 = 1 + i1 + a25;
j2 = 2 + i2 + a26;
j3 = 3 + i3 + a27;
j4 = 4 + i4 + a28;
k1 = 1 + j1 + a29;
k2 = 2 + j2 + a30;
k3 = 3 + j3 + a31;
k4 = 4 + j4 + a32;
i = a - b + 10;
k1 = 1 + j1 + a29;
k2 = 2 + j2 + a30;
k3 = 3 + j3 + a31;
k4 = 4 + j4 + a32;
j1 = 1 + i1 + a25;
j2 = 2 + i2 + a26;
j3 = 3 + i3 + a27;
j4 = 4 + i4 + a28;
i1 = 1 + h1 + a21;
i2 = 2 + h2 + a22;
i3 = 3 + h3 + a23;
i4 = 4 + h4 + a24;
h1 = 1 + g1 + a17;
h2 = 2 + g2 + a18;
h3 = 3 + g3 + a19;
h4 = 4 + g4 + a20;
g1 = 1 + f1 + a13;
g2 = 2 + f2 + a14;
g3 = 3 + f3 + a15;
g4 = 4 + f4 + a16;
f1 = 1 + e1 + a9;
f2 = 2 + e2 + a10;
f3 = 3 + e3 + a11;
f4 = 4 + e4 + a12;
e1 = 1 + d1 + a5;
e2 = 2 + d2 + a6;
e3 = 3 + d3 + a7;
e4 = 4 + d4 + a8;
d1 = 1 + c1 + a1;
d2 = 2 + c2 + a2;
d3 = 3 + c3 + a3;
d4 = 4 + c4 + a4;
d1 = 1 + c1 + a1;
d2 = 2 + c2 + a2;
d3 = 3 + c3 + a3;
d4 = 4 + c4 + a4;
return i + c1 + c2 + c3 + c4 - d1 - d2 - d3 - d4 + e1 + e2 + e3 + e4 - f1 - f2 - f3 - f4 + g1 + g2 + g3 + g4 - h1 - h2 - h3 - h4 + i1 + i2 + i3 + i4 - j1 - j2 - j3 - j4 + k1 + k2 + k3 + k4 + a1 - a2 + a3 - a4 + a5 - a6 + a7 - a8 + a9 - a10 + a11 - a12 + a13 - a14 + a15 - a16 + a17 - a18 + a19 - a20 + a21 - a22 + a23 - a24 + a25 - a26 + a27 - a28 + a29 - a30 + a31 - a32;
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
a10 = 10;
a11 = 11;
a12 = 12;
a13 = 13;
a14 = 14;
a15 = 15;
a16 = 16;
a17 = 1;
a18 = 2;
a19 = 3;
a20 = 4;
a21 = 5;
a22 = 6;
a23 = 7;
a24 = 8;
a25 = 9;
a26 = 10;
a27 = 11;
a28 = 12;
a29 = 13;
a30 = 14;
a31 = 15;
a32 = 16;
a = 1;
b = a + 2 * 9;
a = func(a, b);
write(a);

}

