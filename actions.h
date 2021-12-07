#ifndef ACTIONS_H
#define ACTIONS_H

#include "macro.h"

// All actions and movements
enum { DELETE = 1, INSERT, YANK, PASTE, PASTE_BACKWARD };
enum { FORWARD = 1, BACKWARD, WORD_FORWARD, WORD_BACKWARD, 
       FIND, FIND_BACKWARD, TILL, TILL_BACKWARD, LINE_START, 
       LINE_END, SEARCH_FORWARD, SEARCH_BACKWARD, MATCH }; 

#define MAX_SEARCH_LEN 100
typedef struct {
    char text[MAX_SEARCH_LEN];
    size_t len;
} SearchText;

typedef struct {
    char identifier;
    int command;
    char arg;
    size_t count;
    SearchText s_text;
} Movement;

#define MAX_INSERT_LEN 50
typedef struct {
    char text[MAX_INSERT_LEN];
    size_t len;
} Insert;

typedef struct {
    char identifier;
    int command;
    Movement mov;
    Insert ins;
} Action;

// Structure to hold the copied text
// There sould be one yank buffer
#define YANK_BUF_MAX 200
typedef struct {
    char buf[YANK_BUF_MAX];
    size_t len;
} YankBuffer;

Action process_actions(char *action_str, size_t len);
Line *eval_action_on_line(Line *l, Action *a);

void movement_forward(Line *l, Action *a);
void movement_backward(Line *l, Action *a);
void movement_word_forward(Line *l, Action *a);
void movement_word_backward(Line *l, Action *a);
void movement_line_start(Line *l, Action *a);
void movement_line_end(Line *l, Action *a);
void movement_find(Line *l, Action *a);
void movement_find_backward(Line *l, Action *a);
void movement_till(Line *l, Action *a);
void movement_till_backward(Line *l, Action *a);
void movement_search(Line *l, Action *a);
void movement_search_backward(Line *l, Action *a);
void movement_match_pair(Line *l, Action *a);

void action_delete_word_forward(Line *l, Action *a);
void action_delete_word_backward(Line *l, Action *a);
void action_delete_forward(Line *l, Action *a);
void action_delete_backward(Line *l, Action *a);
void action_delete_to_line_end(Line *l, Action *a);
void action_delete_to_line_start(Line *l, Action *a);
void action_delete_to_find(Line *l, Action *a);
void action_delete_to_find_backward(Line *l, Action *a);
void action_delete_till(Line *l, Action *a);
void action_delete_till_backward(Line *l, Action *a);
void action_delete_search(Line *l, Action *a);
void action_delete_search_backward(Line *l, Action *a);
void action_delete_match_pair(Line *l, Action *a);

void action_insert_at_cursor(Line *l, Action *a);

void action_yank_forward(Line *l, Action *a);
void action_yank_backward(Line *l, Action *a);
void action_yank_word_forward(Line *l, Action *a);
void action_yank_word_backward(Line *l, Action *a);
void action_yank_to_line_start(Line *l, Action *a);
void action_yank_to_line_end(Line *l, Action *a);
void action_yank_to_find(Line *l, Action *a);
void action_yank_to_find_backward(Line *l, Action *a);
void action_yank_till(Line *l, Action *a);
void action_yank_till_backward(Line *l, Action *a);
void action_yank_search(Line *l, Action *a);
void action_yank_search_backward(Line *l, Action *a);
void action_yank_match_pair(Line *l, Action *a);

void action_paste_at_cursor(Line *l, Action *a);
void action_paste_backward_at_cursor(Line *l, Action *a);

bool is_action(char c);
bool is_movement(char c);
int movement_get_value_for_key(char key);
void clear_yank_buffer();
int action_get_value_for_key(char key);
char get_matching_char(char c);
bool is_opening(char c);

#endif
