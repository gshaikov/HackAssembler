#include <stdlib.h>
#include <check.h>

#include "../src/symbol_table.h"

START_TEST(test_symtable_new)
{
    SymbolTable table = symtable_new();
    ck_assert_ptr_eq(table[0], NULL);
    symtable_destroy(table);
}
END_TEST

Suite *symbols_suite(void)
{
    Suite *s = suite_create("Symbol Table");

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_symtable_new);
    suite_add_tcase(s, tc_core);

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

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
