entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal a : integer;

begin

  process is
  begin
    a <= 0;
    for i in 0 to 3 loop
      wait for 5 ns;
      a <= a + 10;
      report "a = " & integer'image(a) severity note;
    end loop;
    finish(0);
  end process;

end architecture rtl;
