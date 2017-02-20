entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is


begin

  generate_loop : for x in 0 to 3 generate

    process is
    begin
      for i in 0 to 3 loop
        report "x = " & integer'image(x) & ", i = " & integer'image(i) severity note;
        wait for 1 ns;
      end loop;
      finish(0);
    end process;

  end generate generate_loop;


end architecture rtl;
