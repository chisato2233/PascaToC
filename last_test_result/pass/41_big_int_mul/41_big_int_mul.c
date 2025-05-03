
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

const long long len = 20;

long long i;
long long j;
long long t;
long long n;
long long temp;

long long len1;
long long len2;

long long mult1[20];
long long mult2[20];

long long c1[25];
long long c2[25];

long long result[40];

int main() {
len1 = len;
len2 = len;
for (i = 0; i <=8; i ++){
mult1[i] = i + 1;}

mult1[9] = 0;
for (i = 10; i <=18; i ++){
mult1[i] = i - 9;}

mult1[19] = 0;
mult2[0] = 2;
mult2[1] = 3;
mult2[2] = 4;
mult2[3] = 2;
mult2[4] = 5;
mult2[5] = 7;
mult2[6] = 9;
mult2[7] = 9;
mult2[8] = 0;
mult2[9] = 1;
mult2[10] = 9;
mult2[11] = 8;
mult2[12] = 7;
mult2[13] = 6;
mult2[14] = 4;
mult2[15] = 3;
mult2[16] = 2;
mult2[17] = 1;
mult2[18] = 2;
mult2[19] = 2;
for (i = 0; i <=len1 - 1; i ++){
c1[i] = mult1[i];}

for (i = 0; i <=len2 - 1; i ++){
c2[i] = mult2[i];}

n = len1 + len2 - 1;
for (i = 0; i <=n; i ++){
result[i] = 0;}

temp = 0;
for (i = 0; i <=len2 - 1; i ++){
t = c2[len2 - 1 - i];
for (j = 0; j <=len1 - 1; j ++){
temp = result[n] + t * c1[len1 - 1 - j];
if (temp >= 10){
result[n] = temp;
result[n - 1] = result[n - 1] + temp / 10;
}
else{
result[n] = temp;
}

n = n - 1;
}

n = n + len1 - 1;
}

if (result[0] != 0){
write(result[0]);
}

for (i = 1; i <=len1 + len2 - 1; i ++){
write(result[i]);
}

}

