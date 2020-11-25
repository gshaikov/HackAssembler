vpath %.o ../src

.PHONY: test clean

run_test_suites = \
test_symbol_table

test: $(run_test_suites)

$(run_test_suites): test_%: test_%.o %.o
	cc -o $@ $^ -lcheck
	./$@

test_%.o:

clean:
	rm -rf $(run_test_suites) *.o
