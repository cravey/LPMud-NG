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
# GNU Make defines CC=cc by default. Use clang unless the user explicitly
# provides CC (command line, environment, or another makefile include).
ifeq ($(origin CC),default)
CC := clang
endif
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
USE_SSL ?= 0
OPENSSL_ROOT ?=
ifeq ($(OPENSSL_ROOT),)
ifneq ("$(wildcard /opt/homebrew/opt/openssl@3/include/openssl/ssl.h)","")
OPENSSL_ROOT := /opt/homebrew/opt/openssl@3
else ifneq ("$(wildcard /usr/local/opt/openssl@3/include/openssl/ssl.h)","")
OPENSSL_ROOT := /usr/local/opt/openssl@3
else ifneq ("$(wildcard /opt/local/include/openssl/ssl.h)","")
OPENSSL_ROOT := /opt/local
endif
endif
SSL_CPPFLAGS :=
SSL_LIBS :=
ifeq ($(USE_SSL),1)
SSL_CPPFLAGS += -DUSE_SSL
ifneq ($(OPENSSL_ROOT),)
SSL_CPPFLAGS += -I$(OPENSSL_ROOT)/include
SSL_LIBS += -L$(OPENSSL_ROOT)/lib
endif
SSL_LIBS += -lssl -lcrypto
endif
CPPFLAGS += $(SSL_CPPFLAGS)
LIBS ?= -lm $(CRYPT_LIB) $(SSL_LIBS)
BUILD_MODE_FILE := .build_mode
BUILD_MODE_VALUE := USE_SSL=$(USE_SSL);OPENSSL_ROOT=$(OPENSSL_ROOT)

# CI hardening profiles.
CI_STRICT_CFLAGS ?= -Wall -Wextra -Wpedantic -Werror -Wno-sign-compare
CI_SANITIZE_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wno-sign-compare -Wno-deprecated-declarations
CI_ANALYZE_FLAGS ?= -std=c23 $(CPPFLAGS) -DTRACE -D$(RAND) -DLEX -DMALLOC_$(MALLOC) -Wno-sign-compare -Wno-deprecated-declarations
CLANG_TIDY ?= /opt/homebrew/opt/llvm/bin/clang-tidy
# Keep ci-tidy focused on actionable defects for this legacy codebase.
CI_TIDY_CHECKS ?= -checks=-*,bugprone-implicit-widening-of-multiplication-result,bugprone-misplaced-widening-cast,bugprone-unchecked-string-to-number-conversion,bugprone-unused-return-value,bugprone-signal-handler,clang-analyzer-*,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-clang-analyzer-security.insecureAPI.rand,-clang-analyzer-security.VAList
CI_TIDY_WARNINGS_AS_ERRORS ?= *
# Analyze only hand-maintained sources; yacc/lex output is generated and noisy.
ANALYZE_SOURCES = $(filter %.c,$(SRC)) comm2.c frontend.c count_active.c
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

debug: build-mode-check debug-internal

debug-internal: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o debug $(LIBS)

parse: build-mode-check parse-internal

parse-internal: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o parse $(LIBS)

.PHONY: build-mode-check

build-mode-check:
	@prev=""; \
	if [ -f $(BUILD_MODE_FILE) ]; then prev=$$(cat $(BUILD_MODE_FILE)); fi; \
	if [ "$$prev" != "$(BUILD_MODE_VALUE)" ]; then \
		if [ -n "$$prev" ]; then \
			echo "Build mode changed ($$prev -> $(BUILD_MODE_VALUE)); cleaning stale objects"; \
			rm -f *.o *.plist parse debug count_active frontend y.tab.h lang.c lexical.c; \
		fi; \
		echo "$(BUILD_MODE_VALUE)" > $(BUILD_MODE_FILE); \
	fi

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
	rm -f parse debug count_active core frontend mudlib/core mudlib/debug.log TAGS
	rm -f $(BUILD_MODE_FILE)
	rm -f lpmud.log
	find . -name '*.debug.log' -type f -print -exec rm -f \{} \;
	rm -rf tests/reports/*
	find . -name '*.i' -print -exec rm -f \{} \;
#	find . -name '*~' -print -exec rm -f \{} \;
#	find . -name '*#' -print -exec rm -f \{} \;
#	find . -name '*.orig' -print -exec rm -f \{} \;
#	find . -name '*.rej' -print -exec rm -f \{} \;

object.o: interpret.h object.h sent.h lnode.h config.h wiz_list.h

backend.o: object.h lnode.h config.h interpret.h wiz_list.h

comm1.o: comm.h sent.h interpret.h object.h config.h

comm2.o: config.h comm.h

frontend: build-mode-check comm2.o frontend.o
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

count_active: build-mode-check count_active.o
	$(CC) $(CFLAGS) $(LDFLAGS) count_active.o -o count_active $(LIBS)

ci-warnings:
	$(MAKE) clean
	$(MAKE) parse count_active CFLAGS='$(BASE_CFLAGS) $(CI_STRICT_CFLAGS)'

ci-sanitize:
	$(MAKE) clean
	$(MAKE) parse count_active CFLAGS='$(BASE_CFLAGS) $(CI_SANITIZE_FLAGS)' LIBS='-lm $(CRYPT_LIB) $(SSL_LIBS) -fsanitize=address,undefined'

ci-analyze:
	$(MAKE) clean
	$(MAKE) lang.c lexical.c
	@if command -v clang >/dev/null 2>&1; then \
		clang --analyze $(CI_ANALYZE_FLAGS) $(ANALYZE_SOURCES); \
	else \
		echo 'clang is required for ci-analyze'; \
		exit 1; \
	fi

ci-tidy:
	$(MAKE) clean
	$(MAKE) lang.c lexical.c
	@tidy_bin=""; \
	if [ -x "$(CLANG_TIDY)" ]; then \
		tidy_bin="$(CLANG_TIDY)"; \
	elif command -v clang-tidy >/dev/null 2>&1; then \
		tidy_bin=$$(command -v clang-tidy); \
	fi; \
	if [ -z "$$tidy_bin" ]; then \
		echo 'clang-tidy is required for ci-tidy'; \
		exit 1; \
	fi; \
	"$$tidy_bin" $(CI_TIDY_CHECKS) --warnings-as-errors='$(CI_TIDY_WARNINGS_AS_ERRORS)' \
		$(ANALYZE_SOURCES) -- $(CI_ANALYZE_FLAGS)

ci: ci-warnings ci-sanitize ci-analyze

mudlibtest:
	@set -e; \
	output="$$(./tests/run_test_mudlib_suite.sh)"; \
	printf '%s\n' "$$output"; \
	run_dir_rel="$$(printf '%s\n' "$$output" | sed -n 's/^==> Done: //p' | tail -n 1)"; \
	if [ -n "$$run_dir_rel" ] && [ -f "$$run_dir_rel/SUMMARY.txt" ]; then \
		echo ""; \
		echo "==> Mudlib test summary ($$run_dir_rel/SUMMARY.txt)"; \
		cat "$$run_dir_rel/SUMMARY.txt"; \
	fi

securitytest:
	@set -e; \
	output="$$(./tests/run_driver_security_suite.sh)"; \
	printf '%s\n' "$$output"; \
	run_dir_rel="$$(printf '%s\n' "$$output" | sed -n 's/^==> Done: //p' | tail -n 1)"; \
	if [ -n "$$run_dir_rel" ] && [ -f "$$run_dir_rel/SUMMARY.txt" ]; then \
		echo ""; \
		echo "==> Security suite summary ($$run_dir_rel/SUMMARY.txt)"; \
		cat "$$run_dir_rel/SUMMARY.txt"; \
	fi

.PHONY: all clean lint diff debug parse frontend count_active \
	ci ci-warnings ci-sanitize ci-analyze ci-tidy mudlibtest securitytest

all: parse count_active
