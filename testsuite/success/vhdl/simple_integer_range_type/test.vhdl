entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type test_t is range 1 to 20;

begin

  process is
    variable test_i : test_t;
  begin 
    test_i := 10;
    finish(0);    
  end process;

end architecture rtl;
