#if 1
#include <stdio.h>
#include <malloc.h>
#include <execinfo.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void __cyg_profile_func_exit(void *callee, void *callsite) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void *callee, void *callsite) __attribute__((no_instrument_function));


extern char **backtrace_symbols_in(void *const *buffer, int size);
extern void SymbolReslove(const char* symbol, char* filename, int* line_number, char* function_name) ;


static FILE *fp_trace;

void __cyg_profile_func_enter(void *callee, void *callsite)
{

	char **strings;
    char file_callee[256],file_callsite[256];
    int line_callee,line_callsite;
    char function_callee[256],function_callsite[256];


	
	if ((fp_trace != NULL) && (callee != NULL))
	{

		// fprintf(fp_trace, "enter %p\n", callee);

		void *buffer[2] = {callee, callsite};

		strings = backtrace_symbols_in(buffer, 2);

		SymbolReslove(strings[0],file_callee, &line_callee, function_callee);

		SymbolReslove(strings[1],file_callsite, &line_callsite, function_callsite);
		//printf("called from File: %s, Line: %d, Function: %s\n", file_callsite, line_callsite, function_callsite);
		fprintf(fp_trace, "%s enter  (called from File: %s, Line: %d, Function: %s)\n", function_callee, file_callsite, line_callsite, function_callsite);

		//int j;
		//for (j = 0; j < 2; j++)
		//	printf("---%s\n", strings[j]);
		//fprintf(fp_trace, "enter %s %p(called from %s)  \n", strings[0], callee, strings[1]);
		free(strings);
	}
}

void __cyg_profile_func_exit(void *callee, void *callsite)
{
	char **strings;
    char file_callee[256],file_callsite[256];
    int line_callee,line_callsite;
    char function_callee[256],function_callsite[256];


	if ((fp_trace != NULL) && (callee != NULL))
	{
		// fprintf(fp_trace, "Exiting %p\n", callee);

		void *buffer[2] = {callee, callsite};

		strings = backtrace_symbols_in(buffer, 2);

		SymbolReslove(strings[0],file_callee, &line_callee, function_callee);

		SymbolReslove(strings[1],file_callsite, &line_callsite, function_callsite);
		// for (j = 0; j < 1; j++)
		//	printf("---%s\n", strings[j]);
		//fprintf(fp_trace, "exit %s %p(called from %s)  \n", strings[0], callee, strings[1]);

		fprintf(fp_trace, "%s exit  \n", function_callee);
		free(strings);
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