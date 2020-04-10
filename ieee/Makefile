SOURCE := $(wildcard *.cpp)
VHDL_SOURCE := std_logic_1164.vhd
VHDL_HPP := $(addprefix ieee_, $(VHDL_SOURCE:.vhd=.hpp))
VHDL_CPP := $(VHDL_SOURCE:.vhd=.cpp)
VHDL_INC := $(VHDL_SOURCE:.vhd=.i)
VHDL_INCLUDE_FILES := $(shell cat $(VHDL_INC))
VHDL_INCLUDES := $(addprefix -I, $(VHDL_INCLUDE_FILES))
OBJS := $(SOURCE:.cpp=.o) 
DEPS := $(OBJS:.o=.d)
MAX_ERRORS := 5
CXXFLAGS := -std=c++14 -g -MMD -fconcepts -fmax-errors=$(MAX_ERRORS) # -Wall -ansi

all: hpp $(VHDL_CPP) $(OBJS)

hpp: $(VHDL_HPP)

ieee_%.hpp: %.vhd ../src/hdlc
	../src/hdlc.sh -f $< -l ieee -s 

%.o: %.cpp ieee_%.hpp
	g++ $(CXXFLAGS) -c $< -o $@ -I../src/kernel/lib $(VHDL_INCLUDES)

-include $(VHDL_INC)

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm $(VHDL_HPP) $(VHDL_CPP)

print-%:
	@echo '$*=$($*)'
