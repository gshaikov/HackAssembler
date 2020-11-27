#ifndef STRING_TABLE_H
#define STRING_TABLE_H

#include "errors.h"

#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 256
#define HASH_TABLE_SIZE 256

typedef struct StrEntry
{
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} StrEntry;

typedef struct StrNode
{
    StrEntry s;
    struct StrNode *next;
} StrNode;

StrNode *strnode_new(StrEntry s);

typedef StrNode **StrTable;

StrTable strtable_new(void);
void strtable_destroy(StrTable st);
Error strtable_add(StrTable st, StrEntry s);
StrEntry *strtable_view_by_name(StrTable st, const char *symbol_name);

unsigned int _strtable_hash(const char *word, unsigned int max);

Error _strtable_store_in_table(StrTable st, unsigned int idx, StrNode *n);
StrEntry *_strtable_find_node_in_list(StrNode *n, const char *name);

#endif /* STRING_TABLE_H */
