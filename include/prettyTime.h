#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// strftime format
#define LOGGER_PRETTY_TIME_FORMAT "%d-%m-%Y %H:%M:%S"

// printf format
#define LOGGER_PRETTY_MS_FORMAT ".%03ld"

// Convert current time to milliseconds since unix epoch.
long to_ms(const struct timespec* tp) {
    return (tp->tv_sec * 1000) + (tp->tv_nsec / 1000000);
}

// Format it in two parts: main part with date and time and part with milliseconds.
inline char* pretty_time() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    time_t current_time = tp.tv_sec;

    // This function use static global pointer, so it's not thread safe solution.
    struct tm* time_info = localtime(&current_time);

    char buffer[128];
    int string_size = strftime(buffer, sizeof(buffer), LOGGER_PRETTY_TIME_FORMAT, time_info);
    long ms = to_ms(&tp) % 1000;
    string_size +=
        snprintf(buffer + string_size, sizeof(buffer) - string_size, LOGGER_PRETTY_MS_FORMAT, ms);

    char* result = malloc(string_size + 1);
    strncpy(result, buffer, string_size);
    result[string_size] = '\0';

    return result;
}