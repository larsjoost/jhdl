entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  function func (
    x : integer := 1)
    return integer is
  begin
    return x + 1;
  end function func;

begin

  process is
    variable a : integer;
  begin
    a := func;
    if (a /= 2) then
      report "a = " & integer'image(a) & ", but expected 2"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    a := func(a);
    if (a /= 3) then
      report "a = " & integer'image(a) & ", but expected 3"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    a := func(x => a);
    if (a /= 4) then
      report "a = " & integer'image(a) & ", but expected 4"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    finish(0);
  end process;

end architecture rtl;
