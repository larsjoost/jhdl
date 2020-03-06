JHDL := $(shell pwd)/..
include $(JHDL)/include.mak
SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
MAX_ERRORS := 5
CXXFLAGS := -std=c++14 -g -MMD -fconcepts -fmax-errors=$(MAX_ERRORS) # -Wall -ansi
INC_DIR := -I$(SYSTEMC)/include -I. -I$(JHDL)/src/kernel/lib -I$(JHDL)/src/output -I$(JHDL)/src/debug
LIBS := -lsystemc
INC := -L$(SYSTEMC)/lib-linux64

all: $(OBJS)

standard.hpp: standard.vhd ../src/hdlc
	../src/hdlc.sh -f standard.vhd -l std -s -p $(VERBOSE) $(GDB)

env.hpp: env.vhd ../src/hdlc
	../src/hdlc.sh -f env.vhd -l std -s $(VERBOSE) $(GDB)

textio.hpp: textio.vhd ../src/hdlc
	../src/hdlc.sh -f textio.vhd -l std -s

%.o: %.cpp standard.hpp env.hpp textio.hpp
	g++ $(CXXFLAGS) $(INC_DIR) $(INC) $(LIBS) -c $< -o $@ -I../src/kernel/lib 

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm standard.hpp standard.cpp env.hpp env.cpp textio.hpp textio.cpp

print-%:
	@echo '$*=$($*)'
