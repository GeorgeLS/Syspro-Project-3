#ifndef EXERCISE_III_REPORT_UTILS_H
#define EXERCISE_III_REPORT_UTILS_H

#include "attributes.h"

__FORMAT__(printf, 1, 2)
void die(const char *fmt, ...);

__FORMAT__(printf, 1, 2)
void report_error(const char *fmt, ...);

__FORMAT__(printf, 1, 2)
void report_response(const char *fmt, ...);

#endif //EXERCISE_III_REPORT_UTILS_H
