program ExpressionErrors;

var
  x, y: integer;
  z: real;
  arr: array[1..10] of integer;

function Sum(a, b: integer): integer;
begin
  Sum := a + b;
end;

begin
  { 错误12: 表达式中运算符错误 }
  y := 5 ++ 3;
  
  { 错误14: 非法字符 }
  x := 10 @ 5;  { @ 不是有效的运算符 }
  
  { 错误15: 未闭合的字符串 }
  writeln('Hello, world);
  
  { 错误16: 数组访问越界 }
  arr[0] := 100;  { 数组定义为 1..10 }
  
  { 错误20: 错误的函数返回值类型 }
  z := Sum(5, 10);  { Sum 返回整数，z 是 real - 这可能是类型转换问题而非语法错误 }
end.