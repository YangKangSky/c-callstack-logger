#include <stdio.h>
#include <stdlib.h>
#include "reslove.h"

/*  main.c:12	func4() */
void parseBacktraceSymbol(const char *symbol, BacktraceEntry *entry)
{
	BacktraceEntry *info = entry; //(BacktraceInfo*)malloc(sizeof(BacktraceInfo));
	if (info == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	// 使用 sscanf 解析字符串
	// 注意: 这个解析能力取决于你的backtrace_symbols的输出格式是否总是遵循 "filename:line function" 这个格式。
	// 如果格式有所不同，你可能需要调整这个解析格式字符串
	int parsed = sscanf(symbol, "%[^:]:%d %s]", info->filename, &(info->line_number), info->function_name);

	if (parsed != 3)
	{
		fprintf(stderr, "Parsing failed\n");
		free(info);
	}

	char *openParenPos = strchr(info->function_name, '(');
	if (openParenPos != NULL)
	{
		*openParenPos = '\0';
	}

	// printf("File: %s, Line: %d, Function: %s\n", entry->filename, entry->line_number, entry->function_name);
}

void SymbolReslove(const char* symbol, char* filename, int* line_number, char* function_name) {
    BacktraceEntry entry;
    parseBacktraceSymbol(symbol, &entry);

    strncpy(filename, entry.filename, sizeof(entry.filename) - 1);
    filename[sizeof(entry.filename) - 1] = '\0';

    *line_number = entry.line_number;

    strncpy(function_name, entry.function_name, sizeof(entry.function_name) - 1);
    function_name[sizeof(entry.function_name) - 1] = '\0';
}