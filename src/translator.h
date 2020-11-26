#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>

#include "symbol_table.h"

int translate(FILE *binfile, FILE *asmfile);

Error _load_symbols_array(SymbolTable st, int arr_size, Symbol *arr);
Error _skip_until_newline(FILE *file);
Error _copy_statements_only(FILE *to, FILE *from);
Error _load_labels(SymbolTable st, FILE *from);
Error _assemble_binary(FILE *to, FILE *from, SymbolTable st);

Error _translate_A_instruction(char *bin, char *buf);
Error _translate_C_instruction(char *bin, char *buf);

#endif /* TRANSLATOR_H */
