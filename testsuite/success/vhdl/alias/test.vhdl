entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  constant size : integer := 4;
  
  type enum_t is ('a', 'b', 'c');

  type array_t is array (natural range <>) of enum_t;

  
begin

  process is
    variable a : array_t(0 to size - 1);
    alias b : array_t(size downto 1) is a;
  begin 
    a(0) := 'b';
    a(size - 1) := 'c';
    report "#1" severity note;
    if (b(size) /= 'b') then
      report "b(size) = " & enum_t'image(b(size)) & ", but expected = 'b'"
        severity failure;
    else
      report "b(size) = " & enum_t'image(b(size))
        severity note;
    end if;
    if b(1) /= 'c' then
      report "b(1) = " & enum_t'image(b(1)) & ", but expected = 'c'"
        severity failure;
    else
      report "b(1) = " & enum_t'image(b(1))
        severity note;
    end if;
    finish(0);
  end process;

end architecture rtl;
