#include <stdio.h>
#include <stdbool.h>

#include "errors.h"
#include "symbol_table.h"

#define PREDEFINED_SYMBOLS_NUMBER 23
#define MAX_STATEMENT_LENGTH 256

Symbol PREDEFINED_SYMBOLS[PREDEFINED_SYMBOLS_NUMBER] = {
    {.name = "R0", .value = 0},
    {.name = "R1", .value = 1},
    {.name = "R2", .value = 2},
    {.name = "R3", .value = 3},
    {.name = "R4", .value = 4},
    {.name = "R5", .value = 5},
    {.name = "R6", .value = 6},
    {.name = "R7", .value = 7},
    {.name = "R8", .value = 8},
    {.name = "R9", .value = 9},
    {.name = "R10", .value = 10},
    {.name = "R11", .value = 11},
    {.name = "R12", .value = 12},
    {.name = "R13", .value = 13},
    {.name = "R14", .value = 14},
    {.name = "R15", .value = 15},
    {.name = "SCREEN", .value = 16384},
    {.name = "KBD", .value = 24576},
    {.name = "SP", .value = 0},
    {.name = "LCL", .value = 1},
    {.name = "ARG", .value = 2},
    {.name = "THIS", .value = 3},
    {.name = "THAT", .value = 4},
};

Error _load_symbols_array(SymbolTable st, int arr_size, Symbol *arr)
{
    Error err;
    for (int i = 0; i < arr_size; i++)
    {
        err = symtable_add(st, arr[i]);
        if (err)
            return err;
    }
    return SUCCESS;
}

/* Advance file cursor until the next new line */
Error _skip_until_newline(FILE *file)
{
    int c;
    while ((c = fgetc(file)) != EOF)
        if (c == '\n')
            return SUCCESS;
    return (ferror(file)) ? ERROR : SUCCESS;
}

bool _is_comment(char buf[2])
{
    return (buf[0] == '/' && buf[1] == '/');
}

Error _copy_statements_only(FILE *to, FILE *from)
{
    Error err;
    char buf[256];
    while (fscanf(from, "%s", buf) != EOF)
    {
        if (!_is_comment(buf))
        {
            fputs(buf, to);
            fputc('\n', to);
        }
        err = _skip_until_newline(from);
        if (err)
            return ERROR;
    }
    return (ferror(from)) ? ERROR : SUCCESS;
}

Error _read_label(char *buf, FILE *file)
{
    return (fscanf(file, "%[^()]", buf) == EOF) ? ERROR : SUCCESS;
}

Error _load_labels(SymbolTable st, FILE *from)
{
    int instruction_number = 0;
    Error err;
    Symbol s;
    int c;
    while ((c = fgetc(from)) != EOF)
    {
        switch (c)
        {
        case '(':
            s.value = instruction_number;
            err = _read_label(s.name, from);
            if (err)
                return err;
            err = symtable_add(st, s);
            if (err)
                return err;
            err = _skip_until_newline(from);
            if (err)
                return err;
            break;
        default:
            instruction_number++;
            err = _skip_until_newline(from);
            if (err)
                return err;
            break;
        }
    }
    return (ferror(from)) ? ERROR : SUCCESS;
}

Error translate(FILE *binfile, FILE *asmfile)
{
    Error err;
    SymbolTable table = symtable_new();
    FILE *tmp_asmfile = tmpfile();

    err = _load_symbols_array(table, PREDEFINED_SYMBOLS_NUMBER, PREDEFINED_SYMBOLS);
    if (err)
    {
        fclose(tmp_asmfile);
        symtable_destroy(table);
        return err;
    }

    err = _copy_statements_only(binfile, asmfile);
    if (err)
    {
        fclose(tmp_asmfile);
        symtable_destroy(table);
        return err;
    }
    rewind(tmp_asmfile);

    err = _load_labels(table, tmp_asmfile);
    if (err)
    {
        fclose(tmp_asmfile);
        symtable_destroy(table);
        return err;
    }

    // walk asm file, translate to new file, populate DS with @variables and @labels

    fclose(tmp_asmfile);
    symtable_destroy(table);
    return SUCCESS;
}
