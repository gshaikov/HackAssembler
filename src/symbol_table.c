#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "symbol_table.h"

/**
 * Create a new node for hash table.
 *
 * Returns: pointer to the HashNode if successfully created;
 *          NULL otherwise.
 * */
HashNode *hashnode_new(Symbol s)
{
    HashNode *n = malloc(sizeof(HashNode));
    if (n == NULL)
    {
        return NULL;
    }
    n->s = s;
    n->next = NULL;
    return n;
}

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
    SymbolTable table = calloc(HASH_TABLE_SIZE, sizeof(HashNode *));
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

Error _store_in_table(SymbolTable st, unsigned int idx, HashNode *n)
{
    if (n == NULL)
    {
        return ERROR;
    }
    n->next = st[idx];
    st[idx] = n;
    return SUCCESS;
}

/**
 * Add Symbol to the table.
 * Return 0 on success, 1 otherwise.
 * */
Error symtable_add(SymbolTable st, Symbol s)
{
    HashNode *n = hashnode_new(s);
    if (n == NULL)
    {
        return ERROR;
    }
    return _store_in_table(st, _hash(s.name, HASH_TABLE_SIZE), n);
}

Symbol *_find_node_in_list(HashNode *n, const char *name)
{
    if (n != NULL)
    {
        if (strcmp(n->s.name, name) == 0)
        {
            return &n->s;
        }
        return _find_node_in_list(n->next, name);
    }
    return NULL;
}

Symbol *symtable_view_by_name(SymbolTable st, const char *symbol_name)
{
    unsigned int idx = _hash(symbol_name, HASH_TABLE_SIZE);
    return _find_node_in_list(st[idx], symbol_name);
}
