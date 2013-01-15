pragma Task_Dispatching_Policy(FIFO_Within_Priorities);
pragma Locking_Policy(Ceiling_Locking);

--  with Ada.Text_IO; use Ada.Text_IO;
with Gnat.IO; use Gnat.IO;

-- 3 nested ATC's

procedure Atc_3 is
pragma Priority(25);

protected PO is
   pragma Priority(25);
   entry E1;
   entry E2;
   entry E3;
   procedure Open_B1;
   procedure Close_B1;
   procedure Open_B2;
   procedure Close_B2;
   procedure Open_B3;
   procedure Close_B3;
private
   B1 : Boolean := False;
   B2 : Boolean := False;
   B3 : Boolean := False;
end PO;

protected body PO is
   entry E1 when B1 is
   begin
      null;
   end E1;
   entry E2 when B2 is
   begin
      null;
   end E2;
   entry E3 when B3 is
   begin
      null;
   end E3;
   procedure Open_B1 is
   begin
      B1 := True;
   end Open_B1;
   procedure Close_B1 is
   begin
      B1 := False;
   end Close_B1;
   procedure Open_B2 is
   begin
      B2 := True;
   end Open_B2;
   procedure Close_B2 is
   begin
      B2 := False;
   end Close_B2;
   procedure Open_B3 is
   begin
      B3 := True;
   end Open_B3;
   procedure Close_B3 is
   begin
      B3 := False;
   end Close_B3;
end PO;


task The_Task is
   pragma Priority(30);
   entry Start;
end The_Task;

task body The_Task is
begin
   accept Start;
   loop
      select
         PO.E1;
         Put_Line("Exit from ATC 1");
      then abort
         select
            PO.E2;
            Put_Line("---Exit from ATC 2");
         then abort
            select
               PO.E3;
               Put_Line("--->>>Exit from ATC 3");
            then abort
               while True loop
                  delay 1.0;
                  -- null;
               end loop;
            end select;
         end select;
      end select;
   end loop;
end The_Task;


begin
   Put_Line("Starting...");
   The_Task.Start;
   delay 0.001;
   Put_Line("Open ATC_3");
   PO.Open_B3;
   delay 0.0001;
   Put_Line("Close ATC_3");
   PO.Close_B3;
   delay 0.0001;
   Put_Line("Open ATC_2");
   PO.Open_B2;
   delay 0.0001;
   Put_Line("Close ATC_2");
   PO.Close_B2;
   delay 0.0001;
   Put_Line("Open ATC_1");
   PO.Open_B1;
   delay 0.0001;
   Put_Line("Close ATC_1");
   PO.Close_B1;
   delay 0.0001;
   Put("Aborting task... ");
   abort The_Task;
   Put_Line("End");
end Atc_3;
