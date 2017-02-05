entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
  begin
    for i in 0 to 3 loop
      report "now = " & time'image(now) severity note;
      wait for 3 ns;
    end loop;
    finish(0);
  end process;

end architecture rtl;
