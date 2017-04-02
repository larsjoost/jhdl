SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -MMD # -Wall -ansi

all: $(OBJS)

standard.hpp: standard.vhd
	../src/hdlc -f standard.vhd -l standard > standard.hpp

%.o: %.cpp standard.hpp
	g++ $(CXXFLAGS) -c $< -o $@ -I../src/kernel/lib 

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete
	rm standard.hpp

print-%:
	@echo '$*=$($*)'
