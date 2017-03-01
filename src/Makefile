DIRS := ast parser generator vhdl verilog
MAKEFILES := $(shell find $(DIRS) -type f -iname 'Makefile' -print)
SUBDIRS := $(patsubst %/Makefile, %, $(MAKEFILES))
SOURCE := $(shell find $(DIRS) -type f -iname '*.cpp' -print)
INCLUDE := $(shell find $(DIRS) -type f -iname '*.hpp' -print)
CXXFLAGS := -std=c++14 -g # -Wall -ansi
# CXX = g++

# OBJ := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRC)))

OBJ=hdlc.o hdlf.o

all: subdirs hdlc hdlf

subdirs:
	for m in $(SUBDIRS); do \
	$(MAKE) -C $$m; \
	done

hdlc: $(SOURCE) $(INCLUDE)
	$(CXX) $(CXXFLAGS) -o $@ $(shell find $(DIRS) -type f -iname '*.o' -print) hdlc.cpp $(FLAGS)

hdlf: $(SOURCE) $(INCLUDE)
	$(CXX) $(CXXFLAGS) -o $@ $(shell find $(DIRS) -type f -iname '*.o' -print) hdlf.cpp $(FLAGS)

cppcheck:
	cppcheck --quiet --enable=all ./

print-%:
	@echo '$*=$($*)'

.PHONY: clean

clean:
	rm hdlc hdlf
	find . -name "*.o" -type f -delete

