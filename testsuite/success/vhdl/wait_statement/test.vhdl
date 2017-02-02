entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
  begin
    for i in 0 to 10 loop
      report "i = " & integer'image(i) severity note;
      wait for 10 ns;
    end loop;
    finish(0);
  end process;

end architecture rtl;
