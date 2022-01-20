#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "pager.h"
#include "test.h"


int main() {
    char *query = "select employee from employees";
    parse(query);
    open_file("db");
    test_write();
    //test_read();
}