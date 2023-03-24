#include "../inc/util.h"

#include <cstdio>
#include <cstdlib>

void ErrIf(bool condition, const char *err_msg) {
    if (condition) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}