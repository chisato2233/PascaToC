
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

long long A[15];

long long B[13];

long long MAX(long long a, long long b) {
if (a >= b){
return a;}
else{
return b;}

}

long long max_sum_nonadjacent(long long n) {
long long i;
long long temp[16];
temp[0] = A[0];
temp[1] = MAX(A[0], A[1]);
for (i = 2; i <=n - 1; i ++){
temp[i] = MAX(temp[i - 2] + A[i], temp[i - 1]);}

return temp[n - 1];
}

long long longest_common_subseq(long long len1, long long len2) {
long long i;
long long j;
long long p[16][16];
for (i = 0; i <=15; i ++){
p[i][0] = 0;}

for (j = 0; j <=15; j ++){
p[0][j] = 0;}

for (i = 1; i <=len1; i ++){
for (j = 1; j <=len2; j ++){
if (A[i - 1] == B[j - 1]){
p[i][j] = p[i - 1][j - 1] + 1;}
else{
p[i][j] = MAX(p[i - 1][j], p[i][j - 1]);}

}

}

return p[len1][len2];
}

int main() {
A[0] = 8;
A[1] = 7;
A[2] = 4;
A[3] = 1;
A[4] = 2;
A[5] = 7;
A[6] = 0;
A[7] = 1;
A[8] = 9;
A[9] = 3;
A[10] = 4;
A[11] = 8;
A[12] = 3;
A[13] = 7;
A[14] = 0;
B[0] = 3;
B[1] = 9;
B[2] = 7;
B[3] = 1;
B[4] = 4;
B[5] = 2;
B[6] = 4;
B[7] = 3;
B[8] = 6;
B[9] = 8;
B[10] = 0;
B[11] = 1;
B[12] = 5;
write(max_sum_nonadjacent(15));

write(longest_common_subseq(15, 13));

}

