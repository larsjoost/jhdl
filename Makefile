DIRS := src std

all:
	for m in $(DIRS); do \
	$(MAKE) -C $$m; \
	done

clean:
	for m in $(DIRS); do \
	$(MAKE) -C $$m clean; \
	done
