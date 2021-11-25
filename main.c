#include <stdio.h>
#include <string.h>
#include "macro.h"


int main()
{
    char *str = "hello world again it's me Mario";
    Line l = process_line(str, strlen(str));   
    Action a = process_actions("d3w", 2);
    eval_action_on_line(&l, &a);

    printf("Line: %s\n", l.cursor);

}

