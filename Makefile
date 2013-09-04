LGEVERSION = '"0.1-pre"'
YACC = bison
YACCOPTS = --graph -k -v
MAINOBJ = main.o
EXE = lcdgameemu
OBJS = parser.o code.o log.o cmdargs.o machine.o vector.o \
	stack.o interpreter.o window.o console_viewer.o
CC = gcc
COPTS = -g -c -I. -Wall -DLGEVERSION=$(LGEVERSION) `pkg-config --libs --cflags sdl`
LD = gcc
LDOPTS = `pkg-config --libs --cflags sdl`

$(EXE): $(MAINOBJ) $(OBJS)
	$(LD) -o $(EXE) $(MAINOBJ) $(OBJS) $(LDOPTS)

%.o: %.c
	$(CC) $(COPTS) -o $@ $<

parser.c: parser.y
	$(YACC)  -o parser.c  $(YACCOPTS) -- parser.y

clean:
	rm -f parser.dot parser.output parser.c *.o $(EXE) trace log lcdgameemu.log lcdgameemu.trace

all: clean $(EXE)

.PHONY: clean
.PHONY: all
