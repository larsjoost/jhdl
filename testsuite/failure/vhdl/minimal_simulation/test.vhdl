entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is
begin

  process is
  begin 
    report "Finish with error" severity error;
    finish(1);    
  end process;
  
end architecture rtl;


