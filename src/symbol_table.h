#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "errors.h"

#define MAX_SYMBOL_LENGTH 32
#define HASH_TABLE_SIZE 256

/* Symbol repr. a symbolic refrence to memory address or instruction number */
typedef struct Symbol
{
    char name[MAX_SYMBOL_LENGTH];
    unsigned int value;
} Symbol;

/* HashNode container for Symbol is a unit in a linked list */
typedef struct HashNode
{
    Symbol s;
    struct HashNode *next;
} HashNode;

HashNode *hashnode_new(Symbol s);

/* SymbolTable is an array of pointers to HashNodes of the size HASH_TABLE_SIZE */
typedef HashNode** SymbolTable;

SymbolTable symtable_new(void);
void symtable_destroy(SymbolTable st);
Error symtable_add(SymbolTable st, Symbol s);
Symbol *symtable_view_by_name(SymbolTable st, const char *symbol_name);

unsigned int _hash(const char *word, unsigned int max);

Error _store_in_table(SymbolTable st, unsigned int idx, HashNode *n);
Symbol *_find_node_in_list(HashNode *n, const char *name);

#endif /* SYMBOLS_H */
