#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "report_utils.h"

static void __report(const char *tag, const char *fmt, va_list args) {
    if (tag) {
        fprintf(stderr, "%s", tag);
    }
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
}

void report_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __report("[ERROR]: ", fmt, args);
}

void report_response(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __report("[RESPONSE]: ", fmt, args);
}

void die(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    report_error(fmt, args);
    exit(EXIT_FAILURE);
}