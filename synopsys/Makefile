SOURCE := $(wildcard *.cpp)
VHDL_SOURCE := $(wildcard *.vhd)
VHDL_HPP := $(VHDL_SOURCE:.vhd=.hpp)
VHDL_CPP := $(VHDL_SOURCE:.vhd=.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
MAX_ERRORS := 5
CXXFLAGS := -std=c++14 -g -MMD -fmax-errors=$(MAX_ERRORS) # -Wall -ansi

all: $(VHDL_HPP)

$(VHDL_HPP): $(VHDL_SOURCE) ../src/hdlc
	../src/hdlc.sh -f $< -l synopsys -s 

%.o: %.cpp $(VHDL_HPP) 
	g++ $(CXXFLAGS) -c $< -o $@ -I../src/kernel/lib 

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm $(VHDL_HPP) $(VHDL_CPP)

print-%:
	@echo '$*=$($*)'
