program ScopeErrors;

var
  x: integer;

procedure Test;
var
  x: real;
begin
  { 错误18: 重复声明 - 这在过程内部是允许的，但会隐藏外部变量 }
  var x: boolean;  { 在同一作用域内重复声明 }
  
  x := true;
end;

begin
  { 错误18: 重复声明 }
  var x: real;  { x 已经在全局作用域声明过 }
  
  Test;
  writeln(x);  { 使用哪个 x? }
end.