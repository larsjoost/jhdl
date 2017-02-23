entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal a : boolean;

begin

  process is
  begin  -- process
    a <= false;
    wait for 5 ns;
    report "A = " & boolean'image(a) severity note;
    if (a) then
      report "A test failed" severity failure;
    end if;
    a <= true;
    wait for 5 ns;
    report "A = " & boolean'image(a) severity note;
    if (not a) then
      report "A test failed" severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;
