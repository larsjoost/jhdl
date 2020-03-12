JHDL := $(shell pwd)/..
DIRS := debug

.PHONY: all test clean 

all:
	for m in $(DIRS); do \
	$(MAKE) JHDL=$(JHDL) -C $$m; \
	done

clean:
	for m in $(DIRS); do \
	$(MAKE) -C $$m clean; \
	done
