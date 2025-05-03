
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

const double RADIUS = 5.5;

const double PI = 3.1415956535897931;

const double EPS = 9.9999999999999995e-07;

const double EVAL1 = 95.033187999999996;

const long long CONV1 = 233;

const long long MAX = 1000000000;

const double TWO = 2.8999999999999999;

const long long THREE = 3;

const long long FIVE = 5;

static const char* e = "e";

static const char* o = "o";

long long p;

double arr[10];

double input;
double area;
double area_trunc;

double float_abs(double x) {
if (x < 0){
return - x;}
else{
return x;}

}

double circle_area(long long radius) {
return PI * radius * radius + radius * radius * PI / 2;
}

long long float_eq(double a, double b) {
if (float_abs(a - b) < EPS){
return 1;}
else{
return 0;}

}

void error() {
write(e);

}

void ok() {
write(o);

}

void assert(long long cond) {
if (cond == 0){
error();
}
else{
ok();
}

}

int main() {
assert(float_eq(circle_area(5), circle_area(FIVE)));

if (1.5 != 0){
ok();
}

if (~ (3.2999999999999998 == 0)){
ok();
}

if (0 != 0 && 3 != 0){
error();
}

if (0 != 0 || 0.29999999999999999 != 0){
ok();
}

p = 0;
arr[0] = 1;
arr[1] = 2;
input = 0.52000000000000002;
area = PI * input;
area_trunc = circle_area(0);
arr[p] = arr[p] + input;
write(area);

write(area_trunc);

write(arr[0]);

}

