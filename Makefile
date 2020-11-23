dirs = src

.PHONY: all build clean

all: build

build:
	$(MAKE) -C $(dirs)

clean:
	$(MAKE) -C $(dirs) clean
