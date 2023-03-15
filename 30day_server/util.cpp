#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void ErrIf(bool condition, const char *err_msg) {
    if (condition) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}