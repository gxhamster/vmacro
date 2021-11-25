#ifndef MACRO_H
#define MACRO_H

#include <stdlib.h>
#include <stdbool.h>

#define ERROR(s) fprintf(stderr, "ERROR: %s\n", s)
#define IS_LINE_NULL(l, return_code) if (l == NULL) { ERROR("Line is NULL"); return return_code;}

// All actions and movements
enum { DELETE = 1 };
enum { FORWARD = 1, BACKWARD, WORD_FORWARD, WORD_BACKWARD, FIND, TILL }; 

typedef struct {
    char key;
    int value;
} KeyVal;


typedef struct {
    char identifier;
    int command;
    char arg;
    size_t count;
} Movement;

// d2w -> delete two words
typedef struct {
    char identifier;
    int command;
    Movement mov; 
} Action;


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
Action process_actions(char *action_str, size_t len);
Line *line_delete_word_at_cursor(Line *l);
Line *line_delete_char_at_cursor(Line *l);
Line *eval_action_on_line(Line *l, Action *a);
char *read_from_file(FILE *fp);
Line *next_word_end(Line *l);
Line *next_word_start(Line *l);
Line *prev_word_start(Line *l);
Line *next_char(Line *l);
Line *prev_char(Line *l);
size_t word_idx_from_cursor(Line *l);
bool is_action(char c);
bool is_movement(char c);
int movement_get_value_for_key(char key);
int action_get_value_for_key(char key);

#endif
