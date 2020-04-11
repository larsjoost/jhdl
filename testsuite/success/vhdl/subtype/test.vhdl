entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type type_t is (IDLE, START, 'a', STOP);

  subtype subtype_t is type_t range START to 'a';
  
begin

  process is
    variable a : subtype_t;
  begin  
    a := START;
    report "a = " & subtype_t'image(a) severity note;
    if a /= START then
      report "a = " & subtype_t'image(a) & ", but expected = START"
        severity failure;
    end if;
    a := 'a';
    report "a = " & subtype_t'image(a) severity note;
    if a /= 'a' then
      report "a = " & subtype_t'image(a) & ", but expected = 'a'"
        severity failure;
    end if;
    if (subtype_t'pos(a) /= 1) then
      report "subtype_t'pos(a) = " & integer'image(subtype_t'pos(a)) & ", but expected = 1"
        severity failure;
    end if;
    finish(0);
  end process;

end architecture rtl;
