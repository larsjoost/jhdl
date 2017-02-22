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
    report "a = " & boolean'image(a) severity note;
    wait for 5 ns;
    a <= true;
    report "a = " & boolean'image(a) severity note;
    wait for 5 ns;
    finish(0);
  end process;

end architecture rtl;
