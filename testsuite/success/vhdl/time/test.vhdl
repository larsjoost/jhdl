entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
    variable t : time;
  begin
    t := 10 ns;
    report "time = " & time'image(t) severity note;
    report "now = " & time'image(now) severity note;
    finish(0);
  end process;

end architecture rtl;
