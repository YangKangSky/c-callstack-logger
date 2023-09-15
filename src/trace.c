#if 1
#include <stdio.h>
#include <malloc.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "reslove.h"

void __cyg_profile_func_exit(void *callee, void *callsite) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void *callee, void *callsite) __attribute__((no_instrument_function));



static FILE *fp_trace;

void __cyg_profile_func_enter(void *callee, void *callsite)
{
    char file_callee[256],file_callsite[256];
    int line_callee,line_callsite;
    char function_callee[256],function_callsite[256];
	
	char* env_store_path = NULL;

	if(fp_trace == NULL)
	{
		env_store_path = getenv("C_CALLTRACE_LOGGER_PATH");
		if((env_store_path != NULL) && (!strcmp(env_store_path, "STDOUT")))
		{
			fp_trace = stdout;
		}
		else if(env_store_path != NULL)
		{
			fp_trace = fopen(env_store_path, "w+");
		}
		
		if (env_store_path == NULL)
		{
			fp_trace = fopen("trace.out", "w+");
		}
	}


	if ((callee != NULL) && (callsite != NULL))
	{
		// fprintf(fp_trace, "enter %p\n", callee);	
		SymbolReslove(callee,file_callee, &line_callee, function_callee);

		SymbolReslove(callsite,file_callsite, &line_callsite, function_callsite);
		//printf("called from File: %s, Line: %d, Function: %s\n", file_callsite, line_callsite, function_callsite);
		fprintf(fp_trace, "%s enter  (called from File: %s, Line: %d, Function: %s)\n", function_callee, file_callsite, line_callsite, function_callsite);
	}
}

void __cyg_profile_func_exit(void *callee, void *callsite)
{


    char file_callee[256],file_callsite[256];
    int line_callee,line_callsite;
    char function_callee[256],function_callsite[256];

	if ((callee != NULL) && (callsite != NULL))
	{
		// fprintf(fp_trace, "enter %p\n", callee);	
		SymbolReslove(callee,file_callee, &line_callee, function_callee);

		SymbolReslove(callsite,file_callsite, &line_callsite, function_callsite);
		//printf("called from File: %s, Line: %d, Function: %s\n", file_callsite, line_callsite, function_callsite);
		//fprintf(fp_trace, "%s exit  (called from File: %s, Line: %d, Function: %s)\n", function_callee, file_callsite, line_callsite, function_callsite);
		fprintf(fp_trace, "%s exit  \n", function_callee);
	}

}

void trace_begin()
{
	// write to file
	fp_trace = fopen("trace.out", "w+");
	// write to terminal
	//fp_trace = stdout;
	
}

void trace_end()
{
	if (fp_trace != NULL)
	{
		fclose(fp_trace);
		fp_trace = NULL;
	}
}
#else

#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void trace_begin()
{
}

void trace_end()
{
}

// this func is called when your program has func is called
// thiz is enter func address
// callsite is func back address
void __cyg_profile_func_enter(void *thiz, void *callsite) __attribute__((no_instrument_function));

void __cyg_profile_func_exit(void *thiz, void *callsite) __attribute__((no_instrument_function));

void display(void *enter, void *callsite, int flag) __attribute__((no_instrument_function));

void display(void *enter, void *callsite, int flag)
{
	Dl_info info;
	char cmd[1024] = "addr2line -e ";
	char *ptr = cmd + strlen(cmd);
	int strsize = readlink("/proc/self/exe", ptr, sizeof(cmd) - (ptr - cmd) - 1);

	if (strsize == -1)
	{
		fprintf(stderr, "readlink is failure!\n");
		exit(-1);
	}

	FILE *fp = popen(cmd, "w");

	if (!fp)
	{
		perror("popen\n");
		exit(-1);
	}

	if (dladdr(enter, &info) == 0)
	{
		fprintf(stderr, "addr to funcname is failure\n");
		exit(-1);
	}

	// flag=1,enter or flag=0,exit
	if (flag)
	{
		char address[256];
		printf("\n-------------------------------------------\n");
		printf("your program has entered a function!\n");
		printf("this run filename is %s\n", info.dli_fname);
		printf("this function name is %s\n", info.dli_sname);
		printf("this function's address is %p\n", enter);
		printf("this function back row is \n");
		sprintf(address, "%p\0", callsite);
		fwrite(address, 1, strlen(address) + 1, fp);
	}
	else
	{
		printf("\n-------------------------------------------\n");
		printf("your program will leave this function %s\n", info.dli_sname);
		printf("-------------------------------------------\n");
	}

	pclose(fp);
}

void __cyg_profile_func_enter(void *thiz, void *callsite)
{
	display(thiz, callsite, 1);
}

void __cyg_profile_func_exit(void *thiz, void *callsite)
{
	display(thiz, callsite, 0);
}

#endif