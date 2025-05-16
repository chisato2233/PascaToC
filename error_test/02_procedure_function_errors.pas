program ProcedureFunctionErrors;

var
  x, y: integer;
  z: real;

{ 错误5: 过程参数错误 }
procedure PrintValues(a: integer, b: integer,);
begin
  writeln(a, b);
end;

{ 错误6: 函数声明缺少返回类型 }
function Sum(a, b: integer);
begin
  Sum := a + b
end;

{ 错误7: begin/end 不匹配 }
procedure Test;
begin
  if x > 0 then
  begin
    writeln('Positive')
  { 缺少 end }
end;

{ 错误8: 函数调用参数不匹配 }
function Calculate(x, y: integer): integer;
begin
  Calculate := x * y;
end;

begin
  z := Calculate(5);  { 错误10: 函数调用缺少参数 }
  PrintValues(1, 2, 3);  { 参数过多 }
end.