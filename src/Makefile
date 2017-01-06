SRC := jhdl.cpp
MODULES := ast vhdl
CXXFLAGS = -std=c++11 -Wno-write-strings
CXX := g++
-include $(patsubst %, %/module.mk, $(MODULES))

OBJ := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

all: jhdl

jhdl: $(OBJ)
	$(CXX) -o $@ $^ $(FLAGS)

print-%:
	@echo '$*=$($*)'

.PHONY: clean

clean:

