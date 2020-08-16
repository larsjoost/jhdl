# Copy from $(JHDL)/templates/Makefile
ifndef JHDL
$(error JHDL is not defined)
endif
ifndef LIBRARY
$(error LIBRARY is not defined)
endif
VHD_SOURCE := $(wildcard *.vhd)
VHD_HPP := $(VHD_SOURCE:.vhd=.hpp)
VHD_OBJECTS := $(addprefix $(LIBRARY)_,$(VHD_HPP))
VHDL_SOURCE := $(wildcard *.vhdl)
VHDL_HPP := $(VHDL_SOURCE:.vhdl=.hpp)
VHDL_OBJECTS := $(addprefix $(LIBRARY)_,$(VHDL_HPP))

all: $(VHD_OBJECTS) $(VHDL_OBJECTS)

$(LIBRARY)_%.hpp: %.vhd
	hdlc $<

$(LIBRARY)_%.hpp: %.vhdl
	hdlc $<

print-%:
	@echo '$*=$($*)'
