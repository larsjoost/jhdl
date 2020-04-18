entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type u_bool_t is (ZERO, ONE);

  type u_bool_array_t is array (natural range <>) of u_bool_t;

  function resolve (
    arg : u_bool_array_t)
    return u_bool_t is
    variable x : u_bool_t;
  begin
    x := ZERO;
    for i in arg'range loop
      if (arg(i) = ONE) then
        x := ONE;
      end if;
    end loop;
    return x;
  end function resolve;

  subtype bool_t is resolve u_bool_t;

  signal a, b, c, d : bool_t;
  
begin

  a <= ZERO;

  b <= ZERO;

  c <= ONE;

  d <= ONE;
  
  process is
  begin
    a <= ZERO;
    b <= ONE;
    c <= ZERO;
    d <= ONE;
    wait for 5 ns;
    if (a /= ZERO) then
      report "a = " & bool_t'image(a) & ", but expected ZERO" severity failure;
    end if;
    if (b /= ONE) then
      report "b = " & bool_t'image(b) & ", but expected ONE" severity failure;
    end if;
    if (c /= ONE) then
      report "c = " & bool_t'image(c) & ", but expected ONE" severity failure;
    end if;
    if (d /= ONE) then
      report "d = " & bool_t'image(d) & ", but expected ONE" severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;
