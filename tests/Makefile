.PHONY: test clean

test: test_translator
	./test_translator

test_translator: test_translator.o
	cc -o test_translator test_translator.o ../src/translator.o -lcheck

test_translator.o:

clean:
	rm -rf test_translator *.o 
