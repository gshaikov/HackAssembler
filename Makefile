.PHONY: build test clean

binary_name = hack_assembler

srcDir = src
testDir = tests

build:
	$(MAKE) -C $(srcDir) -B binary_name=$(binary_name)
	mv $(srcDir)/$(binary_name) .

test: build
	$(MAKE) -C $(testDir) -B test

clean:
	$(MAKE) -C $(srcDir) binary_name=$(binary_name) clean
	$(MAKE) -C $(testDir) clean
	rm -f $(binary_name)
