//#include <execinfo.h>
#include <signal.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void func5() {
    void *buffer[1] = {func5};
    //backtrace_symbols_fd(buffer, 1, STDOUT_FILENO);
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



int main() {
    func1();
    return 0;
}
