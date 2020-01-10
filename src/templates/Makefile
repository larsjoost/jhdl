# Copy from src/templates/Makefile
SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -fconcepts -MMD -O0 # -Wall -ansi
INC_DIR := -I$(SYSTEMC)/include -I${JHDL}/src/kernel/lib -I${JHDL}/std 
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
	rm test.hpp
	rm test.cpp
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete

print-%:
	@echo '$*=$($*)'
