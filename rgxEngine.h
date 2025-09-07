#ifndef REGEXP_H_INCLUDED
#define REGEXP_H_INCLUDED
/*
بسم الله
string of defined character : "'char'"
character undefined         : ".c"
number undefined            : ".n"
space                       : ".s"
tab                         : ".t"
element in set              : ".x{e1,e2,e3}" or ".x{start-end}" (not implemented)
number of element great than: "e>(N)"
number of element less than : "e<(N)"
serie of e                  : "e_(N)"
range                       : "e~(N,M)"
N or M                      : "e|(N,M)"
start with                  : "-s{}"
end with                    : "-e{}"
*/

// Element structure for regex linked list
typedef struct Element {
    char type;          // c: undefined char, n: undefined number, d: defined char, s: space, t: tab, S: start-with, E: end-with
    char *value;        // dynamically allocated string for defined chars or start/end constraints
    char iteration_type; // u: default, g: greater than, l: less than, e: equal, r: range, o: or
    int iteration[2];   // quantifier values (e.g., N for e_(N), or N,M for range/or)
    struct Element *next;
} element;

typedef element virt_string;
// enum for expressin symbole
enum {
    OP_BRA = '[',   // Opening Bracket
    CL_BRA = ']',   // Closing Bracket
    OP_PAR = '(',   // Opening Parenthesis
    CL_PAR = ')',   // Closing Parenthesis
    OP_CUR = '{',   // Opening Curly Brace
    CL_CUR = '}',   // Closing Curly Brace

    DOLLAR = '$',
    POUND  = '#',
    AT     = '@',
    EXCL   = '!',
    CARET  = '^',
    AMP    = '&',
    STAR   = '*',
    PLUS   = '+',
    MINUS  = '-',
    EQUAL  = '=',
    UNDERS = '_',
    PIPE   = '|',
    BSLA   = '\\',  // Backslash
    FSLA   = '/',   // Forward slash
    QUES   = '?',
    PERC   = '%',
    TILDE  = '~',
    GRAVE  = '`',

    LT     = '<',
    GT     = '>',
    COLON  = ':',
    SEMI   = ';',
    QUOTE  = '\'',
    DQUO   = '"',
    COMMA  = ',',
    DOT    = '.',


};
// Enum for element types
enum {
    DEFINED_CHAR = 'd',
    ANY_CHAR     = 'c',
    ANY_NUM      = 'n',
    SPACE        = 's',
    TAB          = 't',
    START_WITH   = 'S',
    END_WITH     = 'E'
};

// Function prototypes
char* remove_spaces(const char *str);
int looking_for_char(const char *exp, char c);
int parse_element_type(const char *exp, int i, int *pi, char expected_type);
int parse_string(const char *exp, int i, int *pi);
int parse_start_with(const char *exp, int i, int *pi);
int parse_end_with(const char *exp, int i, int *pi);
int parse_numOf(const char *exp, int i, int *pi);
int parse_great_than(const char *exp, int i, int *pi);
int parse_less_than(const char *exp, int i, int *pi);
int parse_range(const char *exp, int i, int *pi, int *arr);
int parse_or(const char *exp, int i, int *pi, int *arr);
element* regex_compiler(const char *pattern);
void free_reg_Str(element *node_head);
void show_reg_str(element *node_head);
int match_element_char(const char *str, int i, element *elem);
int match_equal(const char *str, int str_len, int n_equ, int i, int *pos, element *elem);
int match_greater_than(const char *str, int str_len, int n_min, int i, int *pos, element *elem);
int match_less_than(const char *str, int str_len, int n_max, int i, int *pos, element *elem);
int match_range(const char *str, int str_len, int n_min, int n_max, int i, int *pos, element *elem);
int match_or(const char *str, int str_len, int n1, int n2, int i, int *pos, element *elem);
int rgx_match(const char *pattern, const char *str, int *pos);
#endif