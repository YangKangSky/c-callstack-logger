#ifndef RESLOVE_SYMBOLS_H

#include <execinfo.h>
#include <signal.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	char filename[256];
	int line_number;
	char function_name[256];
} BacktraceEntry;


void SymbolReslove(const char* symbol, char* filename, int* line_number, char* function_name);
void parseBacktraceSymbol(const char *symbol, BacktraceEntry *entry);


#endif /*RESLOVE_SYMBOLS_H */