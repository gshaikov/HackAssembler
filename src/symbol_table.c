#include <stdlib.h>

#include "symbol_table.h"

/**
 * Create a new symbol table.
 * 
 * Must call `symtable_destroy` once the table is not needed
 * to free the memory.
 * 
 * Internally implemented as a hash table with linked lists.
 * 
 * Returns: pointer to the SymbolTable if successfully created;
 *          NULL otherwise.
 * */
SymbolTable symtable_new(void)
{
    SymbolTable table = calloc(HASH_TABLE_SIZE, sizeof(HashNode*));
    return table;
}

void _node_destroy(HashNode *n)
{
    if (n == NULL)
    {
        return;
    }
    _node_destroy(n->next);
    free(n);
    return;
}

/* Free the table and all the nodes */
void symtable_destroy(SymbolTable st)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        _node_destroy(st[i]);
    }
    free(st);
    return;
}

// polynomial rolling hash function
// https://cp-algorithms.com/string/string-hashing.html
unsigned int _hash(const char *word, unsigned int max)
{
    unsigned int code = 0;
    unsigned int pow = 1;
    for (int i = 0; word[i] != '\0'; i++)
    {
        code = (code + ((word[i] >= 'a' ? word[i] - 'a' : word[i] - 'A') + 1) * pow) % max;
        pow = (pow * 31) % max;
    }
    return code;
}
