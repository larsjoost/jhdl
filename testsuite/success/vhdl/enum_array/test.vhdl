
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type enum_t is (FIRST, SECOND);

  type c_t is array (enum_t) of integer;

  constant C_B : c_t := (1, 2);

begin


  process is
  begin  -- process
    report "Simulation starting" severity note;
    if (C_B'length /= 2) then
      report "C_B'length = " & integer'image(C_B'length) & ", but expected = 2"
        severity failure;
    else
      report "C_B'length success" severity note;
    end if;
    if (C_B(FIRST) /= 1) then
      report "b(" & enum_t'image(FIRST) & ") = " & integer'image(C_B(FIRST)) &
        ", but expected 1" severity failure;
    else
      report "C_B(FIRST) success" severity note;
    end if;   
    if (C_B(SECOND) /= 2) then
      report "b(" & enum_t'image(SECOND) & ") = " & integer'image(C_B(SECOND)) &
        ", but expected 2" severity failure;
    else
      report "C_B(SECOND) success" severity note;
    end if;   
    finish(0);
  end process;

end architecture rtl;
