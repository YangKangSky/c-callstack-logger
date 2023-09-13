SUBDIRS = src test
ODIR = build
EXECUTABLE = $(ODIR)/runDemo
MKDIR_P = mkdir -p

.PHONY: all
all: makedirs subdirs

subdirs:
	for dir in $(SUBDIRS); do \
	   $(MAKE) -C $$dir; \
	done

makedirs: $(ODIR)

${ODIR}:
	${MKDIR_P} ${ODIR}

run:
	$(EXECUTABLE)

clean:
	rm -f $(ODIR)/* trace.out
