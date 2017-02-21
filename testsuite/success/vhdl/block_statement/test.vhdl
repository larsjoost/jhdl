entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal x : integer;

begin

  process is
  begin  -- process
    x <= 1;
    wait for 1 ns;
    report "1: x = " & integer'image(x) severity note;
    wait for 1 ns;
    finish(0);
  end process;

  test_b : block is
    signal x : integer;
  begin  -- block test_b

    process is
    begin
      x <= 2;
      wait for 1 ns;
      report "2: x = " & integer'image(x) severity note;
      wait for 1 ns;
      finish(0);
    end process;

  end block test_b;


end architecture rtl;
