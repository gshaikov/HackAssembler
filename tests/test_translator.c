#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "../src/symbol_table.h"
#include "../src/translator.h"

START_TEST(test__load_symbols_array)
{
    SymbolTable table = symtable_new();
    Symbol arr[] = {
        {.name = "R0", .value = 0},
        {.name = "R1", .value = 1},
    };
    Error err = _load_symbols_array(table, 2, arr);
    ck_assert(err == SUCCESS);
    Symbol *res = symtable_view_by_name(table, "R1");
    ck_assert_mem_eq(&arr[1], res, sizeof(Symbol));
    symtable_destroy(table);
}
END_TEST

START_TEST(test__copy_statements_only)
{
    FILE *from = tmpfile();
    fputs("\n\n \n  \n//abc \na=b;c // efg \n//a=b;c\n   d;jmp\n(LOOP)\n\n", from);
    rewind(from);

    FILE *to = tmpfile();
    Error err = _copy_statements_only(to, from);
    ck_assert(err == SUCCESS);
    rewind(to);

    char buf[256];
    int n = fread(buf, sizeof(char), 256, to);
    buf[n] = '\0';
    ck_assert_str_eq("a=b;c\nd;jmp\n(LOOP)\n", buf);

    fclose(from);
    fclose(to);
}
END_TEST

START_TEST(test__load_labels)
{
    SymbolTable table = symtable_new();
    FILE *from = tmpfile();
    fputs("(FOO)\na=b;c\nd;jmp\n(LOOP)\nd;jmp\nd;jmp\n(BAR)", from);
    rewind(from);

    Error err = _load_labels(table, from);
    ck_assert(err == SUCCESS);

    Symbol *res;

    res = symtable_view_by_name(table, "FOO");
    ck_assert_ptr_nonnull(res);
    ck_assert_str_eq("FOO", res->name);
    ck_assert_uint_eq(0, res->value);

    res = symtable_view_by_name(table, "LOOP");
    ck_assert_ptr_nonnull(res);
    ck_assert_str_eq("LOOP", res->name);
    ck_assert_uint_eq(2, res->value);

    res = symtable_view_by_name(table, "BAR");
    ck_assert_ptr_nonnull(res);
    ck_assert_str_eq("BAR", res->name);
    ck_assert_uint_eq(4, res->value);

    fclose(from);
    symtable_destroy(table);
}
END_TEST

START_TEST(test__skip_until_newline)
{
    FILE *tmp = tmpfile();
    fputs("abc\nefg\n", tmp);
    rewind(tmp);
    Error err = _skip_until_newline(tmp);
    ck_assert(err == SUCCESS);
    ck_assert_int_eq(ftell(tmp), 4);
    fclose(tmp);
}
END_TEST

Suite *symbols_suite(void)
{
    Suite *s = suite_create("Translator");

    TCase *tc_logic = tcase_create("logic");
    tcase_add_test(tc_logic, test__load_symbols_array);
    tcase_add_test(tc_logic, test__copy_statements_only);
    tcase_add_test(tc_logic, test__load_labels);
    suite_add_tcase(s, tc_logic);

    TCase *tc_helpers = tcase_create("helpers");
    tcase_add_test(tc_helpers, test__skip_until_newline);
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
