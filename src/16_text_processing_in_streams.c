#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include <stddef.h>

typedef enum ERROR ERROR;

enum ERROR {
    SUCCESS,
    ERROR_NOMATCH,
    ERROR_MALLOC_FAILED,
    ERROR_INVALID_EXPRESSION,
    ERROR_INVALID_CHARCLASS,
    ERROR_TRAILING_ESCAPE,
    ERROR_BRACKET,
    ERROR_PARENTHESES,
    ERROR_BRACES,
    ERROR_INVALID_CHARRANGE,
    ERROR_TOO_MANY_SUBGROUP,
    ERROR_EMPTY
};

ERROR error_state = SUCCESS;

bool search(const char text[restrict static 1], const char word[restrict static 1]) {
    if (strstr(text, word)) {
        fprintf(stdout, "Found the word %s\n", word);
        return true;
    } else {
        fprintf(stdout, "The word %s was not found\n", word);
        return false;
    }
}

const char* replace(const char text[restrict static 1], const char old_word[restrict static 1],
                    const char new_word[restrict static 1]) {
    char* pos = strstr(text, old_word);
    if (pos) {
        size_t new_text_size = strlen(text) - strlen(old_word) + strlen(new_word) + 1;
        char* new_text = calloc(new_text_size, sizeof(char));
        if (new_text) {
            memcpy(new_text, text, pos - text);
            memcpy(new_text + (pos - text), new_word, strlen(new_word));
            strcpy(new_text + (pos - text) + strlen(new_word), text + (pos - text) + strlen(old_word));
            return new_text;
        } else {
            error_state = ERROR_MALLOC_FAILED;
            fprintf(stderr, "Memory allocation failed\n");
            return text;
        }
    } else {
        return text;
    }
}

typedef struct RegExpMatch RegExpMatch;

// circular linked list
struct RegExpMatch {
    const char* begin;
    const char* end;
    RegExpMatch* next;
    size_t group_size;
    RegExpMatch* subgroup[10];
};

static bool regex_match_recursive(const char text[restrict static 1], const char pattern[restrict static 1],
                                  RegExpMatch* curr_match);
static bool regex_match_star(char ch, const char text[restrict static 1], const char pattern[restrict static 1],
                             RegExpMatch* curr_match);

typedef struct CharClass CharClass;

static struct CharClass {
    const char* name;
    const char* chars;
} char_classes[] = {
        {"alnum", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"},
        {"alpha", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
        {"blank", " \t"},
        {"cntrl", "\\007\\b\\t\\n\\v\\f\\r\\1\\2\\3\\4\\5\\6\\16\\17\\20\\21\\22\\23\\24"
                  "\\25\\26\\27\\30\\31\\32\\33\\34\\35\\36\\37\\177"},
        {"digit", "0123456789"},
        {"graph", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                  "0123456789!\\\"#$%&'()*+,-./:;<=>?@[\\\\]^_`{|}~"},
        {"lower", "abcdefghijklmnopqrstuvwxyz"},
        {"print", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                  "0123456789!\\\"#$%&'()*+,-./:;<=>?@[\\\\]^_`{|}~ "},
        {"punct", "!\\\"#$%&'()*+,-./:;<=>?@[\\\\]^_`{|}~"},
        {"space", "\t\n\v\f\r "},
        {"upper", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
        {"xdigit", "0123456789ABCDEFabcdef"},
};

typedef struct CharSet CharSet;

struct CharSet {
    uint8_t mask[16];
};

void add_char(char c, CharSet cs[static 1]) {
    assert(c > 0);
    cs->mask[c / 8] |= (1u << (c % 8));
}

bool is_in_charset(char c, CharSet cs[static 1]) {
    assert(c >= 0);
    if (!c) return false;
    return (cs->mask[c / 8] & (1u << (c % 8)));
}

size_t regex_parse_bracket_charclass(const char pattern[restrict static 1], CharSet cs[static 1]) {
    const char* pattern_start = pattern;
    while (*pattern && isalpha(*pattern++)) ;
    size_t length = pattern - pattern_start - 1;
    CharClass* char_class = 0;
    for (char_class = char_classes; char_class->name; char_class++) {
        if (!strncmp(char_class->name, pattern_start, length) && !char_class->name[length]) {
            break;
        }
    }
    if (!char_class->name) {
        error_state = ERROR_INVALID_CHARCLASS;
        fprintf(stderr, "Invalid character class\n");
        return 0;
    }

    const char* chars = char_class->chars;
    char ch;
    while ((ch = *chars++)) {
        add_char(ch, cs);
    }
    return length;
}

size_t regex_parse_bracket_charlist_unit(const char pattern[restrict static 1], CharSet cs[static 1]) {
    size_t parsed = 0;
    switch (pattern[0]) {
        case ':': // character class
            pattern++;
            parsed++;
            if (!pattern[0]) {
                error_state = ERROR_BRACKET;
                fprintf(stderr, "Bracket does not match\n");
                return 0;
            }
            if (pattern[0] == ']') {
                error_state = ERROR_INVALID_CHARCLASS;
                fprintf(stderr, "Invalid character class\n");
                return 0;
            }
            size_t length = regex_parse_bracket_charclass(pattern, cs);
            pattern += length;
            parsed += length;
            if (!pattern[0]) {
                error_state = ERROR_BRACKET;
                fprintf(stderr, "Bracket does not match\n");
                return 0;
            }
            if (!(pattern[0] == ':' && pattern[1] == ']')) {
                error_state = ERROR_INVALID_CHARCLASS;
                fprintf(stderr, "Invalid character class\n");
                return 0;
            } else {
                parsed += 1;
            }
            break;
        default:
            if (!pattern[0]) {
                error_state = ERROR_BRACKET;
                fprintf(stderr, "Bracket does not match\n");
                return 0;
            }
            char start = pattern[0];
            char finish;
            if (pattern[1] == '-' && pattern[2] && pattern[2] != ']') { // range
                finish = pattern[2];
                parsed += 3;
            } else {
                finish = start;
                parsed += 1;
            }
            if (start > finish) {
                error_state = ERROR_INVALID_CHARRANGE;
                fprintf(stderr, "Invalid character range\n");
                return 0;
            }
            for (char ch = start; ch <= finish; ch++) {
                add_char(ch, cs);
            }
            break;
    }
    return parsed;
}

CharSet* regex_parse_bracket_charlist(const char star[restrict static 1], const char pattern[restrict static 1]) {
    CharSet* cs = calloc(1, sizeof(CharSet));
    if (!cs) {
        error_state = ERROR_MALLOC_FAILED;
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    bool invert = false;
    if (pattern[0] == '^') {
        pattern++;
        invert = true;
    }
    while (pattern[0] && pattern[0] != ']') {
        pattern += regex_parse_bracket_charlist_unit(pattern, cs);
    }
    if (pattern[0] != ']') {
        error_state = ERROR_BRACKET;
        fprintf(stderr, "Bracket does not match\n");
        free(cs);
        return 0;
    }
    if (invert) {
        for (size_t i = 0; i < 16; i++) {
            cs->mask[i] = ~cs->mask[i];
        }
    }

    return cs;
}

bool regex_match_star_cs(CharSet cs[static 1], const char text[restrict static 1], const char pattern[restrict static 1],
                         RegExpMatch curr_match[static 1]) {
    bool matched = false;
    do {
        if (regex_match_recursive(text, pattern, curr_match)) {
            matched = true;
        }
    } while (*text && (is_in_charset(*text++, cs)));
    return matched;
}

bool regex_match_recursive(const char text[restrict static 1], const char pattern[restrict static 1],
                           RegExpMatch curr_match[static 1]) {
    if (!pattern[0]) {
        curr_match->end = text;
        return true;
    }
    if (pattern[0] == '(') {
        if (curr_match->group_size >= 10) {
            error_state = ERROR_TOO_MANY_SUBGROUP;
            fprintf(stderr, "Too many subgroups\n");
            return false;
        }
        pattern++;
        const char* pattern_start = pattern;
        while (*pattern && (*++pattern != ')'));
        if (*pattern != ')') {
            error_state = ERROR_PARENTHESES;
            fprintf(stderr, "Parentheses does not match\n");
            return false;
        }
        size_t length = pattern - pattern_start;
        pattern++;
        char* subpattern = calloc(length + 1, 1);
        strncpy(subpattern, pattern_start, length);
        curr_match->group_size++; // starts from 1.
        curr_match->subgroup[curr_match->group_size] = calloc(1, sizeof(RegExpMatch));
        RegExpMatch* subgroup_match = curr_match->subgroup[curr_match->group_size];
        *subgroup_match = (RegExpMatch) {
                .begin = text,
                .end = text,
                .next = subgroup_match,
        };
        for (size_t i = 0; i < curr_match->group_size; i++) {
            subgroup_match->subgroup[i] = curr_match->subgroup[i];
        }
        subgroup_match->group_size = curr_match->group_size;
        if (regex_match_recursive(text, subpattern, subgroup_match)) {
            RegExpMatch* curr_group_curr_node = subgroup_match;
            bool subgroup_matched = false;
            size_t curr_group_length = 0;
            do {
                curr_group_length = curr_group_curr_node->end - curr_group_curr_node->begin;
                if (!strncmp(curr_group_curr_node->begin, text, curr_group_length)) {
                    subgroup_matched = true;
                    break;
                }
                curr_group_curr_node = curr_group_curr_node->next;
            } while (curr_group_curr_node->next != subgroup_match);
            if (!subgroup_matched) {
                return false;
            } else {
                text += curr_group_length;
                return regex_match_recursive(text, pattern, curr_match);
            }
        } else {
            return false;
        }
    }

    CharSet* cs = 0;
    if (pattern[0] == '[') {
        cs = regex_parse_bracket_charlist(text, ++pattern);
        while (*pattern && *pattern++ != ']');
    }

    if (pattern[0] == '\\') {
        if (pattern[1] >= '1' && pattern[1] <= '9') {
            size_t curr_group_index = pattern[1] - '0';
            RegExpMatch* curr_group_head = curr_match->subgroup[curr_group_index];
            if (!curr_group_head) {
                return false;
            } else {
                RegExpMatch* curr_group_curr_node = curr_group_head;
                bool subgroup_matched = false;
                size_t curr_group_length = 0;
                do {
                    curr_group_length = curr_group_curr_node->end - curr_group_curr_node->begin;
                    if (!strncmp(curr_group_curr_node->begin, text, curr_group_length)) {
                        subgroup_matched = true;
                        break;
                    }
                    curr_group_curr_node = curr_group_curr_node->next;
                } while (curr_group_curr_node->next != curr_group_head);
                if (!subgroup_matched) {
                    return false;
                } else {
                    pattern += 2;
                    text += curr_group_length;
                    return regex_match_recursive(text, pattern, curr_match);
                }
            }
        } else {
            error_state = ERROR_INVALID_EXPRESSION;
            fprintf(stderr, "Invalid expression\n");
            return false;
        }
    }

    if (pattern[1] == '*') {
        if (cs) {
            CharSet* aux_cs = calloc(1, sizeof(CharSet));
            for (size_t i = 0; i < 16; i++) {
                aux_cs->mask[i] = cs->mask[i];
            }
            add_char(pattern[0], aux_cs);
            return regex_match_star_cs(aux_cs, text, pattern + 2, curr_match);
        } else {
            return regex_match_star(pattern[0], text, pattern + 2, curr_match);
        }
    }
    if (*text && (pattern[0] == '?' || pattern[0] == *text || (cs && is_in_charset(*text, cs)))) {
        if (!pattern[0]) {
            curr_match->end = text + 1;
            return true;
        } else {
            return regex_match_recursive(text + 1, pattern + 1, curr_match);
        }
    }
    return false;
}

bool regex_match_star(char ch, const char text[restrict static 1], const char pattern[restrict static 1],
                      RegExpMatch curr_match[static 1]) {
    bool matched = false;
    do {
        if (regex_match_recursive(text, pattern, curr_match)) {
            matched = true;
        }
    } while (*text && (*text++ == ch || ch == '?'));
    return matched;
}

bool regex_match(const char text[restrict static 1], const char pattern[restrict static 1],
                 RegExpMatch curr_match[static 1]) {
    bool matched = false;
    *curr_match = (RegExpMatch) {
            .begin = text,
            .end = text,
            .next = curr_match,
    };
    do {
        if (regex_match_recursive(text, pattern, curr_match)) {
            matched = true;
            RegExpMatch* next_match = calloc(1, sizeof(RegExpMatch));
            if (!next_match) {
                error_state = ERROR_MALLOC_FAILED;
                fprintf(stderr, "Memory allocation failed\n");
                return matched;
            }
            *next_match = (RegExpMatch) {
                    .begin = text,
                    .end = text,
                    .next = curr_match->next,
            };
            curr_match->next = next_match;
            curr_match = next_match;
        }
    } while (*text++ && (curr_match->begin = text) && !(curr_match->group_size = 0));
    return matched;
}

void print_regex_result(RegExpMatch result[static 1]) {
    RegExpMatch* head = result;
    RegExpMatch* curr = result;
    while (curr->next != head) {
        const char* ptr = curr->begin;
        if (!*ptr || curr->begin == curr->end) {
            curr = curr->next;
            continue;
        }
        while (*ptr && ptr != curr->end) {
            fputc(*ptr++, stdout);
        }
        fputc('\n', stdout);
        curr = curr->next;
    }
}

size_t count_occurrence(const char text[restrict static 1], const char pattern[restrict static 1],
                        RegExpMatch curr_match[static 1]) {
    regex_match(text, pattern, curr_match);
    RegExpMatch* head = curr_match;
    RegExpMatch* curr = curr_match;
    size_t num_occurrence = 0;
    while (curr->next != head) {
        const char* ptr = curr->begin;
        if (!*ptr || curr->begin == curr->end) {
            curr = curr->next;
            continue;
        }
        num_occurrence++;
        curr = curr->next;
    }
    return num_occurrence;
}

const char* regex_replace(const char text[restrict static 1], RegExpMatch result[static 1],
                          const char new_word[restrict static 1]) {
    RegExpMatch* head = result;
    RegExpMatch* curr = result;
    size_t word_length = strlen(new_word);
    size_t list_length = 0;
    const char* text_pos = text;
    size_t text_length = 0;
    while (curr->next != head) {
        const char* ptr = curr->begin;
        if (!*ptr || curr->begin == curr->end) {
            curr = curr->next;
            continue;
        }
        text_length += curr->begin - text_pos;
        text_length += word_length;
        text_pos = curr->end;
        list_length++;
        curr = curr->next;
    }
    size_t rest_length = 0;
    if (!list_length) {
        return text;
    } else {
        rest_length = strlen(text) - (text_pos - text);
        text_length += rest_length;
    }
    text_pos = text;
    char* new_text = calloc(text_length + 1, 1);
    curr = result;
    char* new_text_pos = new_text;
    while (curr->next != head) {
        const char* ptr = curr->begin;
        if (!*ptr || curr->begin == curr->end) {
            curr = curr->next;
            continue;
        }
        size_t preceding_unmatched = curr->begin - text_pos;
        strncpy(new_text_pos, text_pos, preceding_unmatched);
        new_text_pos += preceding_unmatched;
        strncpy(new_text_pos, new_word, word_length);
        new_text_pos += word_length;
        text_pos = curr->end;
        curr = curr->next;
    }
    strncpy(new_text_pos, text_pos, rest_length);
    return new_text;
}

int main() {
    char sample_text[256];
    char query_text[256];
    char replacing_text[256];

    fprintf(stdout, "Input the text.\n");
    if (fgets(sample_text, sizeof sample_text, stdin)) {
        fprintf(stdout, "Input the query text.\n");
        if (fgets(query_text, sizeof query_text, stdin)) {
            RegExpMatch result = (RegExpMatch) {0};
            size_t num_occurrence = count_occurrence(sample_text, query_text, &result);
            fprintf(stdout, "Number of occurrence of %s is %zu\n", query_text, num_occurrence);
        } else {
            fprintf(stderr, "Invalid query text\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Invalid text\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Input the replacing text.\n");
    if (fgets(replacing_text, sizeof replacing_text, stdin)) {
        RegExpMatch result = (RegExpMatch) {0};
        regex_match(sample_text, query_text, &result);
        const char* replaced_text = regex_replace(sample_text, &result, replacing_text);
        fprintf(stdout, "Replaced text is: %s\n", replaced_text);
    } else {
        fprintf(stderr, "Invalid replacing text\n");
        return EXIT_FAILURE;
    }

}