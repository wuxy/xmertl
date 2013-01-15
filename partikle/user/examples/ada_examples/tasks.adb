with Ada.Real_Time;
use Ada.Real_Time;
--  with RTL_Pt1;
--  use RTL_Pt1;
with Gnat.IO; use Gnat.IO;
procedure Tasks is

   task type Std_Task (Id : Integer) is
     pragma Priority(Id);
     entry Call;
   end Std_Task;

   task body Std_Task is
      Next_Time : Time;
      Period : Time_Span := Milliseconds (1);
      --  Next_While : Time;
      --  Period_While : Time_Span := Microseconds (10);
   begin
      accept Call;
      Next_Time := Clock + Period;
      loop
         Put ("I am "); Put (Id); New_Line;
         --  Next_While := Clock + Period_While;
         --  while Next_While > Clock loop
         --  null;
         --  end loop;
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
end Tasks;
