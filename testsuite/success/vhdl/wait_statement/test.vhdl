entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
  begin
    for i in 0 to 3 loop
      report "(1) now = " & time'image(now) severity note;
      wait for 5 ns;
    end loop;
    finish(0);
  end process;

  process is
  begin
    for i in 0 to 3 loop
      report "(2) now = " & time'image(now) severity note;
      wait for 2 ns;
    end loop;
    wait for 1000 ns;
  end process;

end architecture rtl;
