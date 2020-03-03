entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  constant k1 : integer := 0;
  constant k2 : integer := 20;
  
  type test_t is range k1 + 1 to k2 - 1;

begin

  process is
    variable test_i : test_t;
  begin 
    test_i := 10;
    if (test_i /= 10) then
      report "test_i = " & integer'image(test_i) &
        ", but expected = 10" severity failure;
    end if;     
    finish(0);    
  end process;

end architecture rtl;
