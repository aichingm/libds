.PHONY: default clean indent br cb cbr compile_commands.json

MAIN = bin/tests

SRCS = $(shell find ./ -name "*.c")
HDRS = $(shell find ./ -name "*.h")
OBJS = $(SRCS:.c=.o)

CC       := gcc
CFLAGS   := -std=gnu23 -pedantic -g -Wall -Wextra
LFLAGS   :=
INCLUDES := -I.
LIBS     :=


INDENT_STYLE := $(shell cat indent.pro | sed -e 's/^\s*#.*$$//g' | tr '\n' ' ' | sed -e 's/\s\s*/ /g' -e 's/^ //' -e 's/\s$$//')
TYPE_DEFS := $(shell ctags -x --c-types=t $(HDRS) $(SRCS) | awk '{print $$1}' | tr '\n ' ' ' | sed -e 's/^/ /' -e 's/ $$//' -e 's/ / -T /g')A
INDENT_CMD := VERSION_CONTROL=none indent

default: $(MAIN)

$(MAIN): $(OBJS)
	@mkdir -p $$(dirname $(MAIN))
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

tests/all-tests.o: tests/all-tests.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -c tests/all-tests.c  -o tests/all-tests.o

indent: $(SRCS) $(HDRS)
	@for f in $(HDRS); do \
		$(INDENT_CMD) $(INDENT_STYLE) $(TYPE_DEFS) $$f; \
		echo "" >> $$f;\
	done
	@for f in $(SRCS); do \
		$(INDENT_CMD) $(INDENT_STYLE) $(TYPE_DEFS) $$f; \
		echo "" >> $$f;\
	done

clean:
	rm -rf $(MAIN)
	rm -rf $(OBJS)

br: default
	./$(MAIN)

cb: clean default

cbr: clean default
	./$(MAIN)

compile_commands.json:
	make --always-make --dry-run | grep -wE 'gcc|g\+\+|c\+\+' | grep -w '\-c' | sed 's|cd.*.\&\&||g' | jq -nR '[inputs|{directory:"'`pwd`'", command:., file: (match(" [^ ]+$$").string[1:-1] + "c")}]' > compile_commands.json

