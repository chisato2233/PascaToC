
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

long long a;
long long b;
long long c;
long long d;
long long e;
long long f;
long long g;
long long h;
long long i;
long long j;
long long k;
long long l;
long long m;
long long n;
long long o;
long long p;
long long q;
long long r;
long long s;
long long t;
long long u;
long long v;
long long w;
long long x;

long long sum;
long long sum1;
long long sum2;
long long sum3;

long long foo() {
long long arr[16];
long long a;
long long b;
long long c;
long long d;
long long e;
long long f;
long long g;
long long h;
long long i;
long long j;
long long k;
long long l;
long long m;
long long n;
long long o;
long long p;
long long sum1;
long long sum2;
arr[0] = 0;
arr[1] = 1;
arr[2] = 2;
arr[3] = 3;
arr[4] = 0;
arr[5] = 1;
arr[6] = 2;
arr[7] = 3;
arr[8] = 0;
arr[9] = 1;
arr[10] = 2;
arr[11] = 3;
arr[12] = 0;
arr[13] = 1;
arr[14] = 2;
arr[15] = 3;
a = 3;
b = 7;
c = 5;
d = 6;
e = 1;
f = 0;
g = 3;
h = 5;
i = 4;
j = 2;
k = 7;
l = 9;
m = 8;
n = 1;
o = 4;
p = 6;
sum1 = a + b + c + d + e + f + g + h;
sum2 = i + j + k + l + m + n + o + p;
return sum1 + sum2 + arr[a];
}

int main() {
a = 3;
b = 7;
c = 5;
d = 6;
e = 1;
f = 0;
g = 3;
h = 5;
i = 4;
j = 2;
k = 7;
l = 9;
m = 8;
n = 1;
o = 4;
p = 6;
sum1 = a + b + c + d + e + f + g + h;
sum2 = i + j + k + l + m + n + o + p;
sum1 = sum1 + foo();
q = 4;
r = 7;
s = 2;
t = 5;
u = 8;
v = 0;
w = 6;
x = 3;
sum2 = sum2 + foo();
a = i;
b = j;
c = k;
d = l;
e = m;
f = n;
g = o;
h = p;
sum3 = q + r + s + t + u + v + w + x;
sum = sum1 + sum2 + sum3;
write(sum);

}

