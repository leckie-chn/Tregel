#ifndef _LOGGER_H
#define _LOGGER_H

#include <cstdio>
#define LOG(fmt, ...)    \
    printf(fmt, __VA_ARGS__);   \
    fflush(stdout)

#endif
