entity test2 is
  port (
    a : in  integer;
    b : in  integer;
    q : out integer);
end entity test2;

architecture rtl of test2 is

  signal q_i : integer;
  
begin

  process (a, b) is
  begin
    q_i <= a + b;
  end process;

  q <= q_i;
  
end architecture rtl;

entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  signal a_i : integer;
  signal b_i : integer;
  signal q_i : integer;

begin

  test_1 : entity work.test2(rtl)
    port map (
      a => a_i,
      b => b_i,
      q => q_i);

  process is
    variable e : integer;
  begin  -- process
    a_i <= 1;
    b_i <= 2;
    wait for 10 ns;
    e   := a_i + b_i;
    report "e = " & integer'image(e) severity note;
    wait for 100 ns;
    report "q = " & integer'image(q_i) severity note;
    if (q_i /= e) then
      report "Q = " & integer'image(q_i) &
        ", but expected = " & integer'image(e)
        severity failure;
    end if;
    report "Simulation done" severity note;
    finish(0);
  end process;

end architecture rtl;
