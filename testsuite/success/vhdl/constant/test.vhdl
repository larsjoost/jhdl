
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  constant c : integer := 2;
  
  signal a : integer;

begin


  process is
  begin  -- process
    a <= c;
    wait for 10 ns;
    if (a /= 2) then
      report "a = " & integer'image(a) &
        ", but expected = 2"
        severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;
