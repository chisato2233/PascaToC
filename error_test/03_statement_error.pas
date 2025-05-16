program StatementErrors;

var
  x, y: integer;
  z: real;
  arr: array[1..10] of integer;

begin
  { 错误9: 赋值语句语法错误 }
  x = 10;  { 应该用 := 而不是 = }
  
  { 错误11: if 语句缺少 then }
  if x > 10
    writeln('Greater than 10');
  
  { 错误13: for 循环语法错误 }
  for i := 1 to 10 by 2 do  { Pascal 中没有 by 关键字 }
    writeln(i);
  
  { 错误17: case 语句错误 }
  case x of
    1: writeln('One')
    2: writeln('Two');  { 缺少分号 }
  end;
  
  { 错误19: 变量未声明 }
  counter := counter + 1;
end.