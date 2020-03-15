JHDL := $(shell pwd)/../..
include $(JHDL)/include.mak
SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
MAX_ERRORS := 5
CXXFLAGS := -std=c++14 -g -MMD -fconcepts -fmax-errors=$(MAX_ERRORS) # -Wall -ansi
INC_DIR := -I$(SYSTEMC)/include -I. -I$(JHDL)/src/kernel/lib -I$(JHDL)/src/output -I$(JHDL)/src/debug -I$(JHDL)/std
LIBS := -llibsystemc
INC := -L$(SYSTEMC)/lib-linux64

all: $(OBJS)

debug_debug.hpp: debug.vhd
	$(JHDL)/src/hdlc.sh -f debug.vhd -l debug -s -p $(VERBOSE) $(GDB)

%.o: %.cpp debug_debug.hpp
	g++ $(CXXFLAGS) $(INC_DIR) $(INC) $(LIBS) -c $< -o $@ 

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm debug_debug.hpp debug.cpp

print-%:
	@echo '$*=$($*)'
