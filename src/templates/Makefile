# Copy from $(JHDL)/src/templates/Makefile
ifndef JHDL
$(error JHDL is not defined)
endif
ifndef SYSTEMC
include $(JHDL)/systemc/include.mak
endif
SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -fconcepts -MMD -O3 # -Wall -ansi
INC_DIR := -I$(SYSTEMC)/include -I${JHDL}/src/kernel/lib -I${JHDL}/src/output -I${JHDL}/src/debug -I${JHDL}/std -I. 
LIBS := -lsystemc
INC := -L$(SYSTEMC)/lib-linux64

all: $(OBJS)

test.hpp: test.vhdl
	${JHDL}/src/hdlc.sh -f test.vhdl

-include test.i

%.o: %.cpp test.hpp
	g++ $(CXXFLAGS) -c $< -o $@ $(INC_DIR) $(LIBS) $(INC) $(VHDL_INCLUDES)  

simulate: %.o
	${JHDL}/src/hdls -f test.vhdl -d run.do 


-include $(DEPS)

clean:
	rm work_test.hpp
	rm test.cpp
	rm sc_main.cpp
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete

print-%:
	@echo '$*=$($*)'
