
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

const long long modn = 1000000007;

long long dp[18][18][18][18][18][7];

long long list[200];

long long cns[19];

long long n;

long long i;
long long j;
long long k;
long long l;
long long m;
long long h;

long long ans;

long long equal(long long a, long long b) {
if (a == b){
return 1;}
else{
return 0;}

}

long long dfs(long long a, long long b, long long c, long long d, long long e, long long last) {
long long anss;
if (dp[a][b][c][d][e][last] != - 1){
return dp[a][b][c][d][e][last];}

if (a + b + c + d + e == 0){
return 1;}
else{
anss = 0;
if (a != 0){
anss = mod(anss + (a - equal(last, 2)) * dfs(a - 1, b, c, d, e, 1), modn);}

if (b != 0){
anss = mod(anss + (b - equal(last, 3)) * dfs(a + 1, b - 1, c, d, e, 2), modn);}

if (c != 0){
anss = mod(anss + (c - equal(last, 4)) * dfs(a, b + 1, c - 1, d, e, 3), modn);}

if (d != 0){
anss = mod(anss + (d - equal(last, 5)) * dfs(a, b, c + 1, d - 1, e, 4), modn);}

if (e != 0){
anss = mod(anss + e * dfs(a, b, c, d + 1, e - 1, 5), modn);}

dp[a][b][c][d][e][last] = mod(anss, modn);
return dp[a][b][c][d][e][last];
}

}

int main() {
read(&n);

for (i = 0; i <=17; i ++){
for (j = 0; j <=17; j ++){
for (k = 0; k <=17; k ++){
for (l = 0; l <=17; l ++){
for (m = 0; m <=17; m ++){
for (h = 0; h <=6; h ++){
dp[i][j][k][l][m][h] = - 1;}

}

}

}

}

}

for (i = 0; i <=n - 1; i ++){
// Warning: 下面的表达式可能不是一个有效的左值
read(&(list[i]));

cns[list[i]] = cns[list[i]] + 1;
}

ans = dfs(cns[1], cns[2], cns[3], cns[4], cns[5], 0);
write(ans);

}

