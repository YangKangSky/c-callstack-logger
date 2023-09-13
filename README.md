# c-callstack-logger



## Building and running - legacy (Makefiles)

```
git clone https://github.com/YangKangSky/c-callstack-logger.git
cd c-callstack-logger

# Build with default logging
make
# or for extended logging you can play with these flags
make log_with_addr=1 log_not_demangled=1
# or to compile your application with disabled instrumentation (no logging)
make disable_instrumentation=1

# Build and Run (as the result trace.out file will be generated)
make run
```





# Others

clone from:https://github.com/TomaszAugustyn/call-stack-logger/tree/master



link: https://sii.pl/blog/en/call-stack-logger-function-instrumentation-as-a-way-to-trace-programs-flow-of-execution/?category=hard-development&tag=binutils-en,cpp-en,embedded-competency-center-en,function-instrumentation-en,gcc-en,logging-en,trace-en
