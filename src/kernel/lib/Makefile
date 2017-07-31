SOURCE := $(wildcard *.cpp)
OBJS := $(SOURCE:.cpp=.o)
DEPS := $(OBJS:.o=.d)
CXXFLAGS := -std=c++14 -g -MMD -Wall -fopenmp # -ansi

all: $(OBJS)

%.o: %.cpp 
	g++ $(CXXFLAGS) -c $< -o $@  

-include $(DEPS)

clean:
	find . -name "*.o" -type f -delete

print-%:
	@echo '$*=$($*)'
