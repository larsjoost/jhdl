
package test_pkg is

  constant c : integer := 10;
  
  function test_function (
    a : integer;
    b : integer)
    return integer;

end package test_pkg;

package body test_pkg is

  function test_function (
    a : integer;
    b : integer)
    return integer IS
    variable dummy : integer;
  begin
    dummy := a + b;
    return dummy;
  end function test_function;

end package body test_pkg;

entity test is
end entity test;

library work;
use work.test_pkg.c;
use work.test_pkg.test_function;

library std;
use std.env.finish;

architecture rtl of test is

begin  -- architecture rtl

  process is
    variable a : integer;
  begin  -- process
    if c /= 10 then
      report "c = " & integer'image(c) & ", but expected = 10" severity failure;
    end if;
    a := test_function(2, 3);
    if a /= 5 then
      report "test_function(2, 3) = " & integer'image(a) & ", but expected = 5" severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;

