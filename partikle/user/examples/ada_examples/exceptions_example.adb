--with Ada.Text_Io; use Ada.Text_Io;
with Gnat.IO; use Gnat.IO;
procedure Exceptions_Example is
   procedure Exception_Error is
      Error : exception;
   begin
      raise Error;
   exception
      when Error =>
         Put_Line ("This is an error");
   end Exception_Error;
   Str: String(1..2);
begin
   Exception_Error;

   delay 1.0;
   -- This a CONSTRAINT_ERROR
   Str := "123";
exception
   when CONSTRAINT_ERROR =>
      Put_Line ("--------> This is a CONSTRAINT_ERROR");
end Exceptions_Example;
