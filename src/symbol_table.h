#ifndef SYMBOLS_H
#define SYMBOLS_H

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

/* SymbolTable is an array of pointers to HashNodes of the size HASH_TABLE_SIZE */
typedef HashNode** SymbolTable;

SymbolTable symtable_new(void);
void symtable_destroy(SymbolTable st);

unsigned int _hash(const char *word, unsigned int max);

#endif /* SYMBOLS_H */
