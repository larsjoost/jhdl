WORK_DIR := $(shell pwd)
SRC_DIR := $(shell echo $(WORK_DIR) | sed 's/test/src/g')
OBJ_DIRS := $(SRC_DIR) $(SRC_DIR)/../debug $(SRC_DIR)/../ast
SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
EXECUTABLES := $(SOURCE:.cpp=)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -fconcepts -MMD # -Wall -ansi

all: unit_parameters

%.o: %.cpp Makefile
	g++ $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

unit_parameters: unit_parameters.o 
	make -C $(OBJ_DIRS)
	g++ $< $(SRC_DIR)/parameters.o $(SRC_DIR)/../debug/debug.o $(SRC_DIR)/../output/output.o -o $@ 

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete

print-%:
	@echo '$*=$($*)'
