
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
  begin
    return a + b;
  end function test_function;

end package body test_pkg;

entity test is
end entity test;

library work;
use work.test_pkg.c;

library std;
use std.env.finish;

architecture rtl of test is

begin  -- architecture rtl

  process is
  begin  -- process
    if c /= 10 then
      report "c = " & integer'image(c) & ", but expected = 10" severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;

