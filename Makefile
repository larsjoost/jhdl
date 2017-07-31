DIRS := src std

.PHONY: all test clean 

all:
	for m in $(DIRS); do \
	$(MAKE) -C $$m; \
	done

test:
	cd testsuite; ./test.sh

clean:
	for m in $(DIRS); do \
	$(MAKE) -C $$m clean; \
	done
