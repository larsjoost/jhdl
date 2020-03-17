entity test is
end entity test;

library std;
use std.env.finish;
use std.textio.line;
use std.textio.write;
use std.textio.writeline;

library debug;
use debug.debug.BREAKPOINT;

architecture rtl of test is

--  file fh_out     : text open write_mode is "filename.dat";
  
begin

  process is
    variable l : line;
    variable s : integer;
  begin
    s := 123;
    -- BREAKPOINT;
    write(l, s);
    writeline(output, l);
    finish(0);    
  end process;
  
end architecture rtl;


