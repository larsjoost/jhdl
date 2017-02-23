DIRS := ast parser generator vhdl verilog
SOURCE := $(shell find $(DIRS) -type f -iname '*.cpp' -print)
INCLUDE := $(shell find $(DIRS) -type f -iname '*.hpp' -print)
CXXFLAGS := -std=c++14 -g
# CXX = g++

# OBJ := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

OBJ=hdlc.o hdlf.o

all: hdlc hdlf

hdlc: $(SOURCE) $(INCLUDE)
	$(CXX) $(CXXFLAGS) -o $@ $^ hdlc.cpp $(FLAGS)

hdlf: $(SOURCE) $(INCLUDE)
	$(CXX) $(CXXFLAGS) -o $@ $^ hdlf.cpp $(FLAGS)

cppcheck:
	cppcheck --quiet --enable=all ./

print-%:
	@echo '$*=$($*)'

.PHONY: clean

clean:
	rm hdlc hdlf
	find . -name "*.o" -type f -delete

