
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type enum_t is (FIRST, SECOND);

  constant SIZE : integer := 5;

  type a_t is array (0 to SIZE - 1) of bit;

  constant C_SIZE : positive := 7;

  type c_t is array (enum_t) of string(1 to C_SIZE);

  type e_t is array (natural range <>) of bit;

begin


  process is
    variable a : a_t;
    variable c : c_t;
    variable e : e_t(0 to SIZE - 1);
  begin  -- process
    report "start" severity note;
    a        := "00001";
    report "a := 00001" severity note;
    c(FIRST) := "Tests";
    report "c(FIRST) := Testing" severity note;
    if (e'length /= SIZE) then
      report "e'length = " & integer'image(e'length) & ", but expected = " & integer'image(SIZE) severity failure;
    else
      report "e'length success" severity note;
    end if;
    if (a(4) /= '1') then
      report "a = " & bit'image(a(4)) & ", but expected = '1'" severity failure;
    else
      report "a(4) success" severity note;
    end if;
    if (a(0) /= '0') then
      report "a failed" severity failure;
    else
      report "a(0) success" severity note;
    end if;
    --for i in a'range loop
    --  report "a = " & bit'image(i) severity note;
    --end loop;  -- i
    finish(0);
  end process;

end architecture rtl;
