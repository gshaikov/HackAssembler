#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "../src/symbol_table.h"
#include "../src/string_table.h"
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
    char exp[] = "a=b;c\nd;jmp\n(LOOP)\n";
    ck_assert_str_eq(exp, buf);

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

START_TEST(test__assemble_binary)
{
    StrTable comp = strtable_new();
    ck_assert_ptr_nonnull(comp);
    StrEntry c_a = {.key = "A", .value = "0110000"};
    StrEntry c_d = {.key = "D", .value = "0001100"};
    ck_assert(strtable_add(comp, c_a) == SUCCESS);
    ck_assert(strtable_add(comp, c_d) == SUCCESS);

    StrTable dest = strtable_new();
    ck_assert_ptr_nonnull(dest);
    StrEntry d_n = {.key = "", .value = "000"};
    StrEntry d_d = {.key = "D", .value = "010"};
    ck_assert(strtable_add(dest, d_n) == SUCCESS);
    ck_assert(strtable_add(dest, d_d) == SUCCESS);

    StrTable jump = strtable_new();
    ck_assert_ptr_nonnull(jump);
    StrEntry j_nul = {.key = "", .value = "000"};
    StrEntry j_jeq = {.key = "JEQ", .value = "010"};
    ck_assert(strtable_add(jump, j_nul) == SUCCESS);
    ck_assert(strtable_add(jump, j_jeq) == SUCCESS);

    SymbolTable table = symtable_new();
    Symbol s1 = {.name = "FOO", .value = 2};
    ck_assert(symtable_add(table, s1) == SUCCESS);

    FILE *from = tmpfile();
    fputs("@2\nD=A\n(FOO)\nD;JEQ\n@FOO\n@var\n@bar\n@var", from);
    rewind(from);

    FILE *to = tmpfile();
    ck_assert(_assemble_binary(to, from, table, comp, dest, jump) == SUCCESS);
    rewind(to);

    char buf[512];
    int n = fread(buf, sizeof(char), 512, to);
    buf[n] = '\0';
    char exp[] = "0000000000000010\n1110110000010000\n1110001100000010\n0000000000000010\n0000000000010000\n0000000000010001\n0000000000010000\n";
    ck_assert_str_eq(exp, buf);

    fclose(from);
    symtable_destroy(table);
    strtable_destroy(comp);
    strtable_destroy(dest);
    strtable_destroy(jump);
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

START_TEST(test__translate_A_instruction)
{
    {
        char buf[] = "@2";
        char bin[17];
        ck_assert(_translate_A_instruction(bin, buf, NULL, NULL) == SUCCESS);
        ck_assert_str_eq("0000000000000010", bin);
    }
    {
        char buf[] = "@16";
        char bin[17];
        ck_assert(_translate_A_instruction(bin, buf, NULL, NULL) == SUCCESS);
        ck_assert_str_eq("0000000000010000", bin);
    }
    {
        int next_free_address = 128;
        int *p_next_free_address = &next_free_address;
        SymbolTable table = symtable_new();
        char buf[] = "@var";
        char bin[17];
        ck_assert(_translate_A_instruction(bin, buf, table, p_next_free_address) == SUCCESS);
        ck_assert_str_eq("0000000010000000", bin);
        ck_assert_int_eq(129, next_free_address);
        symtable_destroy(table);
    }
    {
        SymbolTable table = symtable_new();
        Symbol s1 = {.name = "var", .value = 256};
        ck_assert(symtable_add(table, s1) == SUCCESS);
        char buf[] = "@var";
        char bin[17];
        ck_assert(_translate_A_instruction(bin, buf, table, NULL) == SUCCESS);
        ck_assert_str_eq("0000000100000000", bin);
        symtable_destroy(table);
    }
}
END_TEST

START_TEST(test__translate_C_instruction)
{
    {
        StrTable comp = strtable_new();
        ck_assert_ptr_nonnull(comp);
        StrEntry c = {.key = "A", .value = "0110000"};
        ck_assert(strtable_add(comp, c) == SUCCESS);

        StrTable dest = strtable_new();
        ck_assert_ptr_nonnull(dest);
        StrEntry d = {.key = "D", .value = "010"};
        ck_assert(strtable_add(dest, d) == SUCCESS);

        StrTable jump = strtable_new();
        ck_assert_ptr_nonnull(jump);
        StrEntry j = {.key = "", .value = "000"};
        ck_assert(strtable_add(jump, j) == SUCCESS);

        char buf[] = "D=A";
        char bin[17];
        ck_assert(_translate_C_instruction(bin, buf, comp, dest, jump) == SUCCESS);
        ck_assert_str_eq("1110110000010000", bin);

        strtable_destroy(comp);
        strtable_destroy(dest);
        strtable_destroy(jump);
    }
    {
        StrTable comp = strtable_new();
        ck_assert_ptr_nonnull(comp);
        StrEntry c = {.key = "D", .value = "0001100"};
        ck_assert(strtable_add(comp, c) == SUCCESS);

        StrTable dest = strtable_new();
        ck_assert_ptr_nonnull(dest);
        StrEntry d = {.key = "", .value = "000"};
        ck_assert(strtable_add(dest, d) == SUCCESS);

        StrTable jump = strtable_new();
        ck_assert_ptr_nonnull(jump);
        StrEntry j = {.key = "JEQ", .value = "010"};
        ck_assert(strtable_add(jump, j) == SUCCESS);

        char buf[] = "D;JEQ";
        char bin[17];
        ck_assert(_translate_C_instruction(bin, buf, comp, dest, jump) == SUCCESS);
        ck_assert_str_eq("1110001100000010", bin);

        strtable_destroy(comp);
        strtable_destroy(dest);
        strtable_destroy(jump);
    }
}
END_TEST

Suite *symbols_suite(void)
{
    Suite *s = suite_create("Translator");

    TCase *tc_logic = tcase_create("logic");
    tcase_add_test(tc_logic, test__load_symbols_array);
    tcase_add_test(tc_logic, test__copy_statements_only);
    tcase_add_test(tc_logic, test__load_labels);
    tcase_add_test(tc_logic, test__assemble_binary);
    suite_add_tcase(s, tc_logic);

    TCase *tc_helpers = tcase_create("helpers");
    tcase_add_test(tc_helpers, test__skip_until_newline);
    tcase_add_test(tc_helpers, test__translate_A_instruction);
    tcase_add_test(tc_helpers, test__translate_C_instruction);
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
