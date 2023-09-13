#include <sysdep.h>
#include <bfd.h>
#include <getopt.h>
#include <libiberty.h>
#include <demangle.h>
#include <bucomm.h>
#include <elf-bfd.h>


typedef struct
{
	char filename[256];
	int line_number;
	char function_name[256];
} BacktraceEntry;


void SymbolReslove(const char* symbol, char* filename, int* line_number, char* function_name);
void parseBacktraceSymbol(const char *symbol, BacktraceEntry *entry);