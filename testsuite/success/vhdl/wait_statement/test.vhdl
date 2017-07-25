entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
    variable t : time;
    variable x : time;
    variable y : time;
  begin
    t := 0 ns;
    report "t = " & time'image(t) severity note;
    x := 3 ns;
    y := 10 ns;
    t := x + y;
    report "t = " & time'image(t) severity note;
    for i in 0 to 3 loop
      report "(1.1) now = " & time'image(now) & ", i = " & integer'image(i) severity note;
      wait for 5 ns;
      report "(1.2) Wait done. Now = " & time'image(now) severity note;
    end loop;  -- i
    report "(1.3) now = " & time'image(now) severity note;
    finish(0);
  end process;

  process is
  begin
    report "(2.1) now = " & time'image(now) severity note;
    wait for 2 ns;
    report "(2.2) now = " & time'image(now) severity note;
    wait for 1000 ns;
  end process;

end architecture rtl;
