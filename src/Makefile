.PHONY: clean

binary_name ?= hack_assembler

code_objects = \
hack_assembler.o \
translator.o \
symbol_table.o \
string_table.o

warnings = -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow

# $^ catches all pre-requisites
# Ref: https://makefiletutorial.com/
$(binary_name): $(code_objects)
	cc $(warnings) -o $@ $^

# Static pattern rule:
# 	targets ...: target-pattern: prereq-patterns ...
# $< catches first pre-requisite only
# Ref: https://makefiletutorial.com/
$(code_objects): %.o: %.c
	cc $(warnings) -c -o $@ $<

clean:
	rm -rf $(binary_name) $(code_objects)
