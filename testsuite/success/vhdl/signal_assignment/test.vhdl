entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal a : integer;
  signal b : integer;
  signal c : integer;
  signal d : integer;
  signal e : integer;

begin

  b <= 3 when (a = 1) else
       2 when (c = 2) else
       1;

  assign_c: c <= 2;

  d <= b + c;

  e <= 1 when (b = 2) else
       2 when (c = 3);
  
  process is
  begin
    a <= 0;
    for i in 0 to 3 loop
      wait for 5 ns;
      a <= a + 10;
      wait for 5 ns;
      report "a = " & integer'image(a) severity note;
    end loop;
    finish(0);
  end process;

end architecture rtl;
