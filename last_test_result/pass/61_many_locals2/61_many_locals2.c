
#include <stdio.h> 
#include <string.h>

void write_int(int x)        { printf("%d", x); }
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
    float: write_float, \
    double: write_double, \
    char: write_char, \
    char*: write_cstr, \
    const char*: write_cstr \
)(x)


#define writeln(x) do { write(x); putchar('\n'); } while(0)

// 为不同类型实现read函数
void read_int(int* x)        { scanf("%d", x); }
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
    float*: read_float, \
    double*: read_double, \
    char*: read_char \
)(x)

// 字符串需要特殊处理，因为需要指定最大长度
#define read_string(str, max_len) read_cstr(str, max_len)

int main() 
{
int n;
int ret;
int b;
int a0;
int a1;
int a2;
int a3;
int a4;
int a5;
int a6;
int a7;
int a8;
int a9;
int a10;
int a11;
int a12;
int a13;
int a14;
int a15;
int a16;
int a17;
int a18;
int a19;
int a20;
int a21;
int a22;
int a23;
int a24;
int a25;
int a26;
int a27;
int a28;
int a29;
b = 5;
ret = 0;
a0 = 0;
a1 = (a0 + 1);
a2 = (a1 + 1);
a3 = (a2 + 1);
a4 = (a3 + 1);
a5 = (a4 + 1);
a6 = (a5 + 1);
a7 = (a6 + 1);
a8 = (a7 + 1);
a9 = (a8 + 1);
a10 = (a9 + 1);
a11 = (a10 + 1);
a12 = (a11 + 1);
a13 = (a12 + 1);
a14 = (a13 + 1);
a15 = (a14 + 1);
a16 = (a15 + 1);
a17 = (a16 + 1);
a18 = (a17 + 1);
a19 = (a18 + 1);
a20 = (a19 + 1);
a21 = (a20 + 1);
a22 = (a21 + 1);
a23 = (a22 + 1);
a24 = (a23 + 1);
a25 = (a24 + 1);
a26 = (a25 + 1);
a27 = (a26 + 1);
a28 = (a27 + 1);
a29 = (a28 + 1);
write(a0);

write(a1);

write(a2);

write(a3);

write(a4);

write(a5);

write(a6);

write(a7);

write(a8);

write(a9);

write(a10);

write(a11);

write(a12);

write(a13);

write(a14);

write(a15);

write(a16);

write(a17);

write(a18);

write(a19);

write(a20);

write(a21);

write(a22);

write(a23);

write(a24);

write(a25);

write(a26);

write(a27);

write(a28);

write(a29);

write(b);

ret = a25;
write(ret);

}

