.PHONY: build test test_integration test_memory clean

binary_name = hack_assembler

srcDir = src
testDir = tests

build:
	$(MAKE) -C $(srcDir) -B binary_name=$(binary_name)
	mv $(srcDir)/$(binary_name) .

test: build
	$(MAKE) -C $(testDir) -B test

test_integration: build
	./$(binary_name) examples/pong/Pong.asm
	diff out.hack examples/pong/Pong.hack
	rm out.hack

test_memory: build
	./scripts/docker_build.sh
	./scripts/docker_run.sh "make build && valgrind --leak-check=full ./$(binary_name) examples/pong/Pong.asm"
	rm out.hack

clean:
	$(MAKE) -C $(srcDir) binary_name=$(binary_name) clean
	$(MAKE) -C $(testDir) clean
	rm -f $(binary_name)
