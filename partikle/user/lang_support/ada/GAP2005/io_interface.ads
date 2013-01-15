package IO_Interface is
   pragma Elaborate_Body;

   type Unsigned_8 is mod 2 ** 8;
   for Unsigned_8'Size use 8;

   type Unsigned_16 is mod 2 ** 16;
   for Unsigned_16'Size use 16;

   type Unsigned_32 is mod 2 ** 32;
   for Unsigned_32'Size use 32;

   type IO_Port is range 0 .. 16#FFFF#;
   for IO_Port'Size use 16;

   function Inb (Port: in IO_Port) return Unsigned_8;
   pragma Inline (Inb);

   function Inw (Port: in IO_Port) return Unsigned_16;
   pragma Inline (Inw);

   function Inl (Port: in IO_Port) return Unsigned_32;
   pragma Inline (Inl);

   procedure Outb (Port: in IO_Port; Val: in Unsigned_8);
   pragma Inline (Outb);

   procedure Outw (Port: in IO_Port; Val: in Unsigned_16);
   pragma Inline (Outw);

   procedure Outl (Port: in IO_Port; Val: in Unsigned_32);
   pragma Inline (Outl);

end IO_Interface;
