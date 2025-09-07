#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"rgxEngine.h"
int main(void) {
    struct TestCase {
        const char *pattern;
        const char *str;
        int expected;
    } tests[] = {
        {".c_(3)", "abc", 1},
        {".c_(3)", "ab", -1}, 
        {".c_(3)", "abcd", -1},
        {".n>(2)", "123", 1},
        {".n>(2)", "12", 1}, 
        {".n>(2)", "1", -1}, 
        {"'ab'_(2)", "abab", 1},
        {"'a'_(2)", "ab", -1}, 
        {".s~(1,3)", "  ", 1},
        {".s~(1,3)", "    ", -1}, 
        {".c|(2,4)", "abcd", 1},
        {".c|(2,4)", "abcde", -1}, 
        {"-s{abc}.c", "abcd", 1},
        {"-s{abc}.c", "defabc", -1},
        {"-e{xyz}", "abcxyz", 1},
        {"-e{xyz}", "abcdef", -1},
        {"\\..c", ".c", 1},
        {"'a\\''_(2)", "a\\'a\\'", 1},/*this one  work because the quotes '' escape the escaped charctere itself  / */ 
        {".c|(1,4)'.'.c>(2)'@gmail.com'", "john.doe@gmail.com", 1}
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        int pos = 0;
        printf("Test %zu: Pattern=\"%s\", String=\"%s\"\n", i + 1, tests[i].pattern, tests[i].str);
        int result = rgx_match(tests[i].pattern, tests[i].str, &pos);
        printf("Result: %d (Expected: %d, Pos: %d)\n", result, tests[i].expected, pos);
        element *node_head = regex_compiler(tests[i].pattern);
        if (node_head) {
            show_reg_str(node_head);
            free_reg_Str(node_head);
        }
        printf("----------------------------------------\n");
    }

    return 0;
}
