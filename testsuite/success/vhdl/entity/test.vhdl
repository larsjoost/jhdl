entity test is
  port (
    a   : in  integer;
    b   : in  integer;
    q   : out integer);
end entity test; 

architecture rtl of test is

begin  

  q <= a + b;

end architecture rtl;
