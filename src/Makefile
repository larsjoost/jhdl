all:
	make -C version
	mkdir -p build
	cd build; cmake ../; make

print-%:
	@echo '$*=$($*)'

