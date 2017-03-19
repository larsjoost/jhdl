entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  function func (
    a : integer range 2 to 4 := 2;
    b : integer := 1)
    return integer is
  begin
    return a - b + 1;
  end function func;

begin

  process is
    variable a : integer;
    variable b : integer;
  begin
    a := func;
    if (a /= 2) then
      report "a = " & integer'image(a) & ", but expected 2"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    a := 0;
    a := func(a);
    if (a /= 0) then
      report "a = " & integer'image(a) & ", but expected 0"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    a := 2;
    a := func(b => a);
    if (a /= 1) then
      report "a = " & integer'image(a) & ", but expected 1"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    a := 2;
    b := 1;
    a := func(b => b, a => a);
    if (a /= 2) then
      report "a = " & integer'image(a) & ", but expected 2"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    finish(0);
  end process;

end architecture rtl;
