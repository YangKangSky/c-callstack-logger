#ifndef RESLOVE_SYMBOLS_H

#if 0
#include <execinfo.h>
#else
#include "backtrace.h"
#endif


#include <signal.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void SymbolReslove(const void *addr, char* filename, int* line_number, char* function_name) ;

#endif /*RESLOVE_SYMBOLS_H */