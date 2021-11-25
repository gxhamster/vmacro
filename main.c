#include <stdio.h>
#include <string.h>
#include "macro.h"


int main()
{
    char *str = "hello world again it's me Mario";
    Line l = process_line(str, strlen(str));   
    next_char(&l);
    next_char(&l);
    next_char(&l);
    next_char(&l);
    prev_char(&l);
    prev_char(&l);
    prev_char(&l);
    prev_char(&l);
    prev_char(&l);
    prev_char(&l);
    //Action a = process_actions("d4w", 3);
    //Action a1 = process_actions("1w", 2);
    //eval_action_on_line(&l, &a);
    //eval_action_on_line(&l, &a1);

    printf("Line: |%s|\n", l.cursor);

}

