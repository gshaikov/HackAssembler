srcDirs = src
testDirs = tests

.PHONY: build test clean

build:
	$(MAKE) -C $(srcDirs)

test: build
	$(MAKE) -C $(testDirs) test

clean:
	$(MAKE) -C $(srcDirs) clean
	$(MAKE) -C $(testDirs) clean
