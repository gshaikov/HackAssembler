.PHONY: build test clean

srcDirs = src
testDirs = tests

build:
	$(MAKE) -C $(srcDirs)

test: build
	$(MAKE) -C $(testDirs) test

clean:
	$(MAKE) -C $(srcDirs) clean
	$(MAKE) -C $(testDirs) clean
