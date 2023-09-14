#include <stdio.h>
#include <stdlib.h>
#include <backtrace.h>

char** backtrace_symbols(void *const *buffer, int size) {
    struct backtrace_state *state = backtrace_create_state(NULL, 0, NULL, NULL);
    if (!state) {
        fprintf(stderr, "Failed to create backtrace state\n");
        return NULL;
    }

    char** symbols = (char**)malloc(sizeof(char*) * size);
    if (!symbols) {
        fprintf(stderr, "Failed to allocate memory for symbols\n");
        backtrace_free(state);
        return NULL;
    }

    int i;
    for (i = 0; i < size; ++i) {
        symbols[i] = NULL;
    }

    backtrace_full(state, 0, [](void *data, uintptr_t pc, const char *filename, int lineno, const char *function) -> int {
        char **symbols = static_cast<char**>(data);

        // Format the symbol string
        size_t symbol_len = snprintf(NULL, 0, "%s (%s:%d)", function, filename, lineno);
        char *symbol = (char *)malloc(symbol_len + 1);
        if (!symbol) {
            fprintf(stderr, "Failed to allocate memory for symbol\n");
            return 1;  // Stop backtrace on memory allocation failure
        }
        snprintf(symbol, symbol_len + 1, "%s (%s:%d)", function, filename, lineno);

        symbols[i] = symbol;

        return 0;  // Continue backtrace
    }, [](void *data, const char *msg, int errnum) {
        fprintf(stderr, "Backtrace error: %s (errno=%d)\n", msg, errnum);
    }, symbols);

    backtrace_free(state);

    return symbols;
}

void backtrace_symbols_fd(void *const *buffer, int size, int fd) {
    char** symbols = backtrace_symbols(buffer, size);
    if (symbols) {
        int i;
        for (i = 0; i < size; ++i) {
            dprintf(fd, "%s\n", symbols[i]);
            free(symbols[i]);
        }
        free(symbols);
    }
}

#if 0
int main() {
    void *buffer[10];
    int size = backtrace(buffer, 10);
    char **symbols = backtrace_symbols(buffer, size);

    if (symbols) {
        int i;
        for (i = 0; i < size; ++i) {
            printf("%s\n", symbols[i]);
            free(symbols[i]);
        }
        free(symbols);
    }

    return 0;
}
#endif