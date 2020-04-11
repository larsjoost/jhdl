
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

  type char1_t is ('a', 'b');

  type char2_t is ('a', 'c');

  type char_array_t is array(char1_t, char2_t) of char1_t;

  constant c_char_array : char_array_t := (
    ('a', 'b'),
    ('b', 'a')
    );
  
begin


  process is
  begin  -- process
    report "#1" severity note;
    if (c_char_array('a', 'c') /= 'b') then
      report "c_char_array('a', 'c') = " & char1_t'image(c_char_array('a', 'c')) &
        ", but expected = " & char1_t'image('c')
        severity failure;
    end if;
    report "#2" severity note;
    for i in enum_t loop
      for j in enum_t loop
        report "C_D(" & enum_t'image(i) & ", " & enum_t'image(j) & ") = " & integer'image(C_D(i, j)) severity note; 
      end loop;
    end loop;
    if (C_D(SECOND, SECOND) /= 4) then
      report "C_D(SECOND, SECOND) = " & integer'image(C_D(SECOND, SECOND)) & ", but expected 4" severity failure;
    else
      report "C_D(SECOND, SECOND) success" severity note;
    end if;   
    if (C_D(SECOND, FIRST) /= 3) then
      report "C_D(SECOND, FIRST) = " & integer'image(C_D(SECOND, FIRST)) & ", but expected 3" severity failure;
    else
      report "C_D(SECOND, FIRST) success" severity note;
    end if;   
    if (C_D(FIRST, SECOND) /= 2) then
      report "C_D(FIRST, SECOND) = " & integer'image(C_D(FIRST, SECOND)) & ", but expected 2" severity failure;
    else
      report "C_D(FIRST, SECOND) success" severity note;
    end if;   
    if (C_D(FIRST, FIRST) /= 1) then
      report "C_D(FIRST, FIRST) = " & integer'image(C_D(FIRST, FIRST)) & ", but expected 1" severity failure;
    else
      report "C_D(FIRST, FIRST) success" severity note;
    end if;   
    finish(0);      
  end process;

end architecture rtl;
