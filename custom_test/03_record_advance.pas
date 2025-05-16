program RecordAdvanced;

type
  Point = record
    x, y: real;
  end;
  
  Circle = record
    center: Point;
    radius: real;
  end;
  
  Rectangle = record
    topleft, bottomright: Point;
  end;

var
  circle1: Circle;
  rect1, rect2: Rectangle;
const 
  NotUse = 0;
  PI = 3.14159;
function PointDistance(p1, p2: Point): real;
begin
  PointDistance := sqrt(sqr(p2.x - p1.x) + sqr(p2.y - p1.y));
end;

function CircleArea(c: Circle): real;
begin
  CircleArea := PI * sqr(c.radius);
end;

function RectangleArea(r: Rectangle): real;
var
  width, height: real;
begin
  width := r.bottomright.x - r.topleft.x;
  height := r.topleft.y - r.bottomright.y;
  RectangleArea := width * height;
end;

procedure PrintCircleInfo(c: Circle);
begin
  writeln('Circle:');
  writeln('  Center: (', c.center.x, ',', c.center.y, ')');
  writeln('  Radius: ', c.radius);
  writeln('  Area: ', CircleArea(c));
end;

procedure PrintRectangleInfo(r: Rectangle);
begin
  writeln('Rectangle:');
  writeln('  Top-Left: (', r.topleft.x, ',', r.topleft.y, ')');
  writeln('  Bottom-Right: (', r.bottomright.x, ',', r.bottomright.y, ')');
  writeln('  Area: ', RectangleArea(r));
end;

begin
  { 初始化圆形 }
  circle1.center.x := 5.0;
  circle1.center.y := 5.0;
  circle1.radius := 2.5;
  
  { 初始化矩形1 }
  rect1.topleft.x := 0.0;
  rect1.topleft.y := 0.0;
  rect1.bottomright.x := 4.0;
  rect1.bottomright.y := 3.0;
  
  { 初始化矩形2 }
  rect2.topleft.x := 10.0;
  rect2.topleft.y := 10.0;
  rect2.bottomright.x := 15.0;
  rect2.bottomright.y := 12.0;
  
  { 打印形状信息 }
  writeln('Shape Information:');
  
  PrintCircleInfo(circle1);
  writeln;
  
  PrintRectangleInfo(rect1);
  writeln;
  
  PrintRectangleInfo(rect2);
  writeln;
  
  { 计算两点之间的距离 }
  writeln('Distance between rect1 and rect2: ',
    PointDistance(rect1.topleft, rect2.topleft));
end.