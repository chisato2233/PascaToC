
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

long long arr[32][2];

long long i;

long long param32_rec(long long a1, long long a2, long long a3, long long a4, long long a5, long long a6, long long a7, long long a8, long long a9, long long a10, long long a11, long long a12, long long a13, long long a14, long long a15, long long a16, long long a17, long long a18, long long a19, long long a20, long long a21, long long a22, long long a23, long long a24, long long a25, long long a26, long long a27, long long a28, long long a29, long long a30, long long a31, long long a32) {
if (a1 == 0){
return a2;}
else{
return param32_rec(a1 - 1, mod(a2 + a3, 998244353), a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32, 0);}

}

long long param32_arr() {
long long sum;
sum = arr[0][0] + arr[0][1];
sum = sum + arr[1][0] + arr[1][1];
sum = sum + arr[2][0] + arr[2][1];
sum = sum + arr[3][0] + arr[3][1];
sum = sum + arr[4][0] + arr[4][1];
sum = sum + arr[5][0] + arr[5][1];
sum = sum + arr[6][0] + arr[6][1];
sum = sum + arr[7][0] + arr[7][1];
sum = sum + arr[8][0] + arr[8][1];
sum = sum + arr[9][0] + arr[9][1];
sum = sum + arr[10][0] + arr[10][1];
sum = sum + arr[11][0] + arr[11][1];
sum = sum + arr[12][0] + arr[12][1];
sum = sum + arr[13][0] + arr[13][1];
sum = sum + arr[14][0] + arr[14][1];
sum = sum + arr[15][0] + arr[15][1];
sum = sum + arr[16][0] + arr[16][1];
sum = sum + arr[17][0] + arr[17][1];
sum = sum + arr[18][0] + arr[18][1];
sum = sum + arr[19][0] + arr[19][1];
sum = sum + arr[20][0] + arr[20][1];
sum = sum + arr[21][0] + arr[21][1];
sum = sum + arr[22][0] + arr[22][1];
sum = sum + arr[23][0] + arr[23][1];
sum = sum + arr[24][0] + arr[24][1];
sum = sum + arr[25][0] + arr[25][1];
sum = sum + arr[26][0] + arr[26][1];
sum = sum + arr[27][0] + arr[27][1];
sum = sum + arr[28][0] + arr[28][1];
sum = sum + arr[29][0] + arr[29][1];
sum = sum + arr[30][0] + arr[30][1];
sum = sum + arr[31][0] + arr[31][1];
return sum;
}

long long param16(long long a1, long long a2, long long a3, long long a4, long long a5, long long a6, long long a7, long long a8, long long a9, long long a10, long long a11, long long a12, long long a13, long long a14, long long a15, long long a16) {
long long arr2[16];
arr2[0] = a1;
arr2[1] = a2;
arr2[2] = a3;
arr2[3] = a4;
arr2[4] = a5;
arr2[5] = a6;
arr2[6] = a7;
arr2[7] = a8;
arr2[8] = a9;
arr2[9] = a10;
arr2[10] = a11;
arr2[11] = a12;
arr2[12] = a13;
arr2[13] = a14;
arr2[14] = a15;
arr2[15] = a16;
return param32_rec(arr2[0], arr2[1], arr2[2], arr2[3], arr2[4], arr2[5], arr2[6], arr2[7], arr2[8], arr2[9], arr2[10], arr2[11], arr2[12], arr2[13], arr2[14], arr2[15], a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16);
}

long long getint(long long* index) {
long long input[16];
input[0] = 17;
input[1] = 13;
input[2] = 80;
input[3] = 55;
input[4] = 81;
input[5] = 91;
input[6] = 95;
input[7] = 58;
input[8] = 13;
input[9] = 5;
input[10] = 63;
input[11] = 19;
input[12] = 54;
input[13] = 45;
input[14] = 67;
input[15] = 63;
*index = *index + 1;
return input[*index - 1];
}

int main() {
i = 0;
arr[0][0] = param16(getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i), getint(&i));
arr[0][1] = 8848;
for (i = 1; i <=31; i ++){
arr[i][0] = arr[i - 1][1] - 1;
arr[i][1] = arr[i - 1][0] - 2;
}

write(param32_arr());

}

