#include "actions.h"

// This file defines what each vim action and movement does. 

const KeyVal mapped_actions[] = {
    {'d', DELETE}
};

const KeyVal mapped_movements[] = {
    {'l', FORWARD},
    {'h', BACKWARD},
    {'w', WORD_FORWARD},
    {'b', WORD_BACKWARD},
    {'^', LINE_START},
    {'$', LINE_END},
    {'f', FIND},
    {'t', TILL}
};

// Movements
void movement_forward(Line *l, Action *a)
{
    if (a->command == DELETE)
        action_delete_forward(l, a);

    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        next_char(l);
    }
}

void movement_backward(Line *l, Action *a)
{
    if (a->command == DELETE)
        action_delete_backward(l, a);

    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        prev_char(l);
    }
}

void movement_word_forward(Line *l, Action *a)
{
    if (a->command == DELETE)
        action_delete_word_forward(l, a);

    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        next_word_start(l);
    }
}

void movement_word_backward(Line *l, Action *a)
{
    if (a->command == DELETE)
        action_delete_word_backward(l, a);

    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        prev_word_start(l);
    }

}

void movement_line_start(Line *l, Action *a)
{
    (void) a;
    set_cursor_at_start(l);      
}

void movement_line_end(Line *l, Action *a)
{
    (void) a;
    set_cursor_at_end(l);

}

void movement_find(Line *l, Action *a)
{
    // TODO: Maybe move this to a function
    size_t i;
    char *cur_pos;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        cur_pos = search_char_forward(l, a->mov.arg);
        l->cursor = cur_pos;
        l->cur_word_idx = word_idx_from_cursor(l);
    }

}

// Delete action and different variations
void action_delete_word_forward(Line *l, Action *a) 
{
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        line_delete_word_at_cursor(l);
    }

}

void action_delete_word_backward(Line *l, Action *a)
{
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        line_delete_word_at_cursor(l);
        prev_word_start(l);
    }

}

void action_delete_forward(Line *l, Action *a)
{
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        line_delete_char_at_cursor(l);
    }

}

void action_delete_backward(Line *l, Action *a)
{
    size_t i;
    char *initial_cursor_pos = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        prev_char(l);
    }
    if (l->cursor < initial_cursor_pos) {
        for (i = 0; i < a->mov.count; i++) {
            line_delete_char_at_cursor(l);
        }
    }
}

// Helper functions

bool is_movement(char c)
{
    size_t keyval_len;
    keyval_len = sizeof(mapped_movements) / sizeof(KeyVal);
    
    size_t i;
    for (i = 0; i < keyval_len; i++) {
        if (c == mapped_movements[i].key) 
            return true;
    }
    return false;
}

bool is_action(char c)
{
    size_t keyval_len;
    keyval_len = sizeof(mapped_actions) / sizeof(KeyVal);
    
    size_t i;
    for (i = 0; i < keyval_len; i++) {
        if (c == mapped_actions[i].key) 
            return true;
    }
    return false;
}

int movement_get_value_for_key(char key)
{
    size_t keyval_len;
    keyval_len = sizeof(mapped_movements) / sizeof(KeyVal);
    
    size_t i;
    for (i = 0; i < keyval_len; i++) {
        if (key == mapped_movements[i].key) 
            return mapped_movements[i].value;
    }
    return 0;
}

int action_get_value_for_key(char key)
{
    size_t keyval_len;
    keyval_len = sizeof(mapped_actions) / sizeof(KeyVal);
    
    size_t i;
    for (i = 0; i < keyval_len; i++) {
        if (key == mapped_actions[i].key) 
            return mapped_actions[i].value;
    }
    return 0;
}

// Takes an string of action and identifies them which can be executed on a line
Action process_actions(char *action_str, size_t len)
{
    if (action_str == NULL) {
        ERROR("Action string is NULL");
        exit(-1);
    }

    Action a = {0};
    Movement m = {0};
    char num_buffer[10] = {0};
    size_t num_buffer_count = 0;
    bool found_action = false;
    bool found_movement = false;
    size_t i;
    for (i = 0; i < len; i++) {
        // If a number 
        if (action_str[i] >= '0' && action_str[i] <= '9') {
            num_buffer[num_buffer_count] = action_str[i];
            num_buffer_count++;
        }  
        if (found_action == false) {
            if (is_action(action_str[i])) {
                a.identifier = action_str[i];
                a.command = action_get_value_for_key(action_str[i]);
                found_action = true;
            }
        }  
        if (found_movement == false) {
            if (is_movement(action_str[i])) {
                m.identifier = action_str[i];
                m.command = movement_get_value_for_key(action_str[i]);
                found_movement = true;
            }
        } else if (found_movement) {
            m.arg = action_str[i];
        }
    }
    size_t count = (unsigned int)atoi(num_buffer);
    m.count = (count > 0) ? count : 1;
    a.mov = m;

    return a;
}


