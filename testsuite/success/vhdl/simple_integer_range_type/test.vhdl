entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type test_t is range 1 to 20;

begin

  process is
  begin 
    finish(0);    
  end process;

end architecture rtl;
