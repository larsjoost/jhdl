entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

begin

  test_b : block is
    signal x : integer;
  begin  -- block test_b

    process is
    begin
      x <= 1;
      wait for 1 ns;
      report "x = " & integer'image(x) severity note;
      wait for 1 ns;
      finish(0);
    end process;

  end block test_b;


end architecture rtl;
