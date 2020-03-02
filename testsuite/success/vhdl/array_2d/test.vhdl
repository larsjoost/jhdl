
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type enum_t is (FIRST, SECOND);

  type d_t is array(enum_t, enum_t) of integer;

  constant C_D : d_t := (
    (1, 2),
    (3, 4)
    );

begin


  process is
  begin  -- process
    if (C_D(FIRST, SECOND) /= 2) then
      report "C_D(FIRST, SECOND) = " & integer'image(C_D(FIRST, SECOND)) & ", but expected 2" severity failure;
    else
      report "C_D(FIRST, SECOND) success" severity note;
    end if;   
        finish(0);
  end process;

end architecture rtl;
