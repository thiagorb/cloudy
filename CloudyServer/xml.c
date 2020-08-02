#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "xml.h"

int is_blank(int c) {
    return (c == ' ') || (c == '\n') || (c == '\t') || (c == '\r');
}

char *read_string_recursive(FILE *f, int end_token(int c), int *next_char, int n) {
    int c = fgetchar(f);
    char *ret;
    if (end_token(c)) {
        ret = malloc(sizeof(char) * (n + 1));
        *next_char = c;
        ret[n] = 0;
    } else {
        ret = read_string(f, end_token, next_char, n + 1);
        ret[n] = c;
    }
    return ret;
}

char *read_string(FILE *f, int end_token(int c), int *next_char) {
    return read_string_recursive(f, end_token, next_char);
}

int end_token_id(int c) {
    if ((c >= 'a') && (c <= 'z')) return 0;
    if ((c >= 'A') && (c <= 'Z')) return 0;
    if ((c >= '0') && (c <= '9')) return 0;
    if (c == '_') return 0;
    return 1;
}

int end_token_value(int c) {
}

struct xml_node *parse_node(FILE* f) {
    int c;
    do {c = fgetchar(f);} while (is_blank(c));
    if (c == EOF) return NULL;
    if (c != '<') return NULL;
    char *tag = read_string(f, end_token_id, &c);
    while (is_blank(c)) c = fgetchar(f);
    if (c == '=') {
        do {c = fgetchar(f);} while (is_blank(c));
        if (c == '"') {
            struct xml_node *node = (struct xml_node *)malloc(sizeof(struct xml_node));
        }
        
    }
}

struct xml_node *parse_xml(char *file) {
    FILE *f = fopen(file);
    
}
