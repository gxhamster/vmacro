#include <string.h>
#include "actions.h"
// Yank actions

// Global Yank buffer
YankBuffer yank_buffer = {0};

void action_yank_forward(Line *l, Action *a)
{
    (void) a;
    size_t buf_len = (l->cursor + a->mov.count) - l->cursor;
    clear_yank_buffer();

    yank_buffer.len = buf_len;
    line_copy_range(l, l->cursor, l->cursor + a->mov.count - 1, yank_buffer.buf, buf_len); 
}

void action_yank_backward(Line *l, Action *a)
{
    (void) a;
    size_t buf_len = (l->cursor - 1) - (l->cursor - a->mov.count) + 1;
    clear_yank_buffer();

    yank_buffer.len = buf_len;
    line_copy_range(l, l->cursor - a->mov.count, l->cursor - 1, yank_buffer.buf, buf_len); 
}

void action_yank_word_forward(Line *l, Action *a) 
{
    size_t i;
    char *start, *end;
    size_t cur_idx = l->cur_word_idx;
    start = l->cursor;
    end = l->cursor;
    for (i = 0; i < a->mov.count; i++) {
        next_word_start(l);
    }


    if (l->cur_word_idx != l->n_words - 1 || cur_idx != l->cur_word_idx) {
        prev_word_end(l);
        end = l->cursor;
    } else {
        end = line_get_end_ptr(l);
    }

    size_t buf_len = end - start + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, start, end, yank_buffer.buf, yank_buffer.len);

    //Put cursor back at original pos
    l->cursor = start;
}

void action_yank_word_backward(Line *l, Action *a)
{
    size_t i;
    char *start, *end;
    start = l->cursor;
    end = l->cursor - 1;
    for (i = 0; i < a->mov.count; i++) {
        prev_word_start(l);
    }


    start = l->cursor;
    size_t buf_len = end - start + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, start, end, yank_buffer.buf, yank_buffer.len);

    //Put cursor back at original pos
    l->cursor = end + 1;
}

void action_yank_to_line_start(Line *l, Action *a)
{
    (void) a;
    char *end;
    char *cur_pos = l->cursor;
    prev_char(l);
    end = l->cursor;
    clear_yank_buffer();
    yank_buffer.len = end - l->src + 1;
    line_copy_range(l, l->src, end, yank_buffer.buf, yank_buffer.len);

    l->cursor = cur_pos;
}

void action_yank_to_line_end(Line *l, Action *a)
{
    (void) a;
    char *start;
    start = l->cursor;
    clear_yank_buffer();
    yank_buffer.len = line_get_end_ptr(l) - start + 1;
    line_copy_range(l, start, line_get_end_ptr(l), yank_buffer.buf, yank_buffer.len);
}

void action_yank_to_find(Line *l, Action *a)
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

    size_t buf_len = end - start + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, start, end, yank_buffer.buf, yank_buffer.len);
}

void action_yank_to_find_backward(Line *l, Action *a)
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

    size_t buf_len = end - start + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, start, end, yank_buffer.buf, yank_buffer.len);
}

void action_yank_till(Line *l, Action *a)
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

    size_t buf_len = end - start + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, start, end, yank_buffer.buf, yank_buffer.len);
}

void action_yank_till_backward(Line *l, Action *a)
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

    size_t buf_len = end - start + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, start, end, yank_buffer.buf, yank_buffer.len);
}

void action_yank_search(Line *l, Action *a)
{
    char *cur_pos;
    char *old_pos = l->cursor;
    if (a->mov.s_text.text[0] == '\0')
        return;
    next_char(l);
    cur_pos = line_search_str(l, a->mov.s_text.text, a->mov.count);
    cur_pos = (cur_pos) ? cur_pos : old_pos;

    if (cur_pos != old_pos) {
        size_t buffer_len = (cur_pos - 1) - old_pos + 1;
        clear_yank_buffer();
        yank_buffer.len = buffer_len;
        line_copy_range(l, old_pos, cur_pos - 1, yank_buffer.buf, yank_buffer.len);
    }
}

void action_yank_search_backward(Line *l, Action *a)
{
    char *cur_pos;
    char *old_pos = l->cursor;
    if (a->mov.s_text.text[0] == '\0')
        return;
    prev_char(l);
    cur_pos = line_search_str_backward(l, a->mov.s_text.text, a->mov.count);
    cur_pos = (cur_pos) ? cur_pos : old_pos;
    if (cur_pos != old_pos) {
        size_t buffer_len = old_pos - (cur_pos + 1) + 1;
        clear_yank_buffer();
        yank_buffer.len = buffer_len;
        line_copy_range(l, cur_pos + 1, old_pos, yank_buffer.buf, yank_buffer.len);
    }
}

void action_yank_match_pair(Line *l, Action *a) 
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

    // line_copy_range might have problems with 
    // calculation if start is bigger than end
    char *near, *far;
    near = (cur_pos < search_pos) ? cur_pos : search_pos;
    far = (cur_pos > search_pos) ? cur_pos : search_pos;

    size_t buf_len = far - near + 1;
    clear_yank_buffer();
    yank_buffer.len = buf_len;
    line_copy_range(l, near, far, yank_buffer.buf, yank_buffer.len);
}

// Paste action
void action_paste_at_cursor(Line *l, Action *a)
{
    (void) a;
    // Empty buffer
    if (yank_buffer.len == 0)
        return;

    next_char(l);
    insert_at_cursor(l, yank_buffer.buf, yank_buffer.len); 
}

void action_paste_backward_at_cursor(Line *l, Action *a)
{
    (void) a;
    // Empty buffer
    if (yank_buffer.len == 0)
        return;

    insert_at_cursor(l, yank_buffer.buf, yank_buffer.len); 
}

// Helper functions

void clear_yank_buffer()
{
    memset(yank_buffer.buf, 0, YANK_BUF_MAX);
    yank_buffer.len = 0;
}

