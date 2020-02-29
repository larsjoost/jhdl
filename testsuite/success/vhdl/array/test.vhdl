
entity test is
end entity test;

library std;
use std.env.finish;

architecture rtl of test is

  type enum_t is (FIRST, SECOND);

  constant SIZE : integer := 5;
  
  type a_t is array (0 to SIZE - 1) of bit;

  type b_t is array (enum_t) of integer;

  constant C_SIZE : positive := 7;
  
  type c_t is array (enum_t) of string(1 to C_SIZE);
  
  constant C_B : b_t := (1, 2);

  type d_t is array(enum_t, enum_t) of integer;

  constant C_D : d_t := (
    (1, 2),
    (3, 4)
    );

  type e_t is array (natural range <>) of bit;

  type f_t is array (0 to SIZE - 1) of natural;
  
begin


  process is
--    variable a : bit_vector(0 to 5);
    variable a : a_t;
    variable c : c_t;
    variable f : f_t;
    variable e : e_t(0 to SIZE - 1);
  begin  -- process
    a := "00001";
    report "a := 00001" severity note;
    c(FIRST) := "Testing";
    report "c(FIRST) := Testing" severity note;
    wait for 10 ns;
    if (e'length /= SIZE) then
      report "e'length = " & integer'image(e'length) & ", but expected = " & integer'image(SIZE) severity failure;
    else
      report "e'length success" severity note;
    end if;   
    if (C_D(FIRST, SECOND) /= 2) then
      report "C_D(FIRST, SECOND) = " & integer'image(C_D(FIRST, SECOND)) & ", but expected 2" severity failure;
    else
      report "C_D(FIRST, SECOND) success" severity note;
    end if;   
    if (a(4) /= '1') then
      report "a = " & bit'image(a(4)) & ", but expected = '1'" severity failure;
    else
      report "a(4) success" severity note;
    end if;
    if (a(0) /= '0') then
      report "a failed" severity failure;
    end if;
    f := (1 => 1, 2 | 3 => 2, others => 3);
    if (f(3) /= 2) then
      report "f(3) = " & natural'image(f(3)) & ", but expected = 2" severity failure;
    end if;   
    if (C_B(FIRST) /= 1) then
      report "b(" & enum_t'image(FIRST) & ") = " & integer'image(C_B(FIRST)) &
        ", but expected 1" severity failure;
    end if;   
    if (C_B(SECOND) /= 2) then
      report "b(" & enum_t'image(SECOND) & ") = " & integer'image(C_B(SECOND)) &
        ", but expected 2" severity failure;
    end if;   
    --for i in a'range loop
    --  report "a = " & bit'image(i) severity note;
    --end loop;  -- i
    finish(0);
  end process;

end architecture rtl;
