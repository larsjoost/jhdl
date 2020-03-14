
entity test is
end entity test;

library std;
use std.env.finish;

library debug;
use debug.debug.breakpoint;

architecture rtl of test is

  type enum_t is (FIRST, SECOND);

--  constant SIZE : integer := 5;

  constant C_SIZE : positive := 7;

  type c_t is array (enum_t) of string(1 to C_SIZE);

--  type e_t is array (natural range <>) of bit;

begin


  process is
   variable c : c_t;
--    variable e : e_t(0 to SIZE - 1);
  begin  -- process
--    breakpoint;
    c(FIRST) := "Testing";
    report "c(FIRST) := " & C(FIRST) severity note;
    -- if (e'length /= SIZE) then
    --   report "e'length = " & integer'image(e'length) & ", but expected = " & integer'image(SIZE) severity failure;
    -- else
    --   report "e'length success" severity note;
    -- end if;
    --for i in a'range loop
    --  report "a = " & bit'image(i) severity note;
    --end loop;  -- i
    --breakpoint;
    report "Ending" severity note;
    finish(0);
  end process;

end architecture rtl;
