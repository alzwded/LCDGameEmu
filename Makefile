VERSION = 0.6.5
LGEVERSION = '"'$(VERSION)'"'
YACC = bison
YACCOPTS = --graph -k -v
MAINOBJ = main.o
EXE = lcdgameemu
OBJS = parser.o code.o log.o cmdargs.o machine.o vector.o \
	stack.o interpreter.o window.o console_viewer.o inputbit_impl.o \
    cmdargs_impl.o
CC = gcc
COPTS = -g -c -I. -Wall -DLGEVERSION=$(LGEVERSION) `pkg-config --libs --cflags sdl`
LD = gcc
LDOPTS = `pkg-config --libs --cflags sdl`
TMP = /tmp

$(EXE): $(MAINOBJ) $(OBJS)
	$(LD) -o $(EXE) $(MAINOBJ) $(OBJS) $(LDOPTS)

%.o: %.c
	$(CC) $(COPTS) -o $@ $<

parser.c: parser.y
	$(YACC)  -o parser.c  $(YACCOPTS) -- parser.y

clean:
	rm -rf parser.dot parser.output parser.c *.o $(EXE) trace log lcdgameemu.log lcdgameemu.trace delivery/

all: clean $(EXE)

delivery: $(EXE)
	mkdir -p $(TMP)/LCDGameEmu-$(VERSION)
	rm -rf $(TMP)/LCDGameEmu-$(VERSION)/*
	tar cv . -X tar.excludes | tar xv -C $(TMP)/LCDGameEmu-$(VERSION)
	mkdir -p delivery
	tar cvjf delivery/LCDGameEmu-$(VERSION).tbz -C $(TMP) LCDGameEmu-$(VERSION)
	#tar cv -C $(TMP) LCDGameEmu-$(VERSION).tbz | tar xv -C delivery/
	rm -rf $(TMP)/LCDGameEmu-$(VERSION) #$(TMP)/LCDGameEmu-$(VERSION).tbz

.PHONY: clean
.PHONY: all
.PHONY: delivery
