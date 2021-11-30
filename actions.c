#include <string.h>
#include <assert.h>
#include "actions.h"

// This file defines what each vim action and movement does. 

// Please add any new movement or action here too
const KeyVal mapped_actions[] = {
    {'d', DELETE},
    {'i', INSERT},
};

const KeyVal mapped_movements[] = {
    {'l', FORWARD},
    {'h', BACKWARD},
    {'w', WORD_FORWARD},
    {'b', WORD_BACKWARD},
    {'^', LINE_START},
    {'$', LINE_END},
    {'f', FIND},
    {'F', FIND_BACKWARD},
    {'t', TILL},
    {'T', TILL_BACKWARD}
};

// Movements
void movement_forward(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_forward(l, a);
        return;
    }
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        next_char(l);
    }
}

void movement_backward(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_backward(l, a);
        return;
    }
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        prev_char(l);
    }
}

void movement_word_forward(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_word_forward(l, a);
        return;
    }
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        next_word_start(l);
    }
}

void movement_word_backward(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_word_backward(l, a);
        return;
    }
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        prev_word_start(l);
    }

}

void movement_line_start(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_to_line_start(l, a);
        return;
    }
    set_cursor_at_start(l);      
}

void movement_line_end(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_to_line_end(l, a);
        return;
    }
    set_cursor_at_end(l);

}

void movement_find(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_to_find(l, a);
        return;
    }
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

void movement_find_backward(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_to_find_backward(l, a);
        return;
    }
    size_t i;
    char *cur_pos;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        cur_pos = search_char_backward(l, a->mov.arg);
        l->cursor = cur_pos;
        l->cur_word_idx = word_idx_from_cursor(l);
    }

}

void movement_till(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_till(l, a);
        return;
    }
    char *cur_pos;
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        cur_pos = search_until_char_forward(l, a->mov.arg);
        l->cursor = cur_pos;
        l->cur_word_idx = word_idx_from_cursor(l);
    }
}

void movement_till_backward(Line *l, Action *a)
{
    if (a->command == DELETE) {
        action_delete_till_backward(l, a);
        return;
    }
    char *cur_pos;
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        cur_pos = search_until_char_backward(l, a->mov.arg);
        l->cursor = cur_pos;
        l->cur_word_idx = word_idx_from_cursor(l);
    }
}

// Delete action and different variations
void action_delete_word_forward(Line *l, Action *a) 
{
    size_t i;
    char *start, *end;
    start = l->cursor;
    end = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        next_word_start(l);
    }
    end = (l->cursor - 1 > start) ? l->cursor - 1 : l->cursor;
    line_delete_range(l, start, end);
}

void action_delete_word_backward(Line *l, Action *a)
{
    char *start, *end;
    end = l->cursor - 1;
    start = l->cursor;
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        prev_word_start(l);
    }
    start = l->cursor;
    line_delete_range(l, start, end);
}

#define SINGLE_CHAR_DEL_LIMIT 1
void action_delete_forward(Line *l, Action *a)
{
    // For optimization reasons
    if (a->mov.count > SINGLE_CHAR_DEL_LIMIT) {
        line_delete_range(l, l->cursor, l->cursor + a->mov.count - 1); 
        return;
    }
    size_t i;
    for (i = 0; i < a->mov.count; i++) {
        line_delete_char_at_cursor(l);
    }

}

void action_delete_backward(Line *l, Action *a)
{
    if (a->mov.count > SINGLE_CHAR_DEL_LIMIT) {
        line_delete_range(l, l->cursor - a->mov.count - 1, l->cursor - 1); 
        return;
    }
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

void action_delete_to_line_end(Line *l, Action *a)
{
    (void) a;
    line_delete_range(l, l->cursor, line_get_end_ptr(l));
    prev_char(l);
}

void action_delete_to_line_start(Line *l, Action *a)
{
    (void) a;
    prev_char(l);
    line_delete_range(l, l->cursor, l->src);
}

void action_delete_to_find(Line *l, Action *a)
{
    char *start, *end;
    size_t i;
    start = l->cursor;
    end = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        end = search_char_forward(l, a->mov.arg);
        l->cursor = end;
        l->cur_word_idx = word_idx_from_cursor(l);
    }
    if (start == end)
        return;
    line_delete_range(l, start, end);
}

void action_delete_to_find_backward(Line *l, Action *a)
{
    char *start, *end;
    size_t i;
    end = l->cursor;
    start = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        start = search_char_backward(l, a->mov.arg);
        l->cursor = start;
        l->cur_word_idx = word_idx_from_cursor(l);
    }
    if (start == end)
        return;
    line_delete_range(l, start, end);
}

void action_delete_till(Line *l, Action *a)
{
    char *start, *end;
    size_t i;
    start = l->cursor;
    end = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        end = search_until_char_forward(l, a->mov.arg);
        l->cursor = end;
        l->cur_word_idx = word_idx_from_cursor(l);
    }
    if (start == end)
        return;
    line_delete_range(l, start, end);
}

void action_delete_till_backward(Line *l, Action *a)
{
    char *start, *end;
    size_t i;
    end = l->cursor;
    start = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        if (a->mov.arg == 0)
            continue;
        start = search_until_char_backward(l, a->mov.arg);
        l->cursor = end;
        l->cur_word_idx = word_idx_from_cursor(l);
    }
    if (start == end)
        return;
    line_delete_range(l, start, end);
}

// Insert action function
#define TEMP_BUF_LEN 200
void action_insert_at_cursor(Line *l, Action *a)
{
    char buf[TEMP_BUF_LEN] = {0};
    char *temp_buf = buf;
    int cur_offset = l->cursor - l->src;
    // Copy everything before cursor
    memcpy(temp_buf, l->src, cur_offset);
    // Copy insert text at cursor
    memcpy(temp_buf + cur_offset, a->ins.text, a->ins.len);
    char *last_pos_src = &l->src[l->len];
    // Copy whats left of src to temp_buf
    memcpy(temp_buf + cur_offset + a->ins.len, l->src + cur_offset, last_pos_src - l->cursor + 1);
    
    // Create new line with new src
    size_t prev_word_idx = l->cur_word_idx;
    size_t new_len = strlen(buf);
    free_line(l);
    Line l_new = process_line(buf, new_len);
    *l = l_new;
    // FIXME: If we insert a whitespace using insert
    // word_idx_from_cursor will return 0
    l->cursor = l->src + cur_offset;
    if (is_delim_whitespace(*l->cursor)) {
        l->cur_word_idx = prev_word_idx;
    } else {
        l->cur_word_idx = word_idx_from_cursor(l);
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
    memset(a.ins.text, 0, MAX_INSERT_LEN);
    char num_buffer[10] = {0};
    char *insert_buffer = a.ins.text;
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
                // If an insert identifier is found
                if (action_get_value_for_key(action_str[i]) == INSERT) {
                    a.identifier = action_str[i];
                    a.command = INSERT;
                    i++;
                    a.ins.len = len - i;
                    assert(a.ins.len < MAX_INSERT_LEN);
                    memcpy(insert_buffer, &action_str[i], len - i + 1);
                    return a; 
                }
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


