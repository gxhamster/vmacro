#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ERROR(s) fprintf(stderr, "ERROR: %s\n", s)

// All actions and movements
enum { DELETE = 1, CHANGE, MOVE, WORD_FORWARD, WORD_BACKWARD }; 
typedef struct {
    char identifier;
    int command;
    size_t count;
} Movement;

// d2w -> delete two words
typedef struct {
    char identifier;
    int command;
    Movement mov; 
} Action;


typedef struct { char *start; char *end;
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
Line *line_delete_word(Line *l, size_t word_idx);
Line *eval_action_on_line(Line *l, Action *a);
char *read_from_file(FILE *fp);


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
            case 'm':
                a.identifier = 'm';
                a.command = MOVE;
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


// Delete a word in a line at a certain index
// This will also create a new src str the line points to
Line *line_delete_word(Line *l, size_t word_idx)
{
    if (l == NULL) {
        ERROR("Line is NULL");
        return NULL;
    }
    if (word_idx > l->n_words-1 || word_idx < 0) {
        ERROR("Word index out of range");
        return NULL;
    }
    
    Word *word, *word_next;
    word = &l->words[word_idx];
    word_next = (word_idx+1 < l->n_words - 1) ? &l->words[word_idx+1] : NULL;


    // hello world again???
    char new_src[l->len - word->len];
    size_t new_src_count = 0;
    char *p;
    size_t i;
    for (i = 0; i < l->len; i++) {
        p = &l->src[i];
        if (word_next == NULL) {
            // If it is last word
            if (p < word->start || p > word->end) {
                new_src[new_src_count] = *p;
                new_src_count++;
            }
        } else if (p < word->start || p >= word_next->start) {
            // Only copy from this word to start of next word
            new_src[new_src_count] = *p;
            new_src_count++;
        } 
    }
    new_src[new_src_count] = '\0';
    word->start = NULL;
    word->end = NULL;
    word->len = 0;
    
    // Reproduce the line structure becaus src is changed
    Line l_new = process_line(new_src, new_src_count);
    *l = l_new;

    return l;
}

Line *eval_action_on_line(Line *l, Action *a)
{
    // No action to perform (just a movement)
    if (a->command == 0 && a->mov.command == WORD_FORWARD) {
        l->cur_word_idx += (a->mov.count + l->cur_word_idx > l->n_words) ? l->n_words-1 : a->mov.count; 
        l->cursor = l->words[l->cur_word_idx].start;
    } else if (a->command == 0 && a->mov.command == WORD_BACKWARD) {
        l->cur_word_idx -= (l->cur_word_idx - a->mov.count < 0 ) ? 0 : a->mov.count; 
        l->cursor = l->words[l->cur_word_idx].start;
    }
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
    line.cursor = buf_src;
    line.cur_word_idx = 0;
    line.n_words = 0;

    char *token;
    size_t token_size;
    const char *delim = "-.\t ";
    // FIXME: Pointer in the words no longer point to src
    // because we duplicated it 
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

int main()
{
    char *str = "hello world again it's me Mario";
    Line l = process_line(str, strlen(str));   
    line_delete_word(&l, 5);
    line_delete_word(&l, 1);
    line_delete_word(&l, 0);
    printf("Line: %s\n", l.src);

}

int main1(int argc, char **argv)
{
    //FILE *fp = fopen("test.txt", "r");
    //read_from_file(fp);
    Action a = process_actions("2w", 2);
    Action b = process_actions("1b", 2);
    char *str = "hello world again???";
    Line l = process_line(str, strlen(str));   
    eval_action_on_line(&l, &a);
    eval_action_on_line(&l, &b);
}
