entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is
begin

  process is
  begin 
    finish(0);    
  end process;
  
end architecture rtl;


