entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal a : boolean;

begin

  process (a) is
  begin
    report "A event" severity note;
    wait for 1 ns;
    finish(0);
  end process;

  process is
  begin
    for i in 0 to 4 loop
      wait for 5 ns;
      a <= false;
      wait for 5 ns;
      a <= true;
    end loop;  -- i
    report "Event not received" severity failure;
  end process;

end architecture rtl;
