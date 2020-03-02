
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  constant SIZE : integer := 5;

  type f_t is array (0 to SIZE - 1) of natural;
  
begin

  process is
    variable f : f_t;
  begin
    f := (1 => 1, 2 | 3 => 2, others => 3);
    if (f(3) /= 2) then
      report "f(3) = " & natural'image(f(3)) & ", but expected = 2" severity failure;
    end if;   
    report "Success" severity note;
    finish(0);
  end process;

end architecture rtl;
