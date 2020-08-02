# Copy from $(JHDL)/src/templates/Makefile
ifndef JHDL
$(error JHDL is not defined)
endif
# definitions.include must contain definition of LIBRARY variable
include definitions.include
SOURCE := $(wildcard *.vhd *.vhdl)
HEADER_FILE_LONG_EXTENSION := $(SOURCE:.vhdl=.hpp)
HEADER_FILE := $(HEADER_FILE_LONG_EXTENSION:.vhd=.hpp)
BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/test
VHDL_INCLUDE_FILES := $(shell cat test.i)
VHDL_INCLUDES := $(addprefix -I, $(VHDL_INCLUDE_FILES))

all: $(HEADER_FILE)

$(LIBRARY)_%.hpp: %.vhd?
	hdlc -f $<

$(BUILD_DIR)/%: %.cpp
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); cmake ../

simulate: $(BUILD_DIR)/
	hdls -d run.do $<

-include $(DEPS)

clean:
	rm work_test.hpp
	rm test.cpp
	rm sc_main.cpp
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete

print-%:
	@echo '$*=$($*)'
