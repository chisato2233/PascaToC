

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
int a;
int b;
a = 10;
b = func(a);
write(b);

}


