SOURCE := $(shell find ./ -type f -iname '*.cpp' -print)
INCLUDE := $(shell find ./ -type f -iname '*.hpp' -print)
CXXFLAGS := -std=c++11 -Wno-write-strings -g
# CXX = g++

# OBJ := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

OBJ=jhdl.o

all: jhdl

jhdl: $(SOURCE) $(INCLUDE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(FLAGS)


print-%:
	@echo '$*=$($*)'

.PHONY: clean

clean:
	rm jhdl
	find . -name "*.o" -type f -delete

