#ifndef RESLOVE_BACKTRACE_SYMBOLS_H
#include <backtrace.h>
#include <backtrace-supported.h>
char **backtrace_symbols(void *const *buffer, int size);

void backtrace_symbols_fd(void *const *buffer, int size, int fd);

#endif /*RESLOVE_BACKTRACE_SYMBOLS_H */


