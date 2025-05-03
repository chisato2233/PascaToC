
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

long long arr1[10][2][3][4][5][6][2];

long long arr2[10][2][3][2][4][8][7];

long long x;
long long y;
long long h;
long long i;
long long j;
long long k;
long long l;
long long m;
long long n;
long long ret;

void loop1(long long x, long long y) {
long long a;
long long b;
long long c;
long long d;
long long e;
long long f;
long long g;
for (a = 0; a <=x - 1; a ++){
for (b = 0; b <=1; b ++){
for (c = 0; c <=2; c ++){
for (d = 0; d <=3; d ++){
for (e = 0; e <=4; e ++){
for (f = 0; f <=5; f ++){
for (g = 0; g <=1; g ++){
arr1[a][b][c][d][e][f][g] = a + b + c + d + e + f + g + x + y;
}

}

}

}

}

}

}

}

void loop2() {
long long a;
long long b;
long long c;
long long d;
long long e;
long long f;
long long g;
for (a = 0; a <=9; a ++){
for (b = 0; b <=1; b ++){
for (c = 0; c <=2; c ++){
for (d = 0; d <=1; d ++){
for (e = 0; e <=3; e ++){
for (f = 0; f <=7; f ++){
for (g = 0; g <=6; g ++){
arr2[a][b][c][d][e][f][g] = a + b + d + g;
}

}

}

}

}

}

}

}

long long loop3(long long h, long long i, long long j, long long k, long long l, long long m, long long n) {
long long ans;
long long a;
long long b;
long long c;
long long d;
long long e;
long long f;
long long g;
ans = 0;
for (a = 0; a <=h - 1; a ++){
for (b = 0; b <=i - 1; b ++){
for (c = 0; c <=j - 1; c ++){
for (d = 0; d <=k - 1; d ++){
for (e = 0; e <=l - 1; e ++){
for (f = 0; f <=m - 1; f ++){
for (g = 0; g <=n - 1; g ++){
ans = mod(ans, 817) + arr1[a][b][c][d][e][f][g] + arr2[a][b][c][d][e][f][g];
}

}

}

}

}

}

}

return ans;
}

int main() {
ret = 0;
x = 1;
y = 2;
h = 3;
i = 4;
j = 5;
k = 6;
l = 7;
m = 8;
n = 9;
loop1(x, y);

loop2();

ret = loop3(h, i, j, k, l, m, n);
write(ret);

}

