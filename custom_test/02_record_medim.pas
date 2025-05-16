program RecordMedium;

type
  Date = record
    day: integer;
    month: integer;
    year: integer;
  end;
  
  Address = record
    street: string;
    city: string;
    zipcode: string;
  end;
  
  Person = record
    name: string;
    birthdate: Date;
    home: Address;
  end;

var
  employee: Person;

procedure DisplayPerson(p: Person);
begin
  writeln('Name: ', p.name);
  writeln('Birth Date: ', p.birthdate.day, '/', p.birthdate.month, '/', p.birthdate.year);
  writeln('Address: ', p.home.street, ', ', p.home.city, ' ', p.home.zipcode);
end;

begin
  { 初始化记录 }
  employee.name := 'Jane Smith';
  
  employee.birthdate.day := 15;
  employee.birthdate.month := 3;
  employee.birthdate.year := 1985;
  
  employee.home.street := '123 Main St';
  employee.home.city := 'Boston';
  employee.home.zipcode := '02108';
  
  { 显示记录内容 }
  DisplayPerson(employee);
end.