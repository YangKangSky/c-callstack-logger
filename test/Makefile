#
# Copyright © 2020-2023 Tomasz Augustyn
# All rights reserved.
#
# Project Name: Call Stack Logger
# GitHub: https://github.com/TomaszAugustyn/call-stack-logger
# Contact Email: t.augustyn@poczta.fm

# Find C++ Standard Library header files to exclude them from instrumentation
#CPP_STD_INCLUDES=$(shell ( 	`gcc -print-prog-name=cc1plus` -v < /dev/null 2>&1 \
#								| LC_ALL=C sed -ne '/starts here/,/End of/p' \
#								| grep -o '/[^"]*' ; \
#							`gcc -print-prog-name=cpp` -v < /dev/null 2>&1 \
#								| LC_ALL=C sed -ne '/starts here/,/End of/p' \
#								| grep -o '/[^"]*' ) \
#						| cat | sort | uniq | tr '\n' ',' | sed 's/\(.*\),/\1 /' | xargs )
#
# Exclude tracing functions themselves from instrumentation
#CPP_STD_INCLUDES := "${CPP_STD_INCLUDES},include/callStack.h,include/unwinder.h,include/types.h,include/format.h,include/prettyTime.h"
#$(info $$CPP_STD_INCLUDES is [${CPP_STD_INCLUDES}])




CC ?= cc



CFLAGS = -g -Wall -rdynamic -finstrument-functions 
#CFLAGS += -finstrument-functions-exclude-function-list=sym,sym,...
CFLAGS += -finstrument-functions-exclude-function-list=main
#CFLAGS += -finstrument-functions-exclude-file-list=$(CPP_STD_INCLUDES)



INCS = -I$(INC_DIR) -L$(LIB_DIR)
LIBS = -L${BUILD_DIR} -l:libcallstacklogger.a  -lbacktrace
#LIBS += -ldl -lbfd 

#SOURCES := $(shell find . -name "*.c" -printf '%f\n')

SOURCES := main.c
_HEADERS  := $(shell find $(INC_DIR) -name "*.h" -printf '%f\n')

HEADERS = $(patsubst %,$(INC_DIR)/%,$(_HEADERS))

OBJS := $(patsubst %.c,%.o,$(SOURCES))

EXECUTABLE = $(BUILD_DIR)/runDemo

ifdef log_with_addr
	CFLAGS += -DLOG_ADDR
endif
ifdef log_not_demangled
	CFLAGS += -DLOG_NOT_DEMANGLED
endif
ifdef disable_instrumentation
	CFLAGS = -g -Wall  -DDISABLE_INSTRUMENTATION
endif


.PHONY: build
build: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCS)

run:
	$(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJS)

cleanAll:
	rm -f $(ODIR)/*
