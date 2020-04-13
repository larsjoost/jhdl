entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type enum_t is (zero, one);

  function "and" (l, r : enum_t) return enum_t is
    variable x :  enum_t;
  begin
    if (l = one and r = one) then
      x := one;
    else
      x := zero;
    end if;
    return x;
  end function "and";

begin

  process is
    variable a, b, c : enum_t;
  begin
    a := one;
    b := one;
    c := a and b;
    if (c /= one) then
      report "c = " & enum_t'image(c) & ", but expected one" severity FAILURE;
    end if;
    a := zero;
    b := one;
    c := a and b;
    if (c /= zero) then
      report "c = " & enum_t'image(c) & ", but expected zero" severity FAILURE;
    end if;
    finish(0);
  end process;

end architecture rtl;
