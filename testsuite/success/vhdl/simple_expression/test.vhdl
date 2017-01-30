entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
    variable x : integer;
  begin
    x := 10 + 4;
    report "x = " & integer'image(x) severity NOTE;
    finish(0);    
  end process;

end architecture rtl;
