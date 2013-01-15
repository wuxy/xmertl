with System.Machine_Code; use System.Machine_Code;

package body IO_Interface is
   function Inb (Port: in IO_Port) return Unsigned_8 is
      Tmp : Unsigned_8;
   begin
      Asm ("inb %%dx, %0",
           Unsigned_8'Asm_Output ("=a", Tmp),
           IO_Port'Asm_Input ("d", Port),
           Volatile=> True);
      return Tmp;
   end Inb;
   pragma Inline (Inb);

   function Inw (Port: in IO_Port) return Unsigned_16 is
      Tmp : Unsigned_16;
   begin
      Asm (".byte 0x66 ; inl %%dx, %0",
           Unsigned_16'Asm_Output ("=a", Tmp),
           IO_Port'Asm_Input ("d", Port),
           Volatile => True);
      return Tmp;
   end Inw;
   pragma Inline (Inw);

   function Inl (Port: in IO_Port) return Unsigned_32 is
      Tmp : Unsigned_32;
   begin
      Asm ("inl %%dx, %0",
           Unsigned_32'Asm_Output ("=a", Tmp),
           IO_Port'Asm_Input ("d", Port),
           Volatile => True);
      return Tmp;
   end Inl;
   pragma Inline (Inl);

   procedure Outb (Port: in IO_Port; Val: in Unsigned_8) is
   begin
      Asm ("outb %0, %%dx",
           No_Output_Operands,
           (Unsigned_8'Asm_Input ("a", Val),
            IO_Port'Asm_Input ("d", Port)),
            Volatile => True);
   end Outb;
   pragma Inline (Outb);

   procedure Outw (Port: in IO_Port; Val: in Unsigned_16) is
   begin
      Asm (".byte 0x66; outl %0, %%dx",
           No_Output_Operands,
           (Unsigned_16'Asm_Input ("a", Val),
            IO_Port'Asm_Input ("d", Port)),
            Volatile => True);
   end Outw;
   pragma Inline (Outw);

   procedure Outl (Port: in IO_Port; Val: in Unsigned_32) is
   begin
      Asm ("outl %0, %%dx",
           No_Output_Operands,
           (Unsigned_32'Asm_Input ("a", Val),
            IO_Port'Asm_Input ("d", Port)),
            Volatile => True);
   end Outl;
   pragma Inline (Outl);

   procedure Iopl (Val: Integer);
   pragma Import (C, Iopl, "iopl");
begin
   Iopl (3);
end IO_Interface;
