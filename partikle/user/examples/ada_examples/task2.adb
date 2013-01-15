with Ada.Real_Time;
use Ada.Real_Time;
--  with RTL_Pt1;
--  use RTL_Pt1;
with Gnat.IO; use Gnat.IO;
procedure Tasks is

   finished : Boolean := false;
   
   task type Std_Task (Id : Integer) is
     pragma Priority(20+Id);
     entry Call;
   end Std_Task;

   task body Std_Task is
      Next_Time : Time;
      Period : Time_Span := Milliseconds (100);
      cnt : Integer := 0;
      --  Next_While : Time;
      --  Period_While : Time_Span := Microseconds (10);
   begin
      accept Call;
      Next_Time := Clock + Period;
      loop
         cnt := cnt + 1;
         Put ("I am" & integer'image(Id) & " (" & integer'image(cnt) & ")"); New_Line;
         exit when finished;
         --  Next_While := Clock + Period_While;
         --  while Next_While > Clock loop
         --  null;
         --  end loop;
         if (cnt = 5) and (id = 1) then delay 10.0; end if;
         delay until Next_Time;
         Next_Time := Clock + Period;
      end loop;
   end Std_Task;

   Std_Task1 : Std_Task(1);
   Std_Task2 : Std_Task(2);

--  dev : Integer;
begin
   --   dev := Integer(rtl_ktrace_start);
   Std_Task1.Call;
   Std_Task2.Call;
   --  delay 0.005;
   --  dev := Integer(rtl_ktrace_stop);
   put_line("Mode delay 1.0");
   for i in 1..20 loop
    delay 1.0;
   end loop;
   put_line("Mode delay 5.0");
   for i in 1..4 loop
    delay 5.0;
   end loop;
   put_line("Mode delay 20.0");
   delay 20.0;
   finished := true;
end Tasks;
