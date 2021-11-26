#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "macro.h"

typedef struct {
    char **actions_str;
    char *file_name;
    size_t num_of_actions;
} Args;


void free_args(Args *args)
{
    free(args->actions_str);
    args->file_name = NULL;
}

#define MAX_BUF_READ 100
static char *read_from_file(FILE *fp, Args *args)
{
    if (fp == NULL) {
        ERROR("File handler is not valid");
        exit(-1);
    }

    char buf[MAX_BUF_READ];
    size_t i;
    while (fgets(buf, MAX_BUF_READ, fp) != NULL) {
        buf[strcspn(buf, "\n")] = 0;
        Line l = process_line(buf, strlen(buf));
        for (i = 0; i < args->num_of_actions; i++) {
            Action a = process_actions(args->actions_str[i], strlen(args->actions_str[i]));
            eval_action_on_line(&l, &a);
        }
        printf("|%s|\n", l.cursor);
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

#define MAX_ACTIONS 10
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
    char c;
    // FIXME: values from optarg might get overwritten
    while ((c = getopt(argc, argv, ":d:m:f:")) != -1) {
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
        ERROR("file name is missing");
        print_help();
        exit(-1);
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
    
    Args args = {actions_str, file_name, actions_str_count};
    return args;

}

int main(int argc, char **argv)
{
    //Args args = handle_args(argc, argv);
    //FILE *fp = fopen(args.file_name, "r");
    //read_from_file(fp, &args);
    //free_args(&args);
    
    char *str = "hello world again";
    Line l = process_line(str, strlen(str));
    set_cursor_at_end(&l);
    Action a = process_actions("2fo", 3);
    eval_action_on_line(&l, &a);

    return 0;
}

