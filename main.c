#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "pager.h"
#include "test.h"

/* TODO 
 * Add console input and fix a parser
 * Sequential index file
 * B+tree index file
 * Read multiple pages in to memory at once
 * Dirty bit for pages
 */
int main() {
    char *query = "select employee from employees";
    parse(query);
    open_file("db");
    test_binary_search();
    //test_read();
}