SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
MAX_ERRORS := 5
CXXFLAGS := -std=c++14 -g -MMD -fmax-errors=$(MAX_ERRORS) # -Wall -ansi

all: $(OBJS)

std_logic_1164.hpp: std_logic_1164.vhd ../src/hdlc
	../src/hdlc -f std_logic_1164.vhd -l ieee -s 

%.o: %.cpp std_logic_1164.hpp 
	g++ $(CXXFLAGS) -c $< -o $@ -I../src/kernel/lib 

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm std_logic_1164.hpp std_logic_1164.cpp

print-%:
	@echo '$*=$($*)'
