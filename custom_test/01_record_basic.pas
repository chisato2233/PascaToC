program RecordBasic;

type
  Person = record
    name: string;
    age: integer;
  end;

var
  student: Person;

begin
  student.name := 'John Doe';
  student.age := 20;
  
  writeln('Name: ', student.name);
  writeln('Age: ', student.age);
end.