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
  
  ShapeKind = (CircleShape, RectangleShape);
  
  Shape = record
    kind: ShapeKind;
    case ShapeKind of
      CircleShape: (circle: Circle);
      RectangleShape: (rect: Rectangle);
  end;

var
  shapes: array[1..3] of Shape;
  
function PointDistance(p1, p2: Point): real;
begin
  PointDistance := sqrt(sqr(p2.x - p1.x) + sqr(p2.y - p1.y));
end;

function CircleArea(c: Circle): real;
const
  PI = 3.14159;
begin
  CircleArea := PI * sqr(c.radius);
end;

function RectangleArea(r: Rectangle): real;
var
  width, height: real;
begin
  width := r.bottomright.x - r.topleft.x;
  height := r.bottomright.y - r.topleft.y;
  RectangleArea := width * height;
end;

procedure PrintShapeInfo(s: Shape);
begin
  case s.kind of
    CircleShape: 
      begin
        writeln('Circle:');
        writeln('  Center: (', s.circle.center.x:0:2, ',', s.circle.center.y:0:2, ')');
        writeln('  Radius: ', s.circle.radius:0:2);
        writeln('  Area: ', CircleArea(s.circle):0:2);
      end;
    RectangleShape:
      begin
        writeln('Rectangle:');
        writeln('  Top-Left: (', s.rect.topleft.x:0:2, ',', s.rect.topleft.y:0:2, ')');
        writeln('  Bottom-Right: (', s.rect.bottomright.x:0:2, ',', s.rect.bottomright.y:0:2, ')');
        writeln('  Area: ', RectangleArea(s.rect):0:2);
      end;
  end;
end;

begin
  { 初始化圆形 }
  shapes[1].kind := CircleShape;
  shapes[1].circle.center.x := 5.0;
  shapes[1].circle.center.y := 5.0;
  shapes[1].circle.radius := 2.5;
  
  { 初始化矩形1 }
  shapes[2].kind := RectangleShape;
  shapes[2].rect.topleft.x := 0.0;
  shapes[2].rect.topleft.y := 0.0;
  shapes[2].rect.bottomright.x := 4.0;
  shapes[2].rect.bottomright.y := 3.0;
  
  { 初始化矩形2 }
  shapes[3].kind := RectangleShape;
  shapes[3].rect.topleft.x := 10.0;
  shapes[3].rect.topleft.y := 10.0;
  shapes[3].rect.bottomright.x := 15.0;
  shapes[3].rect.bottomright.y := 12.0;
  
  { 打印形状信息 }
  writeln('Shape Information:');
  for i := 1 to 3 do
  begin
    PrintShapeInfo(shapes[i]);
    writeln;
  end;
  
  { 计算两点之间的距离 }
  writeln('Distance between shapes[2] and shapes[3]: ',
    PointDistance(shapes[2].rect.topleft, shapes[3].rect.topleft):0:2);
end.