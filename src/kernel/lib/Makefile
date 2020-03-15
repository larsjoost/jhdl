JHDL := $(shell pwd)/../../..
include $(JHDL)/include.mak
SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -fconcepts -MMD -O0 -Wall # -fopenmp # -ansi
INC := -L$(SYSTEMC)/lib64
INC_DIR := -I$(SYSTEMC)/include 

all: $(OBJS)

%.o: %.cpp 
	g++ $(CXXFLAGS) $(INC) $(INC_DIR) -c $< -o $@  

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete

print-%:
	@echo '$*=$($*)'
