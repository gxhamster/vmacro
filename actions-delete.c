#include "actions.h"
// Delete action and different variations

void action_delete_word_forward(Line *l, Action *a) 
{
    size_t i;
    char *start, *end;
    size_t cur_idx = l->cur_word_idx;
    start = l->cursor;
    end = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        next_word_start(l);
    }
    // If it is the last word remove to last character
    // If it cursor moved to another word delete from start to before first 
    // character of the next word
    end = (l->cur_word_idx != l->n_words - 1 || cur_idx != l->cur_word_idx) ? l->cursor - 1 : line_get_end_ptr(l);
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

void action_delete_search(Line *l, Action *a)
{
    char *cur_pos;
    char *old_pos = l->cursor;
    if (a->mov.s_text.text[0] == '\0')
        return;
    next_char(l);
    cur_pos = line_search_str(l, a->mov.s_text.text, a->mov.count);
    cur_pos = (cur_pos) ? cur_pos : old_pos;
    if (cur_pos != old_pos) {
        l->cursor = cur_pos;
        l->cur_word_idx = word_idx_from_cursor(l);
        line_delete_range(l, old_pos, cur_pos - 1);
    }
}

void action_delete_search_backward(Line *l, Action *a)
{
    char *cur_pos;
    char *old_pos = l->cursor;
    if (a->mov.s_text.text[0] == '\0')
        return;
    prev_char(l);
    cur_pos = line_search_str_backward(l, a->mov.s_text.text, a->mov.count);
    cur_pos = (cur_pos) ? cur_pos : old_pos;
    if (cur_pos != old_pos) {
        l->cursor = cur_pos;
        l->cur_word_idx = word_idx_from_cursor(l);
        line_delete_range(l, cur_pos + 1, old_pos);
    }
}

void action_delete_match_pair(Line *l, Action *a)
{
    (void) a;
    char *cur_pos = l->cursor;
    char cur_char = (is_at_line(l, cur_pos)) ? *(cur_pos) : 0;

    char matching_char = get_matching_char(cur_char);
    if (matching_char == 0)
        return;

    char *search_pos;
    search_pos = (is_opening(cur_char)) ? search_char_forward(l, matching_char) : search_char_backward(l, matching_char);
     
    if (search_pos == cur_pos)
        return;

    line_delete_range(l, cur_pos, search_pos);
}
