entity test is
  port (
    a   : in  integer;
    b   : in  integer;
    q   : out integer);
end entity test; 

architecture rtl of test is

begin  

  q <= a + b;

end architecture rtl;

entity test2 is
  
end entity test2;

library std;
use std.env.finish;

architecture rtl of test2 is

  signal a_i : integer;
  signal b_i : integer;
  signal q_i : integer;
  
begin

  test_1: entity work.test(rtl)
    port map (
      a => a_i,
      b => b_i,
      q => q_i);

  process is
  begin  -- process
    a_i <= 1;
    b_i <= 2;
    wait for 1 ns;
    if q_i /= 3 then
      report "Q = " & integer'image(q_i) & ", but expected = 3" severity failure;
    end if;
    finish(0);
  end process;
  
end architecture rtl;
