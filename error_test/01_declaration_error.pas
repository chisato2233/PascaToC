program DeclarationErrors;

{ 错误1: 常量声明中缺少分号 }
const
  PI = 3.14159
  MAX_SIZE = 100;

{ 错误2: 错误的类型声明 }
type
  Student record  { 缺少等号 }
    name: string;
    age: integer;
  end;

{ 错误3: 变量声明中的类型错误 }
var
  x, y: integer;
  z: float;  { Pascal 中应该是 real 而不是 float }
  arr: array[1..10] of integer
  { 错误4: 缺少分号 }

begin
  writeln('Declaration errors test');
end.