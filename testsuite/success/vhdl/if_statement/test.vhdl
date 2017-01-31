entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  process is
    variable x : integer;
  begin
    x := 10;
    if x = 10 then
      report "Success"
      severity NOTE;
    else
      report "Failure"
        severity Failure;
    end if;
    finish(0);    
  end process;

end architecture rtl;
