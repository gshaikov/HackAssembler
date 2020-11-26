#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>

#include "symbol_table.h"

int translate(FILE *binfile, FILE *asmfile);

Error _load_symbols_array(SymbolTable st, int arr_size, Symbol *arr);
Error _skip_until_newline(FILE *file);
Error _copy_statements_only(FILE *to, FILE *from);
Error _load_labels(SymbolTable st, FILE *from);

#endif /* TRANSLATOR_H */
