entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type type_t is (IDLE, START, 'a', STOP);

begin

  process is
    variable a : type_t;
  begin  -- process
    a := IDLE;
    report "a = " & type_t'image(a) severity note;
    wait for 10 ns;
    a := START;
    report "a = " & type_t'image(a) severity note;
    if a /= START then
      report "a = " & type_t'image(a) & ", but expected = START"
        severity failure;
    end if;
    if type_t'pos(a) /= 1 then
      report "type_t'pos(a) = " & integer'image(type_t'pos(a)) & ", but expected = 1"
        severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;
