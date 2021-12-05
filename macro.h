#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR(s) fprintf(stderr, "ERROR: %s\n", s)
#define IS_LINE_NULL(l, return_code) if (l == NULL) { ERROR("Line is NULL"); return return_code;}

typedef struct {
    char key;
    int value;
} KeyVal;

// Word points to places on the actual src
typedef struct { 
    char *start; 
    char *end;
    size_t len;
} Word;

#define MAX_WORDS_IN_LINE 1000
typedef struct {
    char *src;
    char *cursor;
    size_t len;
    size_t n_words;
    size_t cur_word_idx;
    Word words[MAX_WORDS_IN_LINE];
} Line;

// Declarations
Line process_line(char *buf_src, size_t size);
Line *line_delete_char_at_cursor(Line *l);
Line *line_delete_char_at_cursor_optimized(Line *l);
Line *line_delete_range(Line *l, char *start, char *end);
char *search_char_forward(Line *l, char c);
char *search_char_backward(Line *l, char c);
char *search_until_char_forward(Line *l, char c);
char *search_until_char_backward(Line *l, char c);
Line *next_word_end(Line *l);
Line *next_word_start(Line *l);
Line *prev_word_end(Line *l);
Line *prev_word_start(Line *l);
Line *next_char(Line *l);
Line *prev_char(Line *l);
Line *set_cursor_at_start(Line *l);
Line *set_cursor_at_end(Line *l);
char *line_search_str(Line *l, char *str, size_t count);
char *line_search_str_backward(Line *l, char *str, size_t count);
char *line_copy_range(Line *l, char *start, char *end, char *buffer, size_t buffer_len);
void insert_at_cursor(Line *l, char *str, size_t str_len);

char *line_get_end_ptr(Line *l);
bool is_at_line(Line *l, char *ptr);
size_t word_idx_from_cursor(Line *l);
void free_line(Line *l);
void pretty_print_line(Line *l);
char *strstrr(const char *source, const char *target, const char *limit);
bool is_delim(char c);
bool is_delim_whitespace(char c);

#endif
