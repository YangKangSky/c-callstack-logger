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

CC?= cc
AR?= ar
#CFLAGS += -finstrument-functions-exclude-file-list=$(CPP_STD_INCLUDES)
#CFLAGS = -g -Wall -rdynamic -finstrument-functions 

IDIR = ../include
ODIR = ../build
LIB_DIR = ../lib
INCS = -I$(IDIR) -L$(LIB_DIR)
LIBS = -ldl -lbfd

LIB_STATIC = $(ODIR)/libcallstacklogger.a

SOURCES := $(shell find . -name "*.c" -printf '%f\n')
_HEADERS  := $(shell find $(IDIR) -name "*.h" -printf '%f\n')

HEADERS = $(patsubst %,$(IDIR)/%,$(_HEADERS))

_OBJS = $(SOURCES:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

ifdef log_with_addr
	CFLAGS += -DLOG_ADDR
endif
ifdef log_not_demangled
	CFLAGS += -DLOG_NOT_DEMANGLED
endif
ifdef disable_instrumentation
	CFLAGS = -g -Wall  -DDISABLE_INSTRUMENTATION
endif

CFLAGS_DISINST = -g -Wall  -DDISABLE_INSTRUMENTATION

.PHONY: build
build: clean $(LIB_STATIC) 

$(LIB_STATIC): $(OBJS)
	- rm -f $(LIB_STATIC)
	$(AR) rcs  $(LIB_STATIC) $(OBJS)

$(ODIR)/backtrace-symbols.o : backtrace-symbols.c $(HEADERS)
	$(CC) $(CFLAGS_DISINST) -c $< -o $@ $(INCS)

$(ODIR)/trace.o : trace.c $(HEADERS)
	$(CC) $(CFLAGS_DISINST) -c $< -o $@ $(INCS)

$(ODIR)/%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCS)




.PHONY: clean
clean:
	rm -f  $(OBJS)

