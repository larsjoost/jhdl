
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

begin


  process is
    variable a : integer range -4 to 20;
    variable b : integer range 10 downto 0;
  begin  -- process
    a := 3;
    if (a /= 3) then
      report "a = " & integer'image(a) &
        ", but expected = 3"
        severity failure;
    end if;
    if (a'high /= 20) then
      report "a'high = " & integer'image(a'high) &
        ", but expected = 20"
        severity failure;
    end if;   
    if (a'low /= -4) then
      report "a'low = " & integer'image(a'low) &
        ", but expected = -4"
        severity failure;
    end if;   
    if (a'left /= -4) then
      report "a'left = " & integer'image(a'left) &
        ", but expected = -4"
        severity failure;
    end if;   
    if (a'right /= 20) then
      report "a'right = " & integer'image(a'right) &
        ", but expected = 20"
        severity failure;
    end if;   
    if (b'high /= 10) then
      report "b'high = " & integer'image(b'high) &
        ", but expected = 10"
        severity failure;
    end if;   
    if (b'low /= 0) then
      report "b'low = " & integer'image(b'low) &
        ", but expected = 0"
        severity failure;
    end if;   
    if (b'left /= 10) then
      report "b'left = " & integer'image(b'left) &
        ", but expected = 10"
        severity failure;
    end if;   
    if (b'right /= 0) then
      report "b'right = " & integer'image(b'right) &
        ", but expected = 0"
        severity failure;
    end if;   
    finish(0);
  end process;

end architecture rtl;
