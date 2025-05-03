
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

static const char* AA = "A";

static const char* BB = "B";

static const char* CC = "C";

static const char* DD = "D";

static const char* E = "E";

static const char* F = "F";

static const char* G = "G";

static const char* H = "H";

static const char* I = "I";

static const char* J = "J";

static const char* K = "K";

const long long c = 1;

long long a;
long long b;
long long d;

long long i0;
long long i1;
long long i2;
long long i3;
long long i4;

long long set_a(long long val) {
a = val;
return val;
}

long long set_b(long long val) {
b = val;
return val;
}

long long set_d(long long val) {
d = val;
return val;
}

int main() {
a = 2;
b = 3;
if (set_a(0) != 0 && set_b(1) != 0){
/* EmptyStmt */
}

write(a);

write(b);

a = 2;
b = 3;
if (set_a(0) != 0 && set_b(1) != 0){
/* EmptyStmt */
}

write(a);

write(b);

d = 2;
if (c >= 1 && set_d(3) != 0){
/* EmptyStmt */
}

write(d);

if (c <= 1 || set_d(4) != 0){
/* EmptyStmt */
}

write(d);

if (16 >= 3 - (2 + 1)){
write(AA);
}

if (25 - 7 != 36 - 6 * 3){
write(BB);
}

if (1 != mod(7, 2)){
write(CC);
}

if (3 <= 4){
write(DD);
}

if (0 != 0){
write(E);
}

if (1 != 0){
write(F);
}

i0 = 0;
i1 = 1;
i2 = 2;
i3 = 3;
i4 = 4;
if (i0 != 0 || i1 != 0){
write(G);
}

if (i0 >= i1 || i1 <= i0){
write(H);
}

if (i2 >= i1 && i4 != i3){
write(I);
}

if (i0 == 0 && i3 < i3 || i4 >= i4){
write(J);
}

if (i0 == 0 || i3 < i3 && i4 >= i4){
write(K);
}

}

