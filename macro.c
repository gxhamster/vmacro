#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "macro.h"
#include "actions.h"

// This files contains the implemantion for all the line functions


Line *global_line = NULL;

// This function should be called only once and before process_line
Line *init_global_line()
{
    global_line = (Line *)malloc(sizeof(Line));
    if (global_line == NULL) {
        ERROR("Cannot allocate memory for global line");
        exit(-1);
    }
    memset(global_line, 0, sizeof(Line));
    return global_line;
}

// Free all memory associated with a line
void free_line(Line *l)
{
    assert(l != NULL);
    memset(l->words, 0, MAX_WORDS_IN_LINE * sizeof(Word));
    free(l->src);
}

void pretty_print_line(Line *l)
{
    printf("|%s|\n", l->src);
    printf(" ");
    char *i;
    for (i = l->src; i != l->cursor; i++) {
        printf(" ");
    }
    printf("^\n");
}

// Check if ptr is pointing to inside the line
bool is_at_line(Line *l, char *ptr)
{
    IS_LINE_NULL(l, false);
    if (ptr >= l->src && ptr <= &l->src[l->len - 1]) {
        return true;
    }
    return false;
}

// Reverse version of strstr searches backward for target
// if target is found and is below or equal to limit it will return
// its pos
char *strstrr(const char *source, const char *target, const char *limit)
{
    const char *current;
    const char *found = NULL;

    if (*target == '\0')
        return (char *) source;

    size_t target_length = strlen(target);
    current = source + strlen(source) - target_length;

    while ( current >= source ) {
        found = strstr(current, target);
        if (found && found <= limit) {
            break;
        }
        current -= 1;
    }

    return (char *) found;
}

char *line_get_end_ptr(Line *l)
{
    IS_LINE_NULL(l, NULL);
    return &l->src[l->len - 1];
}

// Searches for str from cursor to end of line.
char *line_search_str(Line *l, char *str, size_t count)
{
    char *cur_pos = l->cursor;
    char *prev_result = NULL;
    char *result = NULL;
    size_t found = 0;
    size_t i;
    for (i = 0; i < count; i++) {
        result = strstr(cur_pos, str);
        if (result) {
            found++;
            cur_pos = result + 1;
            prev_result = result;
            if (found == count) {
                return result;
            }
            continue;
        }
        break;
    }
    if (prev_result)
        return prev_result;

    return NULL;
}

// Searches for str from cursor to end of line.
char *line_search_str_backward(Line *l, char *str, size_t count)
{
    char *cur_pos = l->cursor;
    char *prev_result = NULL;
    char *result = NULL;
    size_t found = 0;
    size_t i;
    for (i = 0; i < count; i++) {
        result = strstrr(l->src, str, cur_pos);
        if (result && result <= cur_pos) {
            found++;
            cur_pos = result - 1;
            prev_result = result;
            if (found == count) {
                return result;
            }
            continue;
        }
    }
    if (prev_result)
        return prev_result;

    return NULL;
}

// Searches the line for c from cursor to start
// if char not found return cursor pos
char *search_char_backward(Line *l, char c)
{
    IS_LINE_NULL(l, NULL);
    if (!is_at_line(l, l->cursor - 1)) {
        return l->cursor;
    }
    char *i;
    for (i = l->cursor - 1; i >= l->src; i--) {
        if (*i == c) {
            return i;
        }
    }
    return l->cursor;
}

// Searches the line for c from cursor
// if char not found return cursor pos
char *search_char_forward(Line *l, char c)
{
    IS_LINE_NULL(l, NULL);
    if (!is_at_line(l, l->cursor + 1)) {
        return l->cursor;
    }

    char *i;
    for (i = l->cursor + 1; i < &l->src[l->len]; i++) {
        if (*i == c) {
            return i;
        }
    }
    return l->cursor;
}

// Searches the line for c returns the postions before it
char *search_until_char_forward(Line *l, char c)
{
    IS_LINE_NULL(l, NULL);
    if (!is_at_line(l, l->cursor + 1)) {
        return l->cursor;
    }
    char *i;
    for (i = l->cursor + 1; i < &l->src[l->len]; i++) {
        if (*i == c) {
            return i - 1;
        }
    }
    return l->cursor;
}

// Searches the line backwards for c returns the postions after it
char *search_until_char_backward(Line *l, char c)
{
    IS_LINE_NULL(l, NULL);
    if (!is_at_line(l, l->cursor - 1)) {
        return l->cursor;
    }
    char *i;
    for (i = l->cursor - 1; i >= l->src; i--) {
        if (*i == c) {
            return i + 1;
        }
    }
    return l->cursor;
}

Line *set_cursor_at_start(Line *l)
{
    IS_LINE_NULL(l, NULL);
    l->cursor = l->src;
    l->cur_word_idx = word_idx_from_cursor(l);

    return l;
}

Line *set_cursor_at_end(Line *l)
{
    IS_LINE_NULL(l, NULL);
    l->cursor = line_get_end_ptr(l);
    l->cur_word_idx = word_idx_from_cursor(l);

    return l;
}

Line *prev_char(Line *l)
{
    if (l == NULL) {
        ERROR("Line does not exist");
        return NULL;
    }
    Word *word = &l->words[l->cur_word_idx];
    Word *word_prev = ((int)l->cur_word_idx - 1 >= 0) ? &l->words[l->cur_word_idx - 1] : NULL;
    bool is_word_prev = (word_prev == NULL) ? false : true;

    char *new_cursor_pos = l->cursor - 1;
    if (new_cursor_pos < &l->src[0]) {
        l->cursor = l->src;
        l->cur_word_idx = 0;
        return l;
    }

    if (is_word_prev) {
        if (new_cursor_pos < word->start && new_cursor_pos <= word_prev->end) {
            l->cur_word_idx -= 1;
        }
    }
    l->cursor -= 1;

    return l;
}

Line *next_char(Line *l)
{
    if (l == NULL) {
        ERROR("Line does not exist");
        return NULL;
    }

    Word *word = &l->words[l->cur_word_idx];
    Word *word_next = (l->cur_word_idx + 1 > l->n_words - 1) ? NULL : &l->words[l->cur_word_idx + 1];
    bool is_word_next = (word_next == NULL) ? false : true;

    char *next_cursor_pos = l->cursor + 1;
    char *line_end = line_get_end_ptr(l);
    if (next_cursor_pos > line_end) {
        l->cursor = line_end;
        l->cur_word_idx = l->n_words - 1;
        return l;
    }

    if (is_word_next) {
        if (next_cursor_pos > word->end && next_cursor_pos >= word_next->start) {
            l->cur_word_idx += 1;
        }
    }

    l->cursor += 1;

    return l;
}

// Move back to start of word
Line *prev_word_start(Line *l)
{
    // Because size_t is an unsigned int so cast to (int)
    if ((int)l->cur_word_idx - 1 < 0) {
        l->cursor = l->src;
        l->cur_word_idx = 0;
        return l;
    }
    char *start;
    // If cursor in current word move it to start of cur word
    Word *word = &l->words[l->cur_word_idx];
    if (l->cursor > word->start) {
        start = l->words[l->cur_word_idx].start;
        l->cursor = start;
        return l;
    }

    start = l->words[l->cur_word_idx - 1 ].start;
    l->cursor = start;
    l->cur_word_idx -= 1;

    return l;

}

// Move cursor end of
Line *prev_word_end(Line *l)
{
    // Because size_t is an unsigned int so cast to (int)
    if ((int)l->cur_word_idx - 1 < 0) {
        l->cursor = l->words[0].end;
        l->cur_word_idx = 0;
        return l;
    }
    char *end;
    // If cursor in current word move it to end of cur word
    Word *word = &l->words[l->cur_word_idx];
    if (l->cursor > word->end) {
        end = l->words[l->cur_word_idx].end;
        l->cursor = end;
        return l;
    }

    end = l->words[l->cur_word_idx - 1 ].end;
    l->cursor = end;
    l->cur_word_idx -= 1;

    return l;
}

// Move to end of word
Line *next_word_end(Line *l)
{

    Word *cur_word = &l->words[l->cur_word_idx];
    Word *next_word;
    // If cursor is not at end of word move to end of current word
    if (l->cursor < cur_word->end) {
        l->cursor = cur_word->end;
    } else {
        if (l->cur_word_idx + 1 > l->n_words - 1) {
            l->cursor = l->words[l->n_words - 1].end;
            l->cur_word_idx = l->n_words - 1;
            return l;
        }
        next_word = &l->words[l->cur_word_idx + 1];
        l->cursor = next_word->end;
        l->cur_word_idx += 1;
    }

    return l;
}

// Move cursor to next start of next word
// If cursor is in the word move it to end of cur word
Line *next_word_start(Line *l)
{
    if (l->cur_word_idx + 1 > l->n_words - 1) {
        if (is_at_line(l, l->cursor)) {
            return l;
        }
    }
    char *start = l->words[l->cur_word_idx+1].start;
    l->cursor = start;
    l->cur_word_idx += 1;

    return l;
}

// Get the word idx in the line from positon of cursor
// useful when calling any delete function
// return idx if found if not return 0
size_t word_idx_from_cursor(Line *l)
{
    IS_LINE_NULL(l, 0);

    size_t i;
    char *start;
    char *end;
    for (i = 0; i < l->n_words - 1; i++) {
        start = l->words[i].start;
        end = l->words[i+1].start;
        if (l->cursor >= start && l->cursor < end) {
            return i;
        }
    }
    // Handle last word
    start = l->words[l->n_words - 1].start;
    end = l->words[l->n_words - 1].end;
    if (l->cursor >= start && l->cursor <= end)
        return l->n_words - 1;

    // To handle when there is a space as the last char
    // Otherwise it would just return 0 as the index
    if (l->cursor > l->words[l->n_words - 1].end)
        return l->n_words - 1;
    else if (l->cursor < l->words[0].start)
        return 0;

    return l->cur_word_idx;
}


// Optimized
Line *line_delete_char_at_cursor_optimized(Line *l)
{
    IS_LINE_NULL(l, NULL);
    char *delete_char = l->cursor;
    char *last_char = line_get_end_ptr(l);
    size_t offset = l->cursor - l->src;
    size_t new_size;

    memmove(l->cursor, delete_char+1, last_char + 1 - delete_char);

    new_size = ((int)l->len - 1 < 0) ? 0 : l->len -1;

    l = process_line(l->src, new_size);
    free_line(l);

    l->cursor = l->src + offset;
    l->cur_word_idx = word_idx_from_cursor(l);

    return l;
}

Line *line_delete_char_at_cursor(Line *l)
{
    IS_LINE_NULL(l, NULL);

    char new_src[l->len + 1];
    size_t new_src_count = 0;
    char *delete_char = l->cursor;
    size_t i;
    for (i = 0; i < l->len; i++) {
        if (&l->src[i] != delete_char) {
            new_src[new_src_count] = l->src[i];
            new_src_count++;
        }
    }
    new_src[new_src_count] = '\0';
    size_t offset = l->cursor - l->src;
    free_line(l);
    l = process_line(new_src, new_src_count);


    l->cursor = l->src + offset;
    // If an empty line is at the cursor move back
    if (*l->cursor == '\0')
        prev_char(l);

    l->cur_word_idx = word_idx_from_cursor(l);
    return l;
}

// Delete chars from start to end includes start and end
Line *line_delete_range(Line *l, char *start, char *end)
{
    IS_LINE_NULL(l, NULL);
    // Check if start and end points to between src
    if (start < l->src || start > line_get_end_ptr(l)) {
        ERROR("start does not lie in the line");
        return NULL;
    }
    if (end < l->src || end > line_get_end_ptr(l)) {
        ERROR("end does not lie in the line");
        return NULL;
    }
    if (start == end) {
        // Somehow cursor moved forward in this scenario so cursor is set to start
        // This occured when trying to delete a word of len 1
        l->cursor = start;
        line_delete_char_at_cursor(l);
        return l;
    }

    // which is near to start of line (for easier pointer calc)
    char *near;
    char *far;
    near = (start - l->src < end - l->src) ? start : end;
    far = (start - l->src > end - l->src) ? start : end;

    size_t del_len = far - near + 1;
    char new_src[l->len - del_len + 1];
    size_t new_src_count = 0;
    size_t i;
    for (i = 0; i < l->len; i++) {
        if (&l->src[i] < near || &l->src[i] > far) {
            new_src[new_src_count] = l->src[i];
            new_src_count++;
        }
    }

    size_t offset = near - l->src;

    new_src[new_src_count] = '\0';
    free_line(l);
    l = process_line(new_src, new_src_count);
    l->cursor = l->src + offset;
    // If an empty line is at the cursor move back
    if (*l->cursor == '\0')
        prev_char(l);
    l->cur_word_idx = word_idx_from_cursor(l);
    return l;
}

// Insert a string at cursor
#define TEMP_BUF_LEN 200
void insert_at_cursor(Line *l, char *str, size_t str_len)
{
    char buf[TEMP_BUF_LEN] = {0};
    char *temp_buf = buf;
    // If cursor is at end of line insert after cursor
    int cur_offset = (l->cursor == line_get_end_ptr(l)) ? l->cursor - l->src + 1 : l->cursor - l->src;
    // Copy everything before cursor
    memcpy(temp_buf, l->src, cur_offset);
    // Copy insert text at cursor
    memcpy(temp_buf + cur_offset, str, str_len);
    char *last_pos_src = &l->src[l->len];
    // Copy whats left of src to temp_buf
    memcpy(temp_buf + cur_offset + str_len, l->src + cur_offset, last_pos_src - l->cursor + 1);

    // Create new line with new src
    size_t prev_word_idx = l->cur_word_idx;
    size_t new_len = strlen(buf);
    free_line(l);
    l = process_line(buf, new_len);

    l->cursor = l->src + cur_offset;
    if (is_delim_whitespace(*l->cursor)) {
        l->cur_word_idx = prev_word_idx;
    } else {
        l->cur_word_idx = word_idx_from_cursor(l);
    }
}
// Copy from start to end into buffer
char *line_copy_range(Line *l, char *start, char *end, char *buffer, size_t buffer_len)
{
    if (!start || !end) {
        ERROR("cannot copy, start or buffer does not exist");
        return NULL;
    }

    if (!is_at_line(l, start) || !is_at_line(l, end)) {
        ERROR("start or end does not lie on line");
        return NULL;
    }

    size_t copy_len = end - start + 1;
    if (buffer_len < copy_len) {
        ERROR("cannot copy, buffer too small");
        return NULL;
    }

    memcpy(buffer, start, copy_len);
    return buffer;
}

// Execute an action on a line
Line *eval_action_on_line(Line *l, Action *a)
{
    // After insert command is found no reason to continue
    // as everything after it is considered text
    switch (a->command) {
        case INSERT:
            action_insert_at_cursor(l, a);
            return l;
    }
    switch (a->command) {
        case PASTE:
            action_paste_at_cursor(l, a);
            return l;
        case PASTE_BACKWARD:
            action_paste_backward_at_cursor(l, a);
            return l;
    }

    switch (a->mov.command) {
        case FORWARD:
            movement_forward(l, a);
            break;
        case BACKWARD:
            movement_backward(l, a);
            break;
        case WORD_FORWARD:
            movement_word_forward(l, a);
            break;
        case WORD_BACKWARD:
            movement_word_backward(l, a);
            break;
        case LINE_START:
            movement_line_start(l, a);
            break;
        case LINE_END:
            movement_line_end(l, a);
            break;
        case FIND:
            movement_find(l, a);
            break;
        case FIND_BACKWARD:
            movement_find_backward(l, a);
            break;
        case TILL:
            movement_till(l, a);
            break;
        case TILL_BACKWARD:
            movement_till_backward(l, a);
            break;
        case SEARCH_FORWARD:
            movement_search(l, a);
            break;
        case SEARCH_BACKWARD:
            movement_search_backward(l, a);
            break;
        case MATCH:
            movement_match_pair(l, a);
            break;
        default:
            assert(0 && "Cannot identify the movement\n");
    }


    return l;
}

inline bool is_delim(char c)
{
    return !isalnum(c) || isspace(c);
}

inline bool is_delim_whitespace(char c)
{
    return (c == ' ' || c == '\t') ? true : false;
}

enum { WORD_START, IN_WORD, IN_DELIM };
Line *process_line(char *buf_src, size_t size)
{
    if (buf_src == NULL) {
        ERROR("Buffer is empty");
        exit(-1);
    }

    char *buf = malloc(size+1);
    memcpy(buf, buf_src, size + 1);

    Line *line = global_line;
    memset(line->words, 0, sizeof(Word) * MAX_WORDS_IN_LINE);
    line->len = size;
    line->cursor = buf;
    line->cur_word_idx = 0;
    line->n_words = 0;
    line->src = buf;

    Line *l = line;
    Word *w;
    int state = (is_delim(buf[0])) ? IN_DELIM : WORD_START;
    bool delim = false;
    bool whitespace = false;
    size_t i;
    for (i = 0; i < size+1; i++) {
        w = &l->words[l->n_words];
        delim = is_delim(buf[i]);
        whitespace = is_delim_whitespace(buf[i]);
        switch (state) {
            case IN_WORD:
                if (delim) {
                    state = IN_DELIM;
                    w->end = &buf[i - 1];
                    w->len = w->end - w->start + 1;
                    l->n_words++;
                    i--;
                } else if (buf[i] == '\0') {
                    w->end = &buf[i - 1];
                    w->len = w->end - w->start + 1;
                    l->n_words++;
                }
                break;
            case IN_DELIM:
                if (!delim) {
                    state = WORD_START;
                    i--;
                } else if (buf[i] == '\0') {
                    continue;
                } else if (!whitespace) {
                    w->start = &buf[i];
                    w->end = &buf[i];
                    w->len = 1;
                    l->n_words++;
                }
                break;
            case WORD_START:
                w->start = &buf[i];
                state = IN_WORD;
                break;
            default:
                continue;

        }
    }

    return line;

}
