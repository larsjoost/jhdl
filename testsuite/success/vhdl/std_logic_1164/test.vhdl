entity test is
end entity test;

library std;
use std.env.finish;

library ieee;
use ieee.std_logic_1164.std_logic_vector;
use ieee.std_logic_1164.std_ulogic_vector;

architecture rtl of test is

  constant SIZE : positive := 5;

  signal a : std_logic_vector(0 to SIZE - 1);
  signal b : std_ulogic_vector(0 to SIZE - 1);

begin

  process is
  begin  -- process
    a <= (others => '0');
    wait for 5 ns;
    b <= std_ulogic_vector(a);
    finish(0);
  end process;

end architecture rtl;
