

#include <stdio.h> 

void write_int(int x)        { printf("%d", x); }
void write_float(float x)    { printf("%f", x); }
void write_double(double x)  { printf("%lf", x); }
void write_char(char x)      { putchar(x); }
void write_cstr(const char* s) { printf("%s", s); }


#define write(x) _Generic((x), \
    int: write_int, \
    float: write_float, \
    double: write_double, \
    char: write_char, \
    char*: write_cstr, \
    const char*: write_cstr \
)(x)


#define writeln(x) do { write(x); putchar('\n'); } while(0)

int main() 
{
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
int a30;
int a31;
int a32;
int a;
int b;
a0 = 0;
a1 = 1;
a2 = 2;
a3 = 3;
a4 = 4;
a5 = 5;
a6 = 6;
a7 = 7;
a8 = 8;
a9 = 9;
a10 = 10;
a11 = 11;
a12 = 12;
a13 = 13;
a14 = 14;
a15 = 15;
a16 = 16;
a17 = 1;
a18 = 2;
a19 = 3;
a20 = 4;
a21 = 5;
a22 = 6;
a23 = 7;
a24 = 8;
a25 = 9;
a26 = 10;
a27 = 11;
a28 = 12;
a29 = 13;
a30 = 14;
a31 = 15;
a32 = 16;
a = 1;
b = (a + (2 * 9));
a = func(a, b);
write(a);

}


