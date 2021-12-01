#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <unistd.h>
#include "macro.h"
#include "actions.h"

typedef struct {
    char **actions_str;
    char *file_name;
    size_t num_of_actions;
    bool pretty_print;
    bool stdout_input;
} Args;


void free_args(Args *args)
{
    free(args->actions_str);
    args->file_name = NULL;
}

#define MAX_BUF_READ 200
static char *read_from_file(FILE *fp, Args *args)
{
    // No file handler given read from stdin
    if (fp == NULL || args->stdout_input) {
        fp = stdin;
    }

    char buf[MAX_BUF_READ];
    size_t buf_len = 0;
    size_t i;
    while (fgets(buf, MAX_BUF_READ, fp) != NULL) {
        buf[strcspn(buf, "\n")] = 0;
        buf_len = strlen(buf);
        assert(buf_len < MAX_BUF_READ);
        Line l = process_line(buf, buf_len);
        for (i = 0; i < args->num_of_actions; i++) {
            Action a = process_actions(args->actions_str[i], strlen(args->actions_str[i]));
            eval_action_on_line(&l, &a);
        }
        if (args->pretty_print) 
            pretty_print_line(&l);
        else
            printf("%s\n", l.src);
        free_line(&l);
    }
    fclose(fp);
    return NULL;
}

static void print_help() 
{
    const char help[] = "Usage: macro -d [delimiter] -f [filename] -m [macro]";
    printf("%s\n", help);
}

#define MAX_ACTIONS 50
static Args handle_args(int argc, char **argv)
{
    if (argv == NULL) {
        ERROR("Arguments not given");
        print_help();
        exit(-1);
    }
    
    const char *delim = NULL;
    char *macro_str = NULL;
    char *file_name = NULL;
    bool pretty_print = false;
    bool stdout_input = false;
    char c;
    // FIXME: values from optarg might get overwritten
    while ((c = getopt(argc, argv, ":d:m:f:ps")) != -1) {
        switch (c) {
            case 'd':
                delim = optarg;
                // printf("Delim: %s\n", delim);
                break;
            case 'm':
                macro_str = optarg;
                // printf("Macro: %s\n", macro_str);
                break;
            case 'f':
                file_name = optarg;
                // printf("File: %s\n", file_name);
                break;
            case 'p':
                pretty_print = true;
                break;
            case 's':
                stdout_input = true;
                break;
            case ':':
                printf("ERROR: Option needs value\n");
                break;
            case '?':
                printf("ERROR: Unknown option: %c\n", optopt);
                break;
            default:
                continue;
        }
    }

    char **actions_str = (char **)malloc(sizeof(char **) * MAX_ACTIONS);
    size_t actions_str_count = 0;
    char *token;
    
    if (macro_str == NULL) {
        ERROR("macro is missing");
        print_help();
        exit(-1);
    }

    if (delim == NULL) {
        delim = "";
    }
    
    if (file_name == NULL) {
        stdout_input = true;
    }

    token = strtok(macro_str, delim);
    if (token == NULL) {
        ERROR("Cannot identify any macros, Check delimeter");
        print_help();
        exit(-1);
    }

    while (token != NULL) {
        actions_str[actions_str_count] = token;
        actions_str_count++;
        token = strtok(NULL, delim);
    }
    
    if (actions_str_count > MAX_ACTIONS) {
        ERROR("number of actions is greater than MAX_ACTIONS");
        exit(-1);
    }

    Args args = {actions_str, file_name, actions_str_count, pretty_print, stdout_input};
    return args;

}

int main(int argc, char **argv)
{
    Args args = handle_args(argc, argv);
    FILE *fp = fopen(args.file_name, "r");
    read_from_file(fp, &args);
    free_args(&args);
    
    //char *str = "hello-world--again";
    //Line l = process_line(str, strlen(str));
    //Action a = process_actions("2w", 3);
    //eval_action_on_line(&l, &a);

    return 0;
}

