#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "string_table.h"

/**
 * Create a new node for hash table.
 *
 * Returns: pointer to the StrNode if successfully created;
 *          NULL otherwise.
 * */
StrNode *strnode_new(StrEntry s)
{
    StrNode *n = malloc(sizeof(StrNode));
    if (n == NULL)
    {
        return NULL;
    }
    n->s = s;
    n->next = NULL;
    return n;
}

/**
 * Create a new string table.
 * 
 * Must call `strtable_destroy` once the table is not needed
 * to free the memory.
 * 
 * Internally implemented as a hash table with linked lists.
 * 
 * Returns: pointer to the StrTable if successfully created;
 *          NULL otherwise.
 * */
StrTable strtable_new(void)
{
    StrTable table = calloc(HASH_TABLE_SIZE, sizeof(StrNode *));
    return table;
}

void _node_destroy(StrNode *n)
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
void strtable_destroy(StrTable st)
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

Error _store_in_table(StrTable st, unsigned int idx, StrNode *n)
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
 * Add StrEntry to the table.
 * Return 0 on success, 1 otherwise.
 * */
Error strtable_add(StrTable st, StrEntry s)
{
    StrNode *n = strnode_new(s);
    if (n == NULL)
    {
        return ERROR;
    }
    return _store_in_table(st, _hash(s.key, HASH_TABLE_SIZE), n);
}

StrEntry *_find_node_in_list(StrNode *n, const char *name)
{
    if (n != NULL)
    {
        if (strcmp(n->s.key, name) == 0)
        {
            return &n->s;
        }
        return _find_node_in_list(n->next, name);
    }
    return NULL;
}

StrEntry *strtable_view_by_name(StrTable st, const char *symbol_name)
{
    unsigned int idx = _hash(symbol_name, HASH_TABLE_SIZE);
    return _find_node_in_list(st[idx], symbol_name);
}
