library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity biosram is
port (
		 clka  : IN STD_LOGIC;
		 ena   : IN STD_LOGIC;
		 wea   : IN STD_LOGIC;
		 addra : IN STD_LOGIC_VECTOR(13 DOWNTO 0);
		 dina  : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		 douta : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
     );
end biosram;

architecture Behavioral of biosram is

	--Declaration of type and signal of a 512 element RAM
	--with each element being 8 bit wide.
	type ram_t is array (0 to 74) of std_logic_vector(7 downto 0);
	signal ram : ram_t := ("00000000","00000010","00000110","00000000","00000010","11111111","11111111","00000000","00000010","00000100","00000000","00000000","00000000","00011110","01111100","00000000","00000000","00000000","00101100","00000000","00000010","00000010","11111111","11111111","11111111","11111111","01101100","00000000","00000000","00000000","00010100","11111111","00000000","00000000","00000000","00000000","01001000","01100101","01101100","01101100","01101111","00100000","01110111","01101111","01110010","01101100","01100100","00100001","00100001","00000000","10101000","10101100","10000101","01111110","10000111","11101000","00000000","00000000","00000000","01000000","00101000","00000010","00000000","00000000","00000000","00000000","00000000","11111111","00000000","00000000","00000000","00101111","00101100","00101000","00101111");
	signal data_o : std_logic_vector(7 downto 0);
	signal adr    : std_logic_vector(5 downto 0);
begin

	--process for read and write operation.
	ram(to_integer(unsigned(addra))) <= dina when wea='1' and ena='1';
	douta <= ram(to_integer(unsigned(addra))) when wea='0' and ena='1';

end Behavioral;