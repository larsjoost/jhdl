
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


--  type bit_t is array (natural range <>) of bit;

--  constant c : bit_vector(0 to 3) := "1010";

  type a_t is array (0 to 4) of bit;

begin


  process is
--    variable a : bit_vector(0 to 5);
    variable a : a_t;
  begin  -- process
    a := "00001";
    wait for 10 ns;
    if (a(4) /= '1') then
      report "a = " & bit'image(a(4)) & ", but expected = '1'" severity failure;
    end if;
    if (a(0) /= '0') then
      report "a failed" severity failure;
    end if;
    --for i in a'range loop
    --  report "a = " & bit'image(i) severity note;
    --end loop;  -- i
    finish(0);
  end process;

end architecture rtl;
