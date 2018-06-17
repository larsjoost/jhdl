SOURCE := $(wildcard *.cpp)
VHDL_SOURCE := $(wildcard *.vhd)
VHDL_HPP := $(VHDL_SOURCE:.vhd=.hpp)
VHDL_CPP := $(VHDL_SOURCE:.vhd=.cpp)
VHDL_INC := $(VHDL_SOURCE:.vhd=.i)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
MAX_ERRORS := 5
CXXFLAGS := -std=c++14 -g -MMD -fmax-errors=$(MAX_ERRORS) # -Wall -ansi

all: $(OBJS)

hpp: $(VHDL_HPP)

%.hpp .jhdl.ini : %.vhd ../src/hdlc
	../src/hdlc.sh -f $< -l ieee -s 

%.o: %.cpp %.hpp
	g++ $(CXXFLAGS) -c $< -o $@ -I../src/kernel/lib $(VHDL_INCLUDES)

-include $(VHDL_INC)

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm $(VHDL_HPP) $(VHDL_CPP)

print-%:
	@echo '$*=$($*)'
