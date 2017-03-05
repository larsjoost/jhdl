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

  signal a : integer;
  signal b : integer;
  signal q : integer;
  
begin

  test_1: entity work.test
    port map (
      a => a,
      b => b,
      q => q);

  process is
  begin  -- process
    a <= 1;
    b <= 2;
    wait for 1 ns;
    if q /= 3 then
      report "Q = " & integer'image(q) & ", but expected = 3" severity failure;
    end if;
    finish(0);
  end process;
  
end architecture rtl;
