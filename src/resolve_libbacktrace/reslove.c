


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <backtrace.h>
#include <backtrace-supported.h>

#include "cl_print.h"

//#include "testlib.h"

/* Test the backtrace function with non-inlined functions.  */

//void *state;
/* The number of failures.  */

//int failures;

struct FunctionInfo
{
  char filename[MAX_FILENAME_LENGTH];
  int lineno;
  char function[MAX_FUNCTION_NAME_LENGTH];
};

#define MAX_SUBSTRINGS 8
#define MAX_OUTPUT_SIZE 256

const char *substrings[MAX_SUBSTRINGS] = {
    "aarch64-rdk-linux",
    "all-rdkmllib32-linux",
    "armv7ahf-neon-rdkmllib32-linux-gnueabi",
    "armv7at2hf-neon-rdkmllib32-linux-gnueabi",
    "mesonsc2_lib32_ah212_hy44q_rdk-rdk-linux",
    "mesonsc2_lib32_ah212_hy44q_rdk-rdkmllib32-linux-gnueabi",
	"c-callstack-logger",
    "workspace"
};

void strip_substring(char *output, const char *input) {
    int i;
    for (i = 0; i < MAX_SUBSTRINGS; i++) {
        if (strstr(input, substrings[i]) != NULL) {
            const char *substring_ptr = strstr(input, substrings[i]);
            strncpy(output, substring_ptr + strlen(substrings[i]) + 1, MAX_FILENAME_LENGTH);
            return;
        }
    }

    // 没有匹配的子字符串，将输出设置为输入
    strcpy(output, input);
}


int
bp_callback (void *vdata, uintptr_t pc ,
	      const char *filename, int lineno, const char *function)
{
  struct FunctionInfo *p = (struct FunctionInfo *) vdata;
	char tmp_buf[MAX_FILENAME_LENGTH];


  if (filename == NULL)
    p->filename[0] = '\0';
  else
    {
	 memset(tmp_buf, 0x00, MAX_FILENAME_LENGTH);
	 strip_substring(tmp_buf, filename);
	  memcpy(p->filename, tmp_buf, strlen(tmp_buf));
    }
  p->lineno = lineno;
  if (function == NULL)
		p->function[0] = '\0';
  else
    {
      memset(tmp_buf, 0x00, MAX_FILENAME_LENGTH);
	  strip_substring(tmp_buf, function);
	  memcpy(p->function, tmp_buf, strlen(tmp_buf));
    }

    //printf("Symbol: %s\n", function);
    //printf("File: %s\n", filename);
    //printf("Line: %d\n", lineno);
    //printf("\n");
  return 0;
}




/* An error callback passed to backtrace.  */

void
bp_error_callback (void *vdata, const char *msg, int errnum)
{
    struct FunctionInfo *p = (struct FunctionInfo *) vdata;

  cl_fprintf (STDERR_FILENO , "%s", msg);
  if (errnum > 0)
    cl_fprintf (STDERR_FILENO , ": %s", strerror (errnum));
  cl_fprintf (STDERR_FILENO , "\n");

}


int get_function_info_by_address(uintptr_t buffer, struct FunctionInfo *info) {
    //void *buffer[10];
    //int size = backtrace(buffer, 10);
    struct backtrace_state *state = backtrace_create_state(NULL, 0, NULL, NULL);

    //backtrace_full(state, 1, print_symbol_info, NULL, buffer);
	int result = backtrace_pcinfo(state, buffer, bp_callback, bp_error_callback, info);
    //backtrace_free(state);
	
	//printf("Symbol-5: %s\n", info->function);
	//printf("File1-5: %s\n", info->filename);
	//printf("Line1-5: %d\n", info->lineno);
	
   return result;
}


void SymbolReslove(const void *addr, char* filename, int* line_number, char* function_name) {
    uintptr_t address = (uintptr_t)addr;
    struct FunctionInfo info;
    int result = get_function_info_by_address(address, &info);

    if (result == 0) {
        if (filename != NULL)
            cl_snprintf(filename, MAX_FILENAME_LENGTH, "%s", info.filename);
        if (line_number != NULL)
            *line_number = info.lineno;
        if (function_name != NULL)
            cl_snprintf(function_name, MAX_FUNCTION_NAME_LENGTH, "%s", info.function);
    } else {
        if (filename != NULL)
            filename[0] = '\0';
        if (line_number != NULL)
            *line_number = -1;
        if (function_name != NULL)
            function_name[0] = '\0';
    }
	//printf("File1: %s\n", filename);
	//printf("Line1: %d\n", *line_number);
	//printf("Function1: %s\n", function_name);
}


#if 0
int main() {
	FunctionInfo info;
	uintptr_t buffer = (uintptr_t)&main;
    get_function_info_by_address(buffer, &info);

    char filename[MAX_FILENAME_LENGTH];
    int line_number;
    char function_name[MAX_FUNCTION_NAME_LENGTH];

    SymbolReslove(buffer, filename, &line_number, function_name);

    return 0;
}

#endif