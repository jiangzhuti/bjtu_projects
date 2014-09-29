#ifndef LR0_PARSER_H
#define LR0_PARSER_H

#include <stdio.h>

int input_grammar(FILE *fp);
void print_grammar();
int make_all_items();
void print_items();
void make_DFA();
void print_DFA();
void make_parsing_table();
void print_parsing_table();
void LR0_parser(FILE *fp, int show_detail);
void clean_up();
#endif // LR0_PARSER_H
