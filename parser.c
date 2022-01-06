#include <stdio.h>
#include <string.h>
#include "parser.h"


void get_sub_strings(char *query, char buffer[MAX_TOKENS][MAX_TOKEN_LEN]) {
    int i, j, cur_sub_str;
    i = 0;
    j = 0;
    cur_sub_str = 0;
    while (query[i] != '\0') {
        buffer[cur_sub_str][j] = query[i];
        if (query[i] == ' ') {
            cur_sub_str++;
            j = 0;
        }
        else
            j++;
        i++;
    }
}

void parse(char *query) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int i, j;
    
    for (i=0; i < MAX_TOKENS; i++) {
        for (j=0; j < MAX_TOKEN_LEN; j++) {
            tokens[i][j] = 0;
        }
    }

    get_sub_strings(query, tokens);
}