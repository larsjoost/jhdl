# Copy from $(JHDL)/src/templates/Makefile
ifndef JHDL
$(error JHDL is not defined)
endif
HEADER_FILE := work_test.hpp
BUILD_DIR := build
EXECUTABLE := $(BUILD_DIR)/test
VHDL_INCLUDE_FILES := $(shell cat test.i)
VHDL_INCLUDES := $(addprefix -I, $(VHDL_INCLUDE_FILES))

all: $(HEADER_FILE)

$(HEADER_FILE): test.vhdl
	${JHDL}/src/hdlc.sh -f test.vhdl

$(EXECUTABLE): $(HEADER_FILE)
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); cmake ../

simulate: $(EXECUTABLE)
	${JHDL}/src/hdls -f test.vhdl -d run.do 

-include $(DEPS)

clean:
	rm work_test.hpp
	rm test.cpp
	rm sc_main.cpp
	find . -name "*.o" -type f -delete
	find . -name "*.d" -type f -delete

print-%:
	@echo '$*=$($*)'
