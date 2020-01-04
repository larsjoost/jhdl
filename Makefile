JHDL := $(shell pwd)
DIRS := systemc src std src/kernel/lib

.PHONY: all test clean 

all:
	for m in $(DIRS); do \
	$(MAKE) JHDL=$(JHDL) -C $$m; \
	done

test:
	cd testsuite; ./test.sh

clean:
	for m in $(DIRS); do \
	$(MAKE) -C $$m clean; \
	done
