#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "pager.h"
#include "test.h"

/* TODO 
 * Add console input and fix a parser
 */
int main() {
    char *query = "select employee from employees";
    parse(query);
    open_file("db");
    test_binary_search();
    //test_read();
}