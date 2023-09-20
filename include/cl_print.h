

#include <sys/types.h>
#include <sys/param.h>

#if defined(USE_LONG_LONG)
typedef unsigned long long	ef_number;
#else
typedef unsigned long		ef_number;
#endif


#define MAX_FILENAME_LENGTH 1024
#define MAX_FUNCTION_NAME_LENGTH 1024
#define MAX_FPRINTF_BUF_LENGTH (MAX_FILENAME_LENGTH * 2)

void cl_printf(const char * pattern, ...);

int cl_vsnprintf(char *pcBuf, unsigned long ulSize, const char *pcString, va_list vaArgP);


int cl_sprintf(char *pcBuf, const char *pcString, ...);

int cl_snprintf(char *pcBuf, unsigned long ulSize, const char *pcString, ...);

int cl_fprintf(int fd, const char *format, ...);







		   
