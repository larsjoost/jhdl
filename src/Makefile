SOURCE := $(shell find ./ -type f -iname '*.cpp' -print)
CXXFLAGS := -std=c++11 -Wno-write-strings -g
# CXX = g++

# OBJ := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

OBJ=jhdl.o

all: jhdl

jhdl: $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)


print-%:
	@echo '$*=$($*)'

.PHONY: clean

clean:
	rm jhdl
	find . -name "*.o" -type f -delete

