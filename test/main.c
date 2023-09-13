#include <stdio.h>
#include <unistd.h>

extern void backtrace_symbols_fd_in(void *const *buffer, int size, int fd);

void func5() {
    void *buffer[1] = {func5};
    backtrace_symbols_fd_in(buffer, 1, STDOUT_FILENO);
    printf("This is func5\n");
}

void func4() {
    printf("This is func4\n");
    func5();
}

void func3() {
    printf("This is func3\n");
    func4();
}

void func2() {
    printf("This is func2\n");
    func3();
}

void func1() {
    printf("This is func1\n");
    func2();
}


extern void trace_begin() ;
extern void trace_end() ;

int main() {
	trace_begin() ;
    func1();
	trace_end() ;
    return 0;
}