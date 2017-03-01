SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS_DIRS := ./ ../../ast
DEPS := $(shell find $(DEPS_DIRS) -type f -iname '*.h*' -print)
CXXFLAGS := -std=c++14 -g # -Wall -ansi

%.o: %.cpp $(DEPS)
	g++ $(CXXFLAGS) -c $< -o $@  

all: $(OBJS)

clean:
	find . -name "*.o" -type f -delete
