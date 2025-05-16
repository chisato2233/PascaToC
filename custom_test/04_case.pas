program CaseTest;
var
  grade: integer;
  result: string;
begin
  grade := 85;
  
  case grade of
    90: result := 'A';
    85: result := 'B';
    75: result := 'C';
    65: result := 'D';
    50: result := 'F'
  end;
  
  writeln('Grade: ', grade);
  writeln('Result: ', result);
  
  // 测试另一个case
  case grade - 80 of
    5: writeln('差值是5');
    10: writeln('差值是10');
    0: writeln('刚好是80')
  end;
end.