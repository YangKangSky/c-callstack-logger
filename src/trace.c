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

void  __attribute__((optimize("0"))) __cyg_profile_func_enter(void *callee, void *callsite)
{
    char file_callee[MAX_FILENAME_LENGTH],file_callsite[MAX_FILENAME_LENGTH];
    int line_callee,line_callsite;
    char function_callee[MAX_FUNCTION_NAME_LENGTH],function_callsite[MAX_FUNCTION_NAME_LENGTH];
	
	char* env_store_path = NULL;
	char* env_debug = NULL;


	env_debug = getenv("C_CALLTRACE_LOGGER_DEBUG");
	if((env_debug != NULL) && (!strcmp(env_debug, "y")))
	{
		cl_printf("__cyg_profile_func_enter in\n");
	}	

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

void __attribute__((optimize("0"))) __cyg_profile_func_exit(void *callee, void *callsite) 
{


    char file_callee[MAX_FILENAME_LENGTH],file_callsite[MAX_FILENAME_LENGTH];
    int line_callee,line_callsite;
    char function_callee[MAX_FUNCTION_NAME_LENGTH],function_callsite[MAX_FUNCTION_NAME_LENGTH];

	char* env_debug = NULL;


	env_debug = getenv("C_CALLTRACE_LOGGER_DEBUG");
	if((env_debug != NULL) && (!strcmp(env_debug, "y")))
	{
		cl_printf("__cyg_profile_func_exit in\n");
	}	

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
