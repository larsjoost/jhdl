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
    report "No event expected" severity note;
    wait for 5 ns;
    report "a.event() = " & boolean'image(a'event()) severity note;
    a <= not a;
    wait for 5 ns;
    report "a.event() = " & boolean'image(a'event()) severity note;
    report "Event not received" severity failure;
  end process;

end architecture rtl;
