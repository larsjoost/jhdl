
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


  type bit_vector is array (natural range <>) of bit;

  constant c : bit_vector(0 to 3) := "1010";

  type a_t is array (0 to 4) of bit;

begin


  process is
    variable a : bit_vector(0 to 5);
  begin  -- process
    a := "00001";
    wait for 10 ns;
    if (a /= 2) then
      report "a = " & integer'image(a) &
        ", but expected = 2"
        severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;
