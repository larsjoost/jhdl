entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type SEVERITY_LEVEL is (NOTE, WARNING, ERROR, FAILURE);

begin

  process is
  begin 
    report "Test is almost done" severity note;
    finish(0);    
  end process;

end architecture rtl;
