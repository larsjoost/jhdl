CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 
BASEDIR = ../..
SRCDIR = $(BASEDIR)/src
CPP_FILES := $(wildcard $(SRCDIR)/*/*.cpp) $(SRCDIR)/jhdl.cpp
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

LIBS =

TARGET =	jhdl.exe

$(TARGET):	$(OBJ_FILES)
	$(CXX) -o $(TARGET) $(OBJ_FILES) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
