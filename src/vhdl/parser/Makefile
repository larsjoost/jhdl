SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(wildcard *.c*)
CXXFLAGS := -std=c++14 -g # -Wall -ansi

%.o: %.cpp $(DEPS)
	g++ $(CXXFLAGS) -c $< -o $@  

all: $(OBJS)

clean:
	find . -name "*.o" -type f -delete
