#
# Chose one of these mallocs:
MALLOC=sysalloc
#
# System allocator (recommended on modern macOS/Linux/FreeBSD).
# Uses libc malloc/free/realloc and avoids legacy sbrk/brk allocators.
#MALLOC=sysalloc
#
# Gnu malloc, fastest but uses most memory. Legacy code, not C23-clean.
#MALLOC=gmalloc
#
# Lars special malloc for LPmud. Uses least memory, but has an inefficient
# recombination scheme that may slow down the game after long uptimes.
# Uses brk/sbrk and is not suitable for modern macOS.
#MALLOC=malloc
#
# Define what random number generator to use.
# If no one is specified, a guaranteed bad one will be used.
#
RAND=RANDOM
YFLAGS=-d
#
# If you don't have 'strchr', then add next flag to CFLAGS.
# -Dstrchr=index
#
# If you don't have flex, replace '-DFLEX' with '-DLEX' below and
# redefine 'LEX=lex'.
LEX=lex
#LEX=flex
UNAME_S := $(shell uname -s)
CC ?= cc
PLATFORM_CPPFLAGS :=
ifeq ($(UNAME_S),Linux)
PLATFORM_CPPFLAGS += -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
endif
CPPFLAGS ?= -I. $(PLATFORM_CPPFLAGS)
BASE_CFLAGS ?= -std=c23 -DTRACE -D$(RAND) -O -DLEX -DMALLOC_$(MALLOC)
CFLAGS ?= $(BASE_CFLAGS) -Wno-error=implicit-function-declaration -Wno-error=implicit-int -Wno-error=return-type
LDFLAGS ?=
#
# Add extra libraries here.
#
CRYPT_LIB ?=
ifeq ($(UNAME_S),Linux)
CRYPT_LIB = -lcrypt
endif
LIBS ?= -lm $(CRYPT_LIB)

# CI hardening profiles.
CI_STRICT_CFLAGS ?= -Wall -Wextra -Wpedantic -Werror -Wno-sign-compare
CI_SANITIZE_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wno-sign-compare -Wno-deprecated-declarations
CI_ANALYZE_FLAGS ?= -std=c23 $(CPPFLAGS) -DTRACE -D$(RAND) -DLEX -DMALLOC_$(MALLOC) -Wno-sign-compare -Wno-deprecated-declarations
ANALYZE_SOURCES = $(filter %.c,$(SRC)) lang.c lexical.c comm2.c frontend.c count_active.c
#
# Add str.c here if you don't have memcpy() or strtol()
#
SRC=lang.y lexical.l main.c lnode.c interpret.c simulate.c object.c backend.c\
    comm1.c ed.c regexp.c wiz_list.c swap.c $(MALLOC).c string_space.c \
    call_out.c parse.c otable.c dumpstat.c stralloc.c
#
# Add str.o here if you don't have memcpy() or strtol()
#
OBJ=lang.o lexical.o main.o lnode.o interpret.o simulate.o object.o backend.o\
    comm1.o ed.o regexp.o wiz_list.o swap.o string_space.o $(MALLOC).o \
    call_out.o parse.o otable.o dumpstat.o stralloc.o

debug: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o debug $(LIBS)

parse: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o parse $(LIBS)

lint: *.c
	lint *.c

call_out.o: object.h interpret.h

swap.o: swap.c lnode.h object.h config.h interpret.h

lang.o: lang.c lnode.h config.h object.h interpret.h

lexical.o: lexical.c lnode.h config.h

lnode.o: lnode.h config.h

interpret.o: lnode.h interpret.h config.h object.h

simulate.o: interpret.h object.h config.h sent.h lnode.h wiz_list.h

wiz_list.o: wiz_list.h interpret.h

main.o: lnode.h config.h object.h interpret.h

clean:
	rm -f *.o y.tab.h lang.c lexical.c *.ln mon.out
	rm -f *.plist
	rm -f parse debug core frontend mudlib/core mudlib/debug.log TAGS
	rm -f lpmud.log
#	find . -name '*.i' -print -exec rm -f \{} \;
#	find . -name '*~' -print -exec rm -f \{} \;
#	find . -name '*#' -print -exec rm -f \{} \;
#	find . -name '*.orig' -print -exec rm -f \{} \;
#	find . -name '*.rej' -print -exec rm -f \{} \;

object.o: interpret.h object.h sent.h lnode.h config.h wiz_list.h

backend.o: object.h lnode.h config.h interpret.h wiz_list.h

comm1.o: comm.h sent.h interpret.h object.h config.h

comm2.o: config.h comm.h

frontend: comm2.o frontend.o
	$(CC) $(CFLAGS) $(LDFLAGS) comm2.o frontend.o -o frontend $(LIBS)

frontend.o: frontend.c config.h comm.h

ed.o: regexp.h object.h config.h interpret.h

regexp.o: regexp.h

otable.o: config.h object.h interpret.h

dumpstat.o: object.h interpret.h lnode.h

parse.o: lnode.h interpret.h config.h object.h wiz_list.h

stralloc.o: config.h

TAGS: $(SRC)
	etags $(SRC)

diff:
	make_diffs

count_active: count_active.o
	$(CC) $(CFLAGS) $(LDFLAGS) count_active.o -o count_active $(LIBS)

ci-warnings:
	$(MAKE) clean
	$(MAKE) parse count_active CFLAGS='$(BASE_CFLAGS) $(CI_STRICT_CFLAGS)'

ci-sanitize:
	$(MAKE) clean
	$(MAKE) parse count_active CFLAGS='$(BASE_CFLAGS) $(CI_SANITIZE_FLAGS)' LIBS='-lm -fsanitize=address,undefined'

ci-analyze:
	$(MAKE) clean
	$(MAKE) lang.c lexical.c
	@if command -v clang >/dev/null 2>&1; then \
		clang --analyze $(CI_ANALYZE_FLAGS) $(ANALYZE_SOURCES); \
	else \
		echo 'clang is required for ci-analyze'; \
		exit 1; \
	fi

ci: ci-warnings ci-sanitize ci-analyze

.PHONY: all clean lint diff debug parse frontend count_active \
	ci ci-warnings ci-sanitize ci-analyze

all: parse count_active
