#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <stdio.h>

#include "symbol_table.h"
#include "string_table.h"

int translate(FILE *binfile, FILE *asmfile);

Error _load_symbols_array(SymbolTable st, int arr_size, Symbol *arr);
Error _skip_until_newline(FILE *file);
Error _copy_statements_only(FILE *to, FILE *from);
Error _load_labels(SymbolTable st, FILE *from);
Error _assemble_binary(FILE *to, FILE *from, SymbolTable st, StrTable comp, StrTable dest, StrTable jump);

Error _translate_A_instruction(char *bin, char *buf, SymbolTable st, int *p_next_free_address);
Error _translate_C_instruction(char *bin, char *buf, StrTable comp, StrTable dest, StrTable jump);

#endif /* TRANSLATOR_H */
