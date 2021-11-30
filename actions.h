#ifndef ACTIONS_H
#define ACTIONS_H

#include "macro.h"

// All actions and movements
enum { DELETE = 1, INSERT };
enum { FORWARD = 1, BACKWARD, WORD_FORWARD, WORD_BACKWARD, 
       FIND, FIND_BACKWARD, TILL, TILL_BACKWARD, LINE_START, 
       LINE_END }; 

typedef struct {
    char identifier;
    int command;
    char arg;
    size_t count;
} Movement;

#define MAX_INSERT_LEN 20
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

void action_insert_at_cursor(Line *l, Action *a);

bool is_action(char c);
bool is_movement(char c);
int movement_get_value_for_key(char key);
int action_get_value_for_key(char key);

#endif
