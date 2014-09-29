#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "LR0_parser.h"

int main(void)
{
    FILE *gfp, *sfp;
    gfp = fopen("/tmp/byyl/test5_grammar2.in", "r");
    sfp = fopen("/tmp/byyl/test5_src4.in", "r");
    printf("input grammar from file:%s\n", "test5_grammar.in");
    input_grammar(gfp);
    puts("grammar:");
    print_grammar();
    puts("making items");
    make_all_items();
    puts("items:");
    print_items();
    puts("making DFA");
    make_DFA();
    puts("DFA:");
    print_DFA();
    puts("making parsing table");
    make_parsing_table();
    puts("parsing table:");
    print_parsing_table();
    printf("start to parse file:%s", "test5_src.in\n");
    LR0_parser(sfp, 1);
    fclose(gfp);
    fclose(sfp);
    return 0;
}
