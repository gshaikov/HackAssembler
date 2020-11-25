#include <stdlib.h>
#include <check.h>

#include "../src/symbol_table.h"

START_TEST(test_hashnode_new)
{
    Symbol s = {.name = "symbol_name", .value = 123};
    HashNode *n = hashnode_new(s);
    ck_assert_ptr_nonnull(n);
    ck_assert_mem_eq(&n->s, &s, sizeof(Symbol));
    ck_assert_ptr_null(n->next);
    free(n);
}
END_TEST

START_TEST(test_symtable_new)
{
    SymbolTable table = symtable_new();
    ck_assert_ptr_nonnull(table);
    ck_assert_ptr_null(table[0]);
    ck_assert_ptr_null(table[1]);
    ck_assert_ptr_null(table[HASH_TABLE_SIZE - 1]);
    symtable_destroy(table);
}
END_TEST

START_TEST(test_integration)
{
    SymbolTable table = symtable_new();
    ck_assert_ptr_nonnull(table);
    Symbol s1 = {.name = "s1", .value = 1};
    Symbol s2 = {.name = "s2", .value = 2};

    ck_assert(symtable_add(table, s1) == SUCCESS);
    ck_assert(symtable_add(table, s2) == SUCCESS);
    Symbol *ps1 = symtable_view_by_name(table, "s1");
    ck_assert_ptr_nonnull(ps1);
    ck_assert_mem_eq(ps1, &s1, sizeof(Symbol));
    Symbol *ps2 = symtable_view_by_name(table, "s2");
    ck_assert_ptr_nonnull(ps2);
    ck_assert_mem_eq(ps2, &s2, sizeof(Symbol));
    Symbol *p_null = symtable_view_by_name(table, "dont_exist");
    ck_assert_ptr_null(p_null);

    symtable_destroy(table);
}
END_TEST

START_TEST(test__hash)
{
    ck_assert_int_eq(_hash("", 1), 0);
    ck_assert_int_eq(_hash("sfsdgsd'sdfdsfsd'sfd", 1), 0);

    unsigned int pre_code;
    pre_code = 1 + 2 * 31 + 3 * 31 * 31;
    ck_assert_int_eq(_hash("abc", 11), pre_code % 11);

    pre_code = 1 + 2 * 31 + 3 * 31 * 31 + 4 * 31 * 31 * 31 + 5 * 31 * 31 * 31 * 31;
    ck_assert_int_eq(_hash("abcde", 11), pre_code % 11);
}
END_TEST

START_TEST(test__store_in_table)
{
    SymbolTable table = symtable_new();
    ck_assert_ptr_nonnull(table);
    HashNode n1 = {.s = {.name = "n1", .value = 1}, .next = NULL};
    HashNode n2 = {.s = {.name = "n2", .value = 2}, .next = NULL};

    ck_assert(_store_in_table(table, 0, &n1) == SUCCESS);
    ck_assert(_store_in_table(table, 1, &n2) == SUCCESS);
    ck_assert_mem_eq(table[0], &n1, sizeof(HashNode));
    ck_assert_mem_eq(table[1], &n2, sizeof(HashNode));

    table[0] = NULL;
    table[1] = NULL;

    symtable_destroy(table);
}
END_TEST

START_TEST(test__find_node_in_list)
{
    Symbol s1 = {.name = "s1", .value = 1};
    Symbol s2 = {.name = "s2", .value = 2};
    HashNode n2 = {.s = s2, .next = NULL};
    HashNode n1 = {.s = s1, .next = &n2};

    Symbol *result;
    result = _find_node_in_list(&n1, "s1");
    ck_assert_ptr_nonnull(result);
    ck_assert_mem_eq(result, &s1, sizeof(Symbol));
    result = _find_node_in_list(&n1, "s2");
    ck_assert_ptr_nonnull(result);
    ck_assert_mem_eq(result, &s2, sizeof(Symbol));
    result = _find_node_in_list(&n2, "s1");
    ck_assert_ptr_null(result);
}
END_TEST

Suite *symbols_suite(void)
{
    Suite *s = suite_create("Symbol Table");

    TCase *tc_hashnode = tcase_create("hashnode");
    tcase_add_test(tc_hashnode, test_hashnode_new);
    suite_add_tcase(s, tc_hashnode);

    TCase *tc_interface = tcase_create("interface");
    tcase_add_test(tc_interface, test_symtable_new);
    tcase_add_test(tc_interface, test_integration);
    suite_add_tcase(s, tc_interface);

    TCase *tc_helpers = tcase_create("helpers");
    tcase_add_test(tc_helpers, test__hash);
    tcase_add_test(tc_helpers, test__store_in_table);
    tcase_add_test(tc_helpers, test__find_node_in_list);
    suite_add_tcase(s, tc_helpers);

    return s;
}

/**
 * Reference on `check` unit testing framework:
 * https://libcheck.github.io/check/doc/check_html/check_3.html
 */
int main(void)
{
    int number_failed;
    Suite *s = symbols_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
