#include <stdio.h>
#include <malloc.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "reslove.h"

#include "cl_print.h"


/*for file io*/
#include <fcntl.h>
#include <unistd.h>

void __cyg_profile_func_exit(void *callee, void *callsite) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void *callee, void *callsite) __attribute__((no_instrument_function));



//static FILE *fd_trace;


int fd_trace = -1; // 文件描述符，-1 表示未打开文件

void __cyg_profile_func_enter(void *callee, void *callsite)
{
    char file_callee[256],file_callsite[256];
    int line_callee,line_callsite;
    char function_callee[256],function_callsite[256];
	
	char* env_store_path = NULL;

	if(fd_trace == -1)
	{
		env_store_path = getenv("C_CALLTRACE_LOGGER_PATH");
		if((env_store_path != NULL) && (!strcmp(env_store_path, "STDOUT")))
		{
			fd_trace = STDOUT_FILENO;
		}
		else if(env_store_path != NULL)
		{
			fd_trace = open(env_store_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}
		
		if (env_store_path == NULL)
		{
			fd_trace = open("trace.out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		}
	}


	if ((callee != NULL) && (callsite != NULL))
	{
		// fprintf(fd_trace, "enter %p\n", callee);	
		SymbolReslove(callee,file_callee, &line_callee, function_callee);

		SymbolReslove(callsite,file_callsite, &line_callsite, function_callsite);
		//printf("called from File: %s, Line: %d, Function: %s\n", file_callsite, line_callsite, function_callsite);
		cl_fprintf(fd_trace, "%s enter  (called from File: %s, Line: %d, Function: %s)\n", function_callee, file_callsite, line_callsite, function_callsite);
	}
}

void __cyg_profile_func_exit(void *callee, void *callsite)
{


    char file_callee[256],file_callsite[256];
    int line_callee,line_callsite;
    char function_callee[256],function_callsite[256];

	if ((callee != NULL) && (callsite != NULL))
	{
		// fprintf(fd_trace, "enter %p\n", callee);	
		SymbolReslove(callee,file_callee, &line_callee, function_callee);

		SymbolReslove(callsite,file_callsite, &line_callsite, function_callsite);
		//printf("called from File: %s, Line: %d, Function: %s\n", file_callsite, line_callsite, function_callsite);
		//fprintf(fd_trace, "%s exit  (called from File: %s, Line: %d, Function: %s)\n", function_callee, file_callsite, line_callsite, function_callsite);
		cl_fprintf(fd_trace, "%s exit  \n", function_callee);
	}

}

void trace_begin()
{
	// write to file
	fd_trace = fopen("trace.out", "w+");
	// write to terminal
	//fd_trace = stdout;
	
}

void trace_end()
{
	if (fd_trace != NULL)
	{
		fclose(fd_trace);
		fd_trace = NULL;
	}
}
