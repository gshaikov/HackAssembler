# Hack Assembler Project

An artisan C implementation of the Hack Assembler for the [nand2tetris](https://www.nand2tetris.org/) course.

A custom hash table was implemented to store symbols and instructions. Also, Go-like error handling is employed, albeit with much less power.

## Tutorial

### Build

``` bash
make
```

The binary file `hack_assembler` will appear in the root directory.

### Test

``` bash
make test
```

Tests are implemented with [check](https://libcheck.github.io/check/doc/check_html/index.html#Top) unit test framework.

### Use

``` bash
./hack_assembler <asmfile>
```

The output binary machine code will be stored in `out.hack` file in the root.

The example Hack assembly code is provided in `add` , `max` , `rect` , and `pong` directories.
