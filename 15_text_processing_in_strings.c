#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

bool search(const char text[restrict static 1], const char word[restrict static 1]) {
    if (strstr(text, word)) {
        printf("Found the word %s\n", word);
        return true;
    } else {
        printf("The word %s was not found\n", word);
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
            printf("Memory allocation failed\n");
            return text;
        }
    } else {
        return text;
    }
}

typedef struct RegExpMatch RegExpMatch;

struct RegExpMatch {
    size_t begin;
    size_t length;
    RegExpMatch* prev;
    RegExpMatch* next;
};

static bool regex_match_recursive(const char text[restrict static 1], const char pattern[restrict static 1]);
static bool regex_match_star(char ch, const char text[restrict static 1], const char pattern[restrict static 1]);

bool regex_match_recursive(const char text[restrict static 1], const char pattern[restrict static 1]) {
    if (!pattern[0]) {
        return true;
    }
    if (pattern[0] == '[') {
        
    }
    if (pattern[1] == '*') {
        return regex_match_star(pattern[0], text, pattern + 2);
    }
    if (*text && (pattern[0] == '?' || pattern[0] == *text)) {
        return regex_match_recursive(text + 1, pattern + 1);
    }
    return false;
}

bool regex_match_star(char ch, const char text[restrict static 1], const char pattern[restrict static 1]) {
    do {
        if (regex_match_recursive(text, pattern)) {
            return true;
        }
    } while (*text && (*text++ == ch || ch == '?'));
    return false;
}

bool regex_match(const char text[restrict static 1], const char pattern[restrict static 1]) {
    do {
        if (regex_match_recursive(text, pattern)) {
            return true;
        }
    } while (*text++);
    return false;
}

int main() {
    const char* sample_text =
            "RegExr was created by gskinner.com, and is proudly hosted by Media Temple. "
            "Edit the Expression & Text to see matches. Roll over matches or the expression for details. "
            "PCRE & JavaScript flavors of RegEx are supported. "
            "The side bar includes a Cheatsheet, full Reference, and Help. "
            "You can also Save & Share with the Community, and view patterns you create or favorite in My Patterns. "
            "Explore results with the Tools below. Replace & List output custom results. "
            "Details lists capture groups. Explain describes your expression in plain English.";

    assert(search(sample_text, "Expression"));
    assert(!search(sample_text, "Expresion"));
    const char* new_sample_text = replace(sample_text, "Expression", "espresso");
    printf("%s\n", new_sample_text);

    assert(regex_match(sample_text, "?*"));


}