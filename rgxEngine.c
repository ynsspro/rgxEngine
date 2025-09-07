#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rgxEngine.h"  
#include <stdbool.h>
// Remove spaces from string
char* remove_spaces(const char *str) {
    if (!str) return NULL;
    char *result = malloc(strlen(str) + 1);
    if (!result) return NULL;
    char *dst = result;
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            *dst++ = *str;
        }
        str++;
    }
    *dst = '\0';
    return result;
}

// Search for a character in string
int looking_for_char(const char *exp, char c) {
    if (!exp) return -1;
    int j = 0;
    while (exp[j] != '\0') {
        if (exp[j] == c) return j;
        j++;
    }
    return -1;
}

// Generic parser for .c, .n, .s, .t
int parse_element_type(const char *exp, int i, int *pi, char expected_type) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == DOT) {
        i++;
        if (i < len && exp[i] == expected_type) {
            *pi = i + 1;
            return 1; // Valid token
        }
        return 0; // Not the expected type
    }
    return -1; // Syntax error
}

// Parse quoted string
int parse_string(const char *exp, int i, int *pi) {
    if (exp[i] != QUOTE) return -1;
    int start = i + 1;
    int pos = start;
    bool escaped = false;
    while (pos < (int)strlen(exp)) {
        if (!escaped && exp[pos] == QUOTE) {
            *pi = pos + 1;
            return pos - start; // Length of string content
        }
        if (exp[pos] == BSLA) escaped = !escaped;
        else escaped = false;
        pos++;
    }
    return -1; // Missing closing quote
}

// Parse start-with construct (-s{})
int parse_start_with(const char *exp, int i, int *pi) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == MINUS) {
        i++;
        if (i < len && exp[i] == 's') {
            i++;
            if (i < len && exp[i] == OP_CUR) {
                i++;
                int pos = looking_for_char(exp + i, CL_CUR);
                if (pos != -1) {
                    *pi = i + pos + 1;
                    return pos; // Length of content in {}
                }
                return -1; // Missing }
            }
        }
    }
    return 0; // Not a start-with expression
}

// Parse end-with construct (-e{})
int parse_end_with(const char *exp, int i, int *pi) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == MINUS) {
        i++;
        if (i < len && exp[i] == 'e') {
            i++;
            if (i < len && exp[i] == OP_CUR) {
                i++;
                int pos = looking_for_char(exp + i, CL_CUR);
                if (pos != -1) {
                    *pi = i + pos + 1;
                    return pos; // Length of content in {}
                }
                return -1; // Missing }
            }
        }
    }
    return 0; // Not an end-with expression
}

// Parse exact number of elements (e_(N))
int parse_numOf(const char *exp, int i, int *pi) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == UNDERS) {
        i++;
        if (i < len && exp[i] == OP_PAR) {
            char *endptr;
            int start = i + 1;
            if (start < len && isdigit(exp[start])) {
                long n = strtol(exp + start, &endptr, 10);
                if (endptr[0] == CL_PAR && n >= 0) {
                    *pi = (int)(endptr - exp) + 1;
                    return (int)n;
                }
            }
        }
    }
    return -1; // Syntax error
}

// Parse greater-than quantifier (e>(N))
int parse_great_than(const char *exp, int i, int *pi) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == GT) {
        i++;
        if (i < len && exp[i] == OP_PAR) {
            char *endptr;
            int start = i + 1;
            if (start < len && isdigit(exp[start])) {
                long n = strtol(exp + start, &endptr, 10);
                if (endptr[0] == CL_PAR && n >= 0) {
                    *pi = (int)(endptr - exp) + 1;
                    return (int)n;
                }
            }
        }
    }
    return -1; // Syntax error
}

// Parse less-than quantifier (e<(N))
int parse_less_than(const char *exp, int i, int *pi) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == LT) {
        i++;
        if (i < len && exp[i] == OP_PAR) {
            char *endptr;
            int start = i + 1;
            if (start < len && isdigit(exp[start])) {
                long n = strtol(exp + start, &endptr, 10);
                if (endptr[0] == CL_PAR && n >= 0) {
                    *pi = (int)(endptr - exp) + 1;
                    return (int)n;
                }
            }
        }
    }
    return -1; // Syntax error
}

// Parse range quantifier (e~(N,M))
int parse_range(const char *exp, int i, int *pi, int *arr) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == TILDE) {
        i++;
        if (i < len && exp[i] == OP_PAR) {
            char *endptr;
            int start = i + 1;
            if (start < len && isdigit(exp[start])) {
                long n = strtol(exp + start, &endptr, 10);
                if (endptr[0] == COMMA) {
                    arr[0] = (int)n;
                    start = (int)(endptr - exp) + 1;
                    if (start < len && isdigit(exp[start])) {
                        n = strtol(exp + start, &endptr, 10);
                        if (endptr[0] == CL_PAR && n >= arr[0]) {
                            arr[1] = (int)n;
                            *pi = (int)(endptr - exp) + 1;
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return -1; // Syntax error
}

// Parse or quantifier (e|(N,M))
int parse_or(const char *exp, int i, int *pi, int *arr) {
    int len = (int)strlen(exp);
    if (i < len && exp[i] == PIPE) {
        i++;
        if (i < len && exp[i] == OP_PAR) {
            char *endptr;
            int start = i + 1;
            if (start < len && isdigit(exp[start])) {
                long n = strtol(exp + start, &endptr, 10);
                if (endptr[0] == COMMA) {
                    arr[0] = (int)n;
                    start = (int)(endptr - exp) + 1;
                    if (start < len && isdigit(exp[start])) {
                        n = strtol(exp + start, &endptr, 10);
                        if (endptr[0] == CL_PAR && n >= 0) {
                            arr[1] = (int)n;
                            *pi = (int)(endptr - exp) + 1;
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return -1; // Syntax error
}

// Compile pattern into linked list
element* regex_compiler(const char *pattern) {
    if (!pattern) return NULL;
    char *pattern_nospace = remove_spaces(pattern);
    if (!pattern_nospace) return NULL;
    int len = (int)strlen(pattern_nospace);
    if (len == 0) {
        free(pattern_nospace);
        return NULL;
    }

    element *node_head = malloc(sizeof(element));
    if (!node_head) {
        free(pattern_nospace);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node_head->type = '\0';
    node_head->iteration_type = 'u';
    node_head->value = NULL;
    node_head->next = NULL;
    element *node = node_head;

    int i = 0, old_i;
    int tmp_num, tmp_arr[2];
    while (i < len) {
        old_i = i;
        switch (pattern_nospace[i]) {
            case BSLA: // Handle escaped characters
                i++;
                if (i < len && (pattern_nospace[i] == DOT || pattern_nospace[i] == QUOTE)) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = 'd';
                    node->value = malloc(2);
                    if (!node->value) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node->value[0] = pattern_nospace[i];
                    node->value[1] = '\0';
                    node->iteration_type = 'u';
                    node->next = NULL;
                    i++;
                } else {
                    fprintf(stderr, "Invalid escape sequence at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case DOT:
                if (parse_element_type(pattern_nospace, i, &i, 'c') == 1) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = 'c';
                    node->value = NULL;
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else if (parse_element_type(pattern_nospace, i, &i, 'n') == 1) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = 'n';
                    node->value = NULL;
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else if (parse_element_type(pattern_nospace, i, &i, 's') == 1) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = 's';
                    node->value = NULL;
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else if (parse_element_type(pattern_nospace, i, &i, 't') == 1) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = 't';
                    node->value = NULL;
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else {
                    fprintf(stderr, "Invalid element type at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case QUOTE:
                tmp_num = parse_string(pattern_nospace, i, &i);
                if (tmp_num >= 0) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = 'd';
                    node->value = malloc(tmp_num + 1);
                    if (!node->value) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    strncpy(node->value, pattern_nospace + old_i + 1, tmp_num);
                    node->value[tmp_num] = '\0';
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else {
                    fprintf(stderr, "Invalid string at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case UNDERS:
                tmp_num = parse_numOf(pattern_nospace, i, &i);
                if (tmp_num >= 0) {
                    node->iteration_type = 'e';
                    node->iteration[0] = tmp_num;
                } else {
                    fprintf(stderr, "Invalid exact quantifier at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case GT:
                tmp_num = parse_great_than(pattern_nospace, i, &i);
                if (tmp_num >= 0) {
                    node->iteration_type = 'g';
                    node->iteration[0] = tmp_num;
                } else {
                    fprintf(stderr, "Invalid greater-than quantifier at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case LT:
                tmp_num = parse_less_than(pattern_nospace, i, &i);
                if (tmp_num >= 0) {
                    node->iteration_type = 'l';
                    node->iteration[0] = tmp_num;
                } else {
                    fprintf(stderr, "Invalid less-than quantifier at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case TILDE:
                tmp_num = parse_range(pattern_nospace, i, &i, tmp_arr);
                if (tmp_num == 0) {
                    node->iteration_type = 'r';
                    node->iteration[0] = tmp_arr[0];
                    node->iteration[1] = tmp_arr[1];
                } else {
                    fprintf(stderr, "Invalid range quantifier at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case PIPE:
                tmp_num = parse_or(pattern_nospace, i, &i, tmp_arr);
                if (tmp_num == 0) {
                    node->iteration_type = 'o';
                    node->iteration[0] = tmp_arr[0];
                    node->iteration[1] = tmp_arr[1];
                } else {
                    fprintf(stderr, "Invalid or quantifier at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            case MINUS:
                tmp_num = parse_start_with(pattern_nospace, i, &i);
                if (tmp_num > 0) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = START_WITH;
                    node->value = malloc(tmp_num + 1);
                    if (!node->value) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    strncpy(node->value, pattern_nospace + old_i + 3, tmp_num);
                    node->value[tmp_num] = '\0';
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else if ((tmp_num = parse_end_with(pattern_nospace, i, &i)) > 0) {
                    node->next = malloc(sizeof(element));
                    if (!node->next) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    node = node->next;
                    node->type = END_WITH;
                    node->value = malloc(tmp_num + 1);
                    if (!node->value) {
                        free_reg_Str(node_head);
                        free(pattern_nospace);
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(1);
                    }
                    strncpy(node->value, pattern_nospace + old_i + 3, tmp_num);
                    node->value[tmp_num] = '\0';
                    node->iteration_type = 'u';
                    node->next = NULL;
                } else {
                    fprintf(stderr, "Invalid start/end construct at position %d\n", i);
                    free_reg_Str(node_head);
                    free(pattern_nospace);
                    return NULL;
                }
                break;
            default:
                fprintf(stderr, "Invalid character '%c' at position %d\n", pattern_nospace[i], i);
                free_reg_Str(node_head);
                free(pattern_nospace);
                return NULL;
        }
    }

    // Set default iteration_type for all nodes
    node = node_head->next;
    while (node != NULL) {
        if (node->iteration_type != 'g' && node->iteration_type != 'e' &&
            node->iteration_type != 'l' && node->iteration_type != 'o' &&
            node->iteration_type != 'r') {
            node->iteration_type = 'u';
        }
        node = node->next;
    }

    free(pattern_nospace);
    return node_head;
}

// Free the regex linked list
void free_reg_Str(element *node_head) {
    element *curr = node_head;
    while (curr != NULL) {
        element *tmp = curr;
        curr = curr->next;
        if (tmp->value) free(tmp->value);
        free(tmp);
    }
}

// Display the regex structure
void show_reg_str(element *node_head) {
    element *curr = node_head->next; // Skip dummy head
    int idx = 0;
    printf("Compiled Regex Structure:\n");
    while (curr != NULL) {
        printf("Element %d: ", idx);
        switch (curr->type) {
            case ANY_CHAR: printf(".c"); break;
            case ANY_NUM: printf(".n"); break;
            case SPACE: printf(".s"); break;
            case TAB: printf(".t"); break;
            case DEFINED_CHAR: printf("'%s'", curr->value); break;
            case START_WITH: printf("-s{%s}", curr->value); break;
            case END_WITH: printf("-e{%s}", curr->value); break;
            default: printf("Unknown"); break;
        }
        switch (curr->iteration_type) {
            case 'g': printf(" >(%d)", curr->iteration[0]); break;
            case 'l': printf(" <(%d)", curr->iteration[0]); break;
            case 'e': printf(" _(%d)", curr->iteration[0]); break;
            case 'r': printf(" ~(%d,%d)", curr->iteration[0], curr->iteration[1]); break;
            case 'o': printf(" |(%d,%d)", curr->iteration[0], curr->iteration[1]); break;
            case 'u': break; // No quantifier
        }
        printf("\n");
        curr = curr->next;
        idx++;
    }
}

// Match a single character against an element
int match_element_char(const char *str, int i, element *elem) {
    if (i >= (int)strlen(str)) return 0;
    switch (elem->type) {
        case ANY_CHAR: return isalpha(str[i]);
        case ANY_NUM: return isdigit(str[i]);
        case SPACE: return isspace(str[i]) && str[i] != '\t';
        case TAB: return str[i] == '\t';
        case DEFINED_CHAR: return strncmp(str + i, elem->value, strlen(elem->value)) == 0;
        default: return 0;
    }
}

// Match exact number of elements
int match_equal(const char *str, int str_len, int n_equ, int i, int *pos, element *elem) {
    if (n_equ < 0) return -1;
    int count = 0;
    int step = elem->type == DEFINED_CHAR ? (int)strlen(elem->value) : 1;
    while (count < n_equ && i < str_len) {
        if (!match_element_char(str, i, elem)) return 0;
        i += step;
        count++;
    }
    if (count != n_equ) return 0;
    *pos = i;
    return 1;
}

// Match greater-than number of elements
int match_greater_than(const char *str, int str_len, int n_min, int i, int *pos, element *elem) {
    if (n_min < 0) return -1;
    int count = 0;
    int step = elem->type == DEFINED_CHAR ? (int)strlen(elem->value) : 1;
    while (i < str_len && match_element_char(str, i, elem)) {
        i += step;
        count++;
    }
    if (count < n_min) return 0; // Changed to >= for inclusive
    *pos = i;
    return 1;
}

// Match less-than number of elements
int match_less_than(const char *str, int str_len, int n_max, int i, int *pos, element *elem) {
    if (n_max <= 0) return -1;
    int count = 0;
    int step = elem->type == DEFINED_CHAR ? (int)strlen(elem->value) : 1;
    while (count < n_max && i < str_len && match_element_char(str, i, elem)) {
        i += step;
        count++;
    }
    *pos = i;
    return 1;
}

// Match range of elements
int match_range(const char *str, int str_len, int n_min, int n_max, int i, int *pos, element *elem) {
    if (n_min < 0 || n_max < n_min) return -1;
    int count = 0;
    int step = elem->type == DEFINED_CHAR ? (int)strlen(elem->value) : 1;
    while (i < str_len && match_element_char(str, i, elem)) {
        i += step;
        count++;
    }
    if (count < n_min || count > n_max) return 0;
    *pos = i;
    return 1;
}

// Match or quantifier
int match_or(const char *str, int str_len, int n1, int n2, int i, int *pos, element *elem) {
    if (n1 < 0 || n2 < 0) return -1;
    int count = 0;
    int step = elem->type == DEFINED_CHAR ? (int)strlen(elem->value) : 1;
    while (i < str_len && match_element_char(str, i, elem)) {
        i += step;
        count++;
    }
    if (count != n1 && count != n2) return 0;
    *pos = i;
    return 1;
}

// Match pattern against string
int rgx_match(const char *pattern, const char *str, int *pos) {
    if (!pattern || !str || !pos) return -1;
    int str_len = (int)strlen(str);
    int i = *pos;
    element *node_head = regex_compiler(pattern);
    if (!node_head) {
        fprintf(stderr, "Pattern compilation failed\n");
        return -1;
    }
    element *node = node_head->next; // Skip dummy head
    char *start_str = NULL, *end_str = NULL;

    // Collect start/end constraints
    while (node && (node->type == START_WITH || node->type == END_WITH)) {
        if (node->type == START_WITH) {
            start_str = node->value;
        } else {
            end_str = node->value;
        }
        node = node->next;
    }

    // Verify start-with
    if (start_str && strncmp(str, start_str, strlen(start_str)) != 0) {
        free_reg_Str(node_head);
        *pos = i;
        return -1;
    }
    if (start_str) i += (int)strlen(start_str);

    // Main matching loop
    while (node && i < str_len) {
        int state = 0;
        switch (node->type) {
            case ANY_CHAR: case ANY_NUM: case SPACE: case TAB: case DEFINED_CHAR:
                switch (node->iteration_type) {
                    case 'u':
                        if (match_element_char(str, i, node)) {
                            state = 1;
                            i += node->type == DEFINED_CHAR ? (int)strlen(node->value) : 1;
                        } else {
                            state = 0;
                        }
                        break;
                    case 'e':
                        state = match_equal(str, str_len, node->iteration[0], i, &i, node);
                        break;
                    case 'g':
                        state = match_greater_than(str, str_len, node->iteration[0], i, &i, node);
                        break;
                    case 'l':
                        state = match_less_than(str, str_len, node->iteration[0], i, &i, node);
                        break;
                    case 'r':
                        state = match_range(str, str_len, node->iteration[0], node->iteration[1], i, &i, node);
                        break;
                    case 'o':
                        state = match_or(str, str_len, node->iteration[0], node->iteration[1], i, &i, node);
                        break;
                    default:
                        state = -1;
                }
                break;
            default:
                state = -1;
        }
        if (state != 1) {
            free_reg_Str(node_head);
            *pos = i;
            state=-1;
            return state;
        }
        node = node->next;
    }

    // Verify end-with
    if (end_str) {
        int end_len = (int)strlen(end_str);
        if (str_len < end_len || strcmp(str + str_len - end_len, end_str) != 0) {
            free_reg_Str(node_head);
            *pos = i;
            return -1;
        }
        i = str_len; // Consume entire string
    }

    // Check if entire pattern was consumed
    if (node != NULL || (i < str_len && !end_str)) {
        free_reg_Str(node_head);
        *pos = i;
        return -1;
    }

    free_reg_Str(node_head);
    *pos = i;
    return 1;
}

// Test function for debugging
void test_compiler(const char *pattern) {
    char *pattern_nospace = remove_spaces(pattern);
    if (!pattern_nospace) return;
    int len = (int)strlen(pattern_nospace);
    int i = 0, arr[2] = {-1, -1};

    printf("========================================\n");
    printf("Pattern (no spaces): \"%s\"\n", pattern_nospace);
    printf("========================================\n");

    while (i < len) {
        printf("\n[Step %d] Current char: '%c' (i = %d)\n", i + 1, pattern_nospace[i], i);
        printf("----------------------------------------\n");
        switch (pattern_nospace[i]) {
            case BSLA:
                printf("Escape sequence test: %d\n", i + 1 < len ? 1 : -1);
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i + 1, i + 1);
                i++;
                break;
            case DOT:
                printf("Char test: %d\n", parse_element_type(pattern_nospace, i, &i, 'c'));
                printf("Number test: %d\n", parse_element_type(pattern_nospace, i, &i, 'n'));
                printf("Space test: %d\n", parse_element_type(pattern_nospace, i, &i, 's'));
                printf("Tab test: %d\n", parse_element_type(pattern_nospace, i, &i, 't'));
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case QUOTE:
                printf("String test: %d\n", parse_string(pattern_nospace, i, &i));
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case MINUS:
                printf("Start with test: %d\n", parse_start_with(pattern_nospace, i, &i));
                printf("End with test: %d\n", parse_end_with(pattern_nospace, i, &i));
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case UNDERS:
                printf("Exact quantifier test: %d\n", parse_numOf(pattern_nospace, i, &i));
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case GT:
                printf("Greater-than test: %d\n", parse_great_than(pattern_nospace, i, &i));
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case LT:
                printf("Less-than test: %d\n", parse_less_than(pattern_nospace, i, &i));
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case TILDE:
                printf("Range test: %d (%d,%d)\n", parse_range(pattern_nospace, i, &i, arr), arr[0], arr[1]);
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            case PIPE:
                printf("Or test: %d (%d,%d)\n", parse_or(pattern_nospace, i, &i, arr), arr[0], arr[1]);
                printf("Remaining: |%s|, i = %d\n", pattern_nospace + i, i);
                break;
            default:
                printf("Invalid character '%c' at position %d\n", pattern_nospace[i], i);
                i++;
        }
        printf("----------------------------------------\n");
    }
    printf("========================================\n");
    free(pattern_nospace);
}
