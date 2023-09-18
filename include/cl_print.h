

#include <sys/types.h>
#include <sys/param.h>

#if defined(USE_LONG_LONG)
typedef unsigned long long	ef_number;
#else
typedef unsigned long		ef_number;
#endif


void cl_printf(const char * pattern, ...);

int cl_vsnprintf(char *pcBuf, unsigned long ulSize, const char *pcString, va_list vaArgP);


int cl_sprintf(char *pcBuf, const char *pcString, ...);

int cl_snprintf(char *pcBuf, unsigned long ulSize, const char *pcString, ...);

int cl_fprintf(int fd, const char *format, ...);







		   