entity test is
  generic (
    G_TEST_1 : integer := 2;
    G_TEST_2 : integer := 4);
  port (
    clk : in  bit;
    a   : in  integer;
    q   : out integer);
end entity test; 

