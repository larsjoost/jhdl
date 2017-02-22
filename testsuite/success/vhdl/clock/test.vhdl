entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal clk : bit;
  
begin

  process is
  begin  -- process
    for i in 0 to 10 loop
      clk <= '0';
      wait for 5 ns;
      clk <= '1';
      wait for 5 ns;
    end loop;  -- i
    finish(0);
  end process;

end architecture rtl;
