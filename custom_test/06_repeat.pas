program RepeatTest;
var
  counter, sum: integer;
  innerCount: integer;
begin
  counter := 1;
  sum := 0;
  
  repeat
    sum := sum + counter;
    writeln('Counter: ', counter, ', Sum: ', sum);
    counter := counter + 1;
  until counter > 10;
  
  writeln('Final sum: ', sum);
  
  // 测试嵌套repeat循环
  counter := 1;
  repeat
    writeln('Outer loop: ', counter);
    
    // 移除var关键字，直接使用之前声明的变量
    innerCount := 1;
    
    repeat
      writeln('  Inner loop: ', innerCount);
      innerCount := innerCount + 1;
    until innerCount > 2;
    
    counter := counter + 1;
  until counter > 3;
  
  // 测试至少执行一次的特性
  counter := 5;
  repeat
    writeln('This will execute once even though condition is false');
    counter := counter + 1;
  until counter > 5;
end.