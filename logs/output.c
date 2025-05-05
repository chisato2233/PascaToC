
#include <stdio.h> 
#include <string.h>

void write_int(int x)        { printf("%d", x); }
void write_longlong(long long x)        { printf("%lld", x); }
void write_float(float x)    { printf("%f", x); }

void write_char(char x)      { putchar(x); }
void write_cstr(const char* s) { printf("%s", s); }
void write_double_short(double x) { printf("%.6f", x); }
void write_double_long(double x) { 
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
    double: write_double_short, \
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
    return a % m;
}

long long k;

long long i;
long long j;

double ret0;
double ret1;

double arr[40][3];

long long arr2[24][3];

double params_f40(double x0, double x1, double x2, double x3, double x4, double x5, double x6, double x7, double x8, double x9, double x10, double x11, double x12, double x13, double x14, double x15, double x16, double x17, double x18, double x19, double x20, double x21, double x22, double x23, double x24, double x25, double x26, double x27, double x28, double x29, double x30, double x31, double x32, double x33, double x34, double x35, double x36, double x37, double x38, double x39)  {
double __fun_return_params_f40;
long long i;
double arr[10];
if (x39 != 0){
arr[0] = x0 + x1 + x2 + x3;
arr[1] = x4 + x5 + x6 + x7;
arr[2] = x8 + x9 + x10 + x11;
arr[3] = x12 + x13 + x14 + x15;
arr[4] = x16 + x17 + x18 + x19;
arr[5] = x20 + x21 + x22 + x23;
arr[6] = x24 + x25 + x26 + x27;
arr[7] = x28 + x29 + x30 + x31;
arr[8] = x32 + x33 + x34 + x35;
arr[9] = x36 + x37 + x38 + x39;
for (i = 0; i <=9; i ++){
write(arr[i]);
}

__fun_return_params_f40 = arr[k];
}
else{
__fun_return_params_f40 = params_f40(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31, x32, x33, x34, x35, x36, x37, x38, x39, x0 + x1 + x2);
}

return __fun_return_params_f40;}

double params_f40_i24(long long i23, long long i2, long long i6, double x4, long long i1, long long i4, long long i5, double x8, double x15, double x7, long long i22, double x3, double x28, long long i0, double x37, long long i19, double x30, double x12, double x1, double x11, double x38, double x6, long long i7, double x32, long long i10, long long i13, double x20, double x33, double x23, double x9, double x25, long long i8, double x39, long long i17, double x21, double x16, double x5, double x34, long long i18, long long i9, double x14, double x10, double x0, long long i12, double x31, long long i11, long long i16, double x27, double x24, double x13, double x29, long long i3, long long i21, long long i20, double x18, double x19, double x22, double x26, double x36, double x17, long long i15, double x2, long long i14, double x35)  {
double __fun_return_params_f40_i24;
long long i;
double arr[10];
double arr2[8];
if (i23 != 0){
arr[0] = x0 + x1 + x2 + x3;
arr[1] = x4 + x5 + x6 + x7;
arr[2] = x8 + x9 + x10 + x11;
arr[3] = x12 + x13 + x14 + x15;
arr[4] = x16 + x17 + x18 + x19;
arr[5] = x20 + x21 + x22 + x23;
arr[6] = x24 + x25 + x26 + x27;
arr[7] = x28 + x29 + x30 + x31;
arr[8] = x32 + x33 + x34 + x35;
arr[9] = x36 + x37 + x38 + x39;
arr2[0] = (i0 + i1 + i2) * 1;
arr2[1] = (i3 + i4 + i5) * 1;
arr2[2] = (i6 + i7 + i8) * 1;
arr2[3] = (i9 + i10 + i11) * 1;
arr2[4] = (i12 + i13 + i14) * 1;
arr2[5] = (i15 + i16 + i17) * 1;
arr2[6] = (i18 + i19 + i20) * 1;
arr2[7] = (i21 + i22 + i23) * 1;
for (i = 0; i <=9; i ++){
write(arr[i]);
}

for (i = 0; i <=7; i ++){
write(arr2[i]);
}

for (i = 0; i <=7; i ++){
arr2[i] = arr2[i] - arr[i];}

__fun_return_params_f40_i24 = arr2[k];
}
else{
__fun_return_params_f40_i24 = params_f40_i24(i1, i2, i6, x4, i1, i4, i5, x8, x15, x7, i22, x3, x28, i0, x37, i19, x30, x12, x1, x11, x38, x6, i7, x32, i10, i13, x20, x33, x23, x9, x25, i8, x39, i17, x21, x16, x5, x34, i18, i9, x14, x10, x0, i12, x31, i11, i16, x27, x24, x13, x29, i3, i21, i20, x18, x19, x22, x26, x36, x17, i15, x2, i14, x35);}

return __fun_return_params_f40_i24;}

int main() {
k = 0;
for (i = 0; i <=23; i ++){
for (j = 0; j <=2; j ++){
arr2[i][j] = i * 2 - j * 3;}
}

for (i = 0; i <=39; i ++){
for (j = 0; j <=2; j ++){
arr[i][j] = i * 2.2000000000000002 - j * 3.2999999999999998;}
}

ret0 = params_f40(arr[0][k], arr[1][k], arr[2][k], arr[3][k], arr[4][k], arr[5][k], arr[6][k], arr[7][k], arr[8][k], arr[9][k], arr[10][k], arr[11][k], arr[12][k], arr[13][k], arr[14][k], arr[15][k], arr[16][k], arr[17][k], arr[18][k], arr[19][k], arr[20][k], arr[21][k], arr[22][k], arr[23][k], arr[24][k], arr[25][k], arr[26][k], arr[27][k], arr[28][k], arr[29][k], arr[30][k], arr[31][k], arr[32][k], arr[33][k], arr[34][k], arr[35][k], arr[36][k], arr[37][k], arr[38][k], arr[39][k]);
ret1 = params_f40_i24(arr2[23][k], arr2[2][k], arr2[6][k], arr[4][k], arr2[1][k], arr2[4][k], arr2[5][k], arr[8][k], arr[15][k], arr[7][k], arr2[22][k], arr[3][k], arr[28][k], arr2[0][k], arr[37][k], arr2[19][k], arr[30][k], arr[12][k], arr[1][k], arr[11][k], arr[38][k], arr[6][k], arr2[7][k], arr[32][k], arr2[10][k], arr2[13][k], arr[20][k], arr[33][k], arr[23][k], arr[9][k], arr[25][k], arr2[8][k], arr[39][k], arr2[17][k], arr[21][k], arr[16][k], arr[5][k], arr[34][k], arr2[18][k], arr2[9][k], arr[14][k], arr[10][k], arr[0][k], arr2[12][k], arr[31][k], arr2[11][k], arr2[16][k], arr[27][k], arr[24][k], arr[13][k], arr[29][k], arr2[3][k], arr2[21][k], arr2[20][k], arr[18][k], arr[19][k], arr[22][k], arr[26][k], arr[36][k], arr[17][k], arr2[15][k], arr[2][k], arr2[14][k], arr[35][k]);
write(ret0);

write(ret1);

}

