SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -MMD # -Wall -ansi

%.o: %.cpp 
	g++ $(CXXFLAGS) -c $< -o $@  

-include $(DEPS)

all: $(OBJS)

clean:
	find . -name "*.o" -type f -delete
