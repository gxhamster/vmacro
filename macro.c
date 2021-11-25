#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "macro.h"

// Takes an string of action and identifies them which can be
// executed on a line
Action process_actions(char *action_str, size_t len)
{
    if (action_str == NULL) {
        ERROR("Action string is NULL");
        exit(-1);
    }

    Action a;
    Movement m;
    char num_buffer[10] = {0};
    size_t num_buffer_count = 0;
    size_t i;
    for (i = 0; i < len; i++) {
        printf("%c\n", action_str[i]); 
        switch (action_str[i]) {
            case 'd':
                a.identifier = 'd';
                a.command = DELETE;
                break;
            case 'w':
                m.identifier = 'w';
                m.command = WORD_FORWARD;
                break;
            case 'b':
                m.identifier = 'b';
                m.command = WORD_BACKWARD;
                break;
            case 'h':
                m.identifier = 'h';
                m.command = BACKWARD;
                break;
            case 'l':
                m.identifier = 'l';
                m.command = FORWARD;
                break;
            default:
                num_buffer[num_buffer_count] = action_str[i];
                num_buffer_count++;
                break;

        }
    }
    size_t count = (unsigned int)atoi(num_buffer);
    m.count = (count > 0) ? count : 0;
    a.mov = m;
    
    return a;
}

Line *prev_char(Line *l)
{
    if (l == NULL) {
        ERROR("Line does not exist");
        return NULL;
    }
    Word *word = &l->words[l->cur_word_idx];
    Word *word_prev = ((int)l->cur_word_idx + -1 > 0) ? NULL : &l->words[l->cur_word_idx - 1];
    bool is_word_prev = (word_prev == NULL) ? false : true;

    char *new_cursor_pos = l->cursor - 1;
    if (new_cursor_pos < &l->src[0]) {
        ERROR("Cursor reached start of line");
        return NULL;
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
    if (next_cursor_pos > &l->src[l->len - 1]) {
        ERROR("Cursor reached end of line");
        return NULL;
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
        ERROR("Reached start of words on line");
        return NULL;
    }
    char *start = l->words[l->cur_word_idx - 1 ].start;      
    l->cursor = start;
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
            ERROR("Reached end of words on line");
            return NULL;
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
        ERROR("Reached end of words on line");
        return NULL;
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
    for (i = 0; i < l->len; i++) {
        start = l->words[i].start;
        end = l->words[i].end;
        if (l->cursor >= start && l->cursor <= end) {
            return i;
        }
    }

    return 0;
}

// TODO: create function to delete a char from src
Line *line_delete_char_at_cursor(Line *l)
{
    if (l == NULL) {
        ERROR("Line is NULL");
        return NULL;
    }
    
    char new_src[l->len];
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
    Line l_new = process_line(new_src, new_src_count);
    *l = l_new;

    // TODO: Change the cur_word_idx if necessary (not sure)
    l->cursor = l->src + offset;
    l->cur_word_idx = word_idx_from_cursor(l); 
    return l;
}


// Delete a word in a line at a certain index
// This will also create a new src str the line points to
Line *line_delete_word_at_cursor(Line *l)
{

    if (l == NULL) {
        ERROR("Line is NULL");
        return NULL;
    }
    size_t word_idx = l->cur_word_idx;
    if (word_idx > l->n_words-1) {
        ERROR("Word index out of range");
        return NULL;
    }

    Word *word, *word_next;
    word = &l->words[word_idx];
    word_next = (word_idx+1 < l->n_words - 1) ? &l->words[word_idx+1] : NULL;
    
    bool is_last_word = (!word_next) ? true : false;

    char *start;
    char *end;
    // If in the word delete upto end
    if (l->cursor < word->end && l->cursor > word->start) {
        start = l->cursor;
        end = word->end;
    } else if (l->cursor == word->start) {
        start = word->start;
        end = word->end;
    } else if (l->cursor == word->end) {
        start = word->end;
        end = (!is_last_word) ? word_next->start - 1 : word->end;
    } else {
        ERROR("Cursor is not at this word");
        return NULL;
    }
    
    size_t del_len = end - start + 1;
    char new_src[l->len - del_len];
    size_t new_src_count = 0;
    
    if (!is_last_word) {
        end = word_next->start - 1;
    }

    size_t i;
    for (i = 0; i < l->len; i++) {
        if (&l->src[i] < start || &l->src[i] > end) {
            new_src[new_src_count] = l->src[i];
            new_src_count++;
        }
    }
    new_src[new_src_count] = '\0';
    // Offset to move cursor to point word was deleted
    char *cursor_pos = l->cursor;
    char *src = l->src;
    size_t offset = cursor_pos - src - 1;

    // Reproduce the line structure becaus src is changed
    Line l_new = process_line(new_src, new_src_count);
    *l = l_new;

    // FIXME: cur_word_idx should be set here (oops)
    l->cursor = l->src + offset;
    l->cur_word_idx = word_idx_from_cursor(l); 
    return l;
}

// Execute an action on a line
Line *eval_action_on_line(Line *l, Action *a)
{
    // TODO: Fixed this if else block mess
    // Only a movement
    if (a->identifier == 0) {
        if (a->mov.command == FORWARD) {
            size_t i;
            for (i = 0; i < a->mov.count; i++) {
                next_char(l);
            }
        } else if (a->mov.command == BACKWARD) {
            size_t i;
            for (i = 0; i < a->mov.count; i++) {
                prev_char(l);
            }
        } else if (a->mov.command == WORD_FORWARD) {
            size_t i;
            for (i = 0; i < a->mov.count; i++) {
                next_word_start(l);
            }
        } else if (a->mov.command == WORD_BACKWARD) {
            size_t i;
            for (i = 0; i < a->mov.count; i++) {
                prev_word_start(l);
            }
        }
    } else if (a->command > 0) {
        if (a->command == DELETE) {
            if (a->mov.command == WORD_FORWARD) {
                size_t i;
                for (i = 0; i < a->mov.count; i++) {
                    line_delete_word_at_cursor(l);
                    next_word_start(l);
                }
            } else if (a->mov.command == WORD_BACKWARD) {
                size_t i;
                for (i = 0; i < a->mov.count; i++) {
                    line_delete_word_at_cursor(l);
                    prev_word_start(l);
                }

            }
        }

    }
    return l;
}

// Turn a line to Line structure
// Also seperate the words
Line process_line(char *buf_src, size_t size)
{
    if (buf_src == NULL) {
        ERROR("Buffer is empty");
        exit(-1);
    }
    // Seperate the words in the line
    char *buf = malloc(size+1);
    strcpy(buf, buf_src);

    Line line;
    line.len = size;
    line.cursor = buf;
    line.cur_word_idx = 0;
    line.n_words = 0;


    // TODO: implement way to find words without using strtok
    char *token;
    size_t token_size;
    const char *delim = "-.\t ";
    token = strtok(buf, delim);
    while (token != NULL) {
        // printf("%p %s\n", token, token);
        token_size = strlen(token);
        Word word;
        word.start = token;
        word.end = token + token_size - 1;
        word.len = token_size;
        line.words[line.n_words] = word;
        line.n_words++;
        token = strtok(NULL, delim);
    }
    // Change the src to buf
    strcpy(buf, buf_src);
    line.src = buf;

    return line;
}

// Read from text from file
#define MAX_BUF_READ 100
char *read_from_file(FILE *fp)
{
    if (fp == NULL) {
        ERROR("File handler is not valid");
        exit(-1);
    }
    char buf[MAX_BUF_READ];
    while (fgets(buf, MAX_BUF_READ, fp) != NULL) {
        buf[strcspn(buf, "\n")] = 0;
        process_line(buf, strlen(buf));
    }

    fclose(fp);
    return NULL;
}


