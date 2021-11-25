#include <stdio.h>
#include <string.h>
#include "macro.h"


int main()
{
    char *str = "hello world again it's me Mario";
    Line l = process_line(str, strlen(str));   
    next_word_start(&l);
    line_delete_word_at_cursor(&l);
    printf("Line: %s\n", l.src);

}

//int main1(int argc, char **argv)
//{
//    //FILE *fp = fopen("test.txt", "r");
//    //read_from_file(fp);
//    Action a = process_actions("2w", 2);
//    Action b = process_actions("1b", 2);
//    char *str = "hello world again???";
//    Line l = process_line(str, strlen(str));   
//    eval_action_on_line(&l, &a);
//    eval_action_on_line(&l, &b);
//}
