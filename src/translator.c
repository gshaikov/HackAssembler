#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "errors.h"
#include "symbol_table.h"
#include "string_table.h"

#define MAX_STATEMENT_LENGTH 256
#define BINARY_WORD_LENGTH 16
#define FIRST_FREE_ADDRESS 16

#define PREDEFINED_SYMBOLS_NUMBER 23
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

#define COMP_NUMBER 28
StrEntry COMP[COMP_NUMBER] = {
    {.key = "0", .value = "0101010"},
    {.key = "1", .value = "0111111"},
    {.key = "-1", .value = "0111010"},
    {.key = "D", .value = "0001100"},
    {.key = "A", .value = "0110000"},
    {.key = "M", .value = "1110000"},
    {.key = "!D", .value = "0001101"},
    {.key = "!A", .value = "0110001"},
    {.key = "!M", .value = "1110001"},
    {.key = "-D", .value = "0001111"},
    {.key = "-A", .value = "0110011"},
    {.key = "-M", .value = "1110011"},
    {.key = "D+1", .value = "0011111"},
    {.key = "A+1", .value = "0110111"},
    {.key = "M+1", .value = "1110111"},
    {.key = "D-1", .value = "0001110"},
    {.key = "A-1", .value = "0110010"},
    {.key = "M-1", .value = "1110010"},
    {.key = "D+A", .value = "0000010"},
    {.key = "D+M", .value = "1000010"},
    {.key = "D-A", .value = "0010011"},
    {.key = "D-M", .value = "1010011"},
    {.key = "A-D", .value = "0000111"},
    {.key = "M-D", .value = "1000111"},
    {.key = "D&A", .value = "0000000"},
    {.key = "D&M", .value = "1000000"},
    {.key = "D|A", .value = "0010101"},
    {.key = "D|M", .value = "1010101"},
};

#define DEST_NUMBER 8
StrEntry DEST[DEST_NUMBER] = {
    {.key = "", .value = "000"},
    {.key = "M", .value = "001"},
    {.key = "D", .value = "010"},
    {.key = "MD", .value = "011"},
    {.key = "A", .value = "100"},
    {.key = "AM", .value = "101"},
    {.key = "AD", .value = "110"},
    {.key = "AMD", .value = "111"},
};

#define JUMP_NUMBER 8
StrEntry JUMP[JUMP_NUMBER] = {
    {.key = "", .value = "000"},
    {.key = "JGT", .value = "001"},
    {.key = "JEQ", .value = "010"},
    {.key = "JGE", .value = "011"},
    {.key = "JLT", .value = "100"},
    {.key = "JNE", .value = "101"},
    {.key = "JLE", .value = "110"},
    {.key = "JMP", .value = "111"},
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

// TODO: auto generate (macros?)
Error _load_string_entry_array(StrTable st, int arr_size, StrEntry *arr)
{
    Error err;
    for (int i = 0; i < arr_size; i++)
    {
        err = strtable_add(st, arr[i]);
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

Error _store_new_address_variable(SymbolTable st, char *buf, int free_address)
{
    Symbol s;
    strcpy(s.name, buf);
    s.value = free_address;
    return symtable_add(st, s);
}

void _convert_integer_to_bit_string(char *bin, int size, int value)
{
    bin[size - 1] = '\0';
    for (int i = size - 2; i >= 0; i--)
    {
        if (value == 0)
            bin[i] = '0';
        else
        {
            bin[i] = (value % 2 == 0) ? '0' : '1';
            value /= 2;
        }
    }
}

Error _translate_A_instruction(char *bin, char *buf, SymbolTable st, int *p_next_free_address)
{
    Error err;
    int value;
    int n = sscanf(&buf[1], "%d", &value);
    if (n != 1)
    {
        Symbol *p_s = symtable_view_by_name(st, &buf[1]);
        if (p_s == NULL)
        {
            value = *p_next_free_address;
            (*p_next_free_address)++;
            err = _store_new_address_variable(st, &buf[1], value);
            if (err)
                return err;
        }
        else
            value = p_s->value;
    }
    _convert_integer_to_bit_string(bin, BINARY_WORD_LENGTH + 1, value);
    return SUCCESS;
}

Error _translate_C_instruction(char *bin, char *buf, StrTable comp, StrTable dest, StrTable jump)
{
    char *p_comp = buf;
    char *p_dest = "";
    char *p_jump = "";

    for (int i = 0; buf[i] != '\0'; i++)
    {
        switch (buf[i])
        {
        case '=':
            p_comp = &buf[i + 1];
            p_dest = buf;
            buf[i] = '\0';
            break;
        case ';':
            p_jump = &buf[i + 1];
            buf[i] = '\0';
            break;
        }
    }

    StrEntry *s_comp = strtable_view_by_name(comp, p_comp);
    if (s_comp == NULL)
        return ERROR;
    StrEntry *s_dest = strtable_view_by_name(dest, p_dest);
    if (s_dest == NULL)
        return ERROR;
    StrEntry *s_jump = strtable_view_by_name(jump, p_jump);
    if (s_jump == NULL)
        return ERROR;

    int n = sprintf(bin, "111%s%s%s", s_comp->value, s_dest->value, s_jump->value);
    return (n != BINARY_WORD_LENGTH) ? ERROR : SUCCESS;
}

// TODO: use struct for repositories
Error _assemble_binary(FILE *to, FILE *from, SymbolTable st, StrTable comp, StrTable dest, StrTable jump)
{
    Error err;
    char buf[MAX_STATEMENT_LENGTH];
    char bin[BINARY_WORD_LENGTH + 1];
    int next_free_address = FIRST_FREE_ADDRESS;
    int *p_next_free_address = &next_free_address;
    while (fscanf(from, "%s", buf) == 1)
    {
        switch (buf[0])
        {
        case '@':
            err = _translate_A_instruction(bin, buf, st, p_next_free_address);
            if (err)
                return err;
            fputs(bin, to);
            fputc('\n', to);
            break;
        case '(':
            err = _skip_until_newline(from);
            if (err)
                return err;
            break;
        default:
            err = _translate_C_instruction(bin, buf, comp, dest, jump);
            if (err)
                return err;
            fputs(bin, to);
            fputc('\n', to);
            break;
        }
    }
    return (ferror(from)) ? ERROR : SUCCESS;
}

// TODO: wrap all destroy and fclose
Error translate(FILE *binfile, FILE *asmfile)
{
    Error err;

    SymbolTable table = symtable_new();
    err = _load_symbols_array(table, PREDEFINED_SYMBOLS_NUMBER, PREDEFINED_SYMBOLS);
    if (err)
    {
        symtable_destroy(table);
        return err;
    }

    StrTable comp = strtable_new();
    err = _load_string_entry_array(comp, COMP_NUMBER, COMP);
    if (err)
    {
        symtable_destroy(table);
        strtable_destroy(comp);
        return err;
    }

    StrTable dest = strtable_new();
    err = _load_string_entry_array(dest, DEST_NUMBER, DEST);
    if (err)
    {
        symtable_destroy(table);
        strtable_destroy(comp);
        strtable_destroy(dest);
        return err;
    }

    StrTable jump = strtable_new();
    err = _load_string_entry_array(jump, JUMP_NUMBER, JUMP);
    if (err)
    {
        symtable_destroy(table);
        strtable_destroy(comp);
        strtable_destroy(dest);
        strtable_destroy(jump);
        return err;
    }

    FILE *tmp_asmfile = tmpfile();

    err = _copy_statements_only(tmp_asmfile, asmfile);
    if (err)
    {
        fclose(tmp_asmfile);
        symtable_destroy(table);
        strtable_destroy(comp);
        strtable_destroy(dest);
        strtable_destroy(jump);
        return err;
    }
    rewind(tmp_asmfile);

    err = _load_labels(table, tmp_asmfile);
    if (err)
    {
        fclose(tmp_asmfile);
        symtable_destroy(table);
        strtable_destroy(comp);
        strtable_destroy(dest);
        strtable_destroy(jump);
        return err;
    }
    rewind(tmp_asmfile);

    err = _assemble_binary(binfile, tmp_asmfile, table, comp, dest, jump);
    {
        fclose(tmp_asmfile);
        symtable_destroy(table);
        strtable_destroy(comp);
        strtable_destroy(dest);
        strtable_destroy(jump);
        return err;
    }

    fclose(tmp_asmfile);
    symtable_destroy(table);
    strtable_destroy(comp);
    strtable_destroy(dest);
    strtable_destroy(jump);
    return SUCCESS;
}
