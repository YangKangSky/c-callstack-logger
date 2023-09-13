LOCAL_PATH:=$(shell pwd)

ROOT_PATH = $(LOCAL_PATH)
BUILD_DIR := ${ROOT_PATH}/build
INC_DIR := ${ROOT_PATH}/include
LIB_DIR := ${ROOT_PATH}/lib

export ROOT_PATH
export BUILD_DIR
export INC_DIR
export LIB_DIR


SUBDIRS = src test
EXECUTABLE = $(BUILD_DIR)/runDemo
MKDIR_P = mkdir -p

.PHONY: all
all: makedirs subdirs

subdirs:
	for dir in $(SUBDIRS); do \
	   $(MAKE) -C $$dir; \
	done

makedirs: $(BUILD_DIR)

${BUILD_DIR}:
	${MKDIR_P} ${BUILD_DIR}

run:
	$(EXECUTABLE)

clean:
	for dir in $(SUBDIRS); do \
	   $(MAKE) -C $$dir clean; \
	done
	rm -f $(BUILD_DIR)/* trace.out
