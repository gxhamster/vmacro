#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <unistd.h>
#include "macro.h"
#include "actions.h"

#define NAME vmacro
#define VERSION "0.1.2"
#define AUTHOR "gxhamster"

typedef struct {
    char **actions_str;
    char *file_name;
    size_t num_of_actions;
    int count;
    bool pretty_print;
    bool stdout_input;
} Args;


static void free_args(Args *args)
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

    Action actions_arr[args->num_of_actions];
    size_t k;
    for (k = 0; k < args->num_of_actions; k++) {
        actions_arr[k] = process_actions(args->actions_str[k], strlen(args->actions_str[k]));
    }

    char buf[MAX_BUF_READ];
    size_t buf_len = 0;
    Action *ap = NULL;
    size_t i;
    int j;
    while (fgets(buf, MAX_BUF_READ, fp) != NULL) {
        buf[strcspn(buf, "\n")] = 0;
        buf_len = strlen(buf);
        assert(buf_len < MAX_BUF_READ);
        Line l = process_line(buf, buf_len);

        for (j = 0; j < args->count; j++) {
            for (i = 0; i < args->num_of_actions; i++) {
                ap = &actions_arr[i];
                eval_action_on_line(&l, ap);
            }
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

static void print_version()
{
    printf("vmacro %s\n", VERSION);
}

static void print_help() 
{
    const char help_extended[] = "Execute vim commands on lines\n\
Example: vmacro -f test.txt -m '2w;dw;iHello' -d ';'\n\
If no file is given it will read from stdin\n\n \
-d      Delimeter to use when using multiple macros\n \
-f      Name of file to read from\n \
-c      Number of times to repeat macro\n \
-m      The macro to execute on each line\n";
    const char help[] = "Usage: vmacro -d [delimiter] -f [filename] -m [macro]";
    printf("%s\n%s", help, help_extended);
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
    char *repeat_count = NULL;
    bool pretty_print = false;
    bool stdout_input = false;
    char c;
    // FIXME: values from optarg might get overwritten
    while ((c = getopt(argc, argv, ":d:m:f:c:pshv")) != -1) {
        switch (c) {
            case 'd':
                delim = optarg;
                break;
            case 'm':
                macro_str = optarg;
                break;
            case 'f':
                file_name = optarg;
                break;
            case 'c':
                repeat_count = optarg;
                break;
            case 'p':
                pretty_print = true;
                break;
            case 's':
                stdout_input = true;
                break;
            case 'v':
                print_version();
                exit(-1);
            case 'h':
                print_help();
                exit(-1);
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

    int count = 1;
    if (repeat_count != NULL) {
        if ((count = atoi(repeat_count)) <= 0) {
            count = 1;
        } 
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

    Args args = {actions_str, file_name, actions_str_count, count, pretty_print, stdout_input};
    return args;

}

int main(int argc, char **argv)
{
    Args args = handle_args(argc, argv);
    FILE *fp = fopen(args.file_name, "r");
    read_from_file(fp, &args);
    free_args(&args);
    
    return 0;
}

