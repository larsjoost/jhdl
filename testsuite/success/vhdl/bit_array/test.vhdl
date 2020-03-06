
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  constant SIZE : integer := 5;

  type a_t is array (0 to SIZE - 1) of bit;

begin


  process is
    variable a : a_t;
  begin  -- process
    report "start" severity note;
    a        := "00001";
    report "a := 00001" severity note;
    if (a(SIZE - 1) /= '1') then
      report "a = " & bit'image(a(4)) & ", but expected = '1'" severity failure;
    else
      report "a(4) success" severity note;
    end if;
    if (a(0) /= '0') then
      report "a failed" severity failure;
    else
      report "a(0) success" severity note;
    end if;
    finish(0);
  end process;

end architecture rtl;
