vpath %.o ../src

.PHONY: test clean

run_test_suites = \
test_symbol_table \
test_translator

# Variables can be assigned for specific targets
test_translator: dependencies = \
../src/symbol_table.o

test: $(run_test_suites)

$(run_test_suites): test_%: test_%.o %.o
	cc -o $@ $^ $(dependencies) -lcheck
	./$@

test_%.o:

clean:
	rm -rf $(run_test_suites) *.o
