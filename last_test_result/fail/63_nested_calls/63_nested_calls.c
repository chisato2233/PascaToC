
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

long long i;
long long i1;
long long i2;
long long i3;
long long i4;
long long a;

long long arr[10];

long long func1(long long x, long long y, long long z) {
if (z == 0){
return x * y;}
else{
return func1(x, y - z, 0);}

}

long long func2(long long x, long long y) {
if (y != 0){
return func2(mod(x, y), 0);}
else{
return x;}

}

long long func3(long long x, long long y) {
if (y == 0){
return x + 1;}
else{
return func3(x + y, 0);}

}

long long func4(long long x, long long y, long long z) {
if (x != 0){
return y;}
else{
return z;}

}

long long func5(long long x) {
return - x;
}

long long func6(long long x, long long y) {
if (x != 0 && y != 0){
return 1;}
else{
return 0;}

}

long long func7(long long x) {
if (x == 0){
return 1;}
else{
return 0;}

}

int main() {
i1 = 1;
i2 = 2;
i3 = 3;
i4 = 4;
for (i = 0; i <=9; i ++){
arr[i] = i + 1;}

a = func1(func2(func1(func3(func4(func5(func3(func2(func6(func7(i1), func5(i2)), i3), i4)), arr[0], func1(func2(func3(func4(func5(arr[1]), func6(arr[2], func7(arr[3])), func2(arr[4], func7(arr[5]))), arr[6]), arr[7]), func3(arr[8], func7(arr[9])), i1)), func2(i2, func3(func7(i3), i4))), arr[0], arr[1]), arr[2]), arr[3], func3(func2(func1(func2(func3(arr[4], func5(arr[5])), func5(arr[6])), arr[7], func7(arr[8])), func5(arr[9])), i1));
write(a);

}

