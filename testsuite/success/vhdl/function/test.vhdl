entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  function func (
    x : integer)
    return integer is
  begin
    return x + 1;
  end function func;

begin

  process is
    variable a : integer;
  begin
    a := 0;
    a := func(a);
    if (a /= 1) then
      report "a = " & integer'image(a) & ", but expected 1"
        severity failure;
    else
      report "a = " & integer'image(a) severity note;
    end if;
    finish(0);
  end process;

end architecture rtl;
