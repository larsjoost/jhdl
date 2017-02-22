entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal clk : bit;
  
begin

  process is
  begin  -- process
    for i in 0 to 4 loop
      report "i = " & integer'image(i) severity note;
      clk <= '0';
      report "x = " & bit'image(clk) severity note;
      wait for 5 ns;
      clk <= '1';
      report "x = " & bit'image(clk) severity note;
      wait for 5 ns;
    end loop;  -- i
    finish(0);
  end process;

end architecture rtl;
