program WhileTest;
var
  counter, sum: integer;
  innerCount: integer;  // 将局部变量移到这里
begin
  counter := 1;
  sum := 0;
  
  while counter <= 10 do
  begin
    sum := sum + counter;
    writeln('Counter: ', counter, ', Sum: ', sum);
    counter := counter + 1;
  end;
  
  writeln('Final sum: ', sum);
  
  // 测试嵌套while循环
  counter := 1;
  while counter <= 3 do
  begin
    writeln('Outer loop: ', counter);
    
    // 不使用局部变量声明
    innerCount := 1;
    
    while innerCount <= 2 do
    begin
      writeln('  Inner loop: ', innerCount);
      innerCount := innerCount + 1;
    end;
    
    counter := counter + 1;
  end;
end.