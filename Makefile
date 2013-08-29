YACC = bison
YACCOPTS = --graph -k -v
MAINOBJ = main.o
EXE = lcdgameemu
OBJS = parser.o interfaces_impl.o log.o cmdargs.o
CC = gcc
COPTS = -g -c -I. -Wall
LD = gcc
LDOPTS = 

$(EXE): $(MAINOBJ) $(OBJS)
	$(LD) $(LDOPTS) -o $(EXE) $(MAINOBJ) $(OBJS)

%.o: %.c
	$(CC) $(COPTS) -o $@ $<

parser.c: parser.y
	$(YACC)  -o parser.c  $(YACCOPTS) -- parser.y

clean:
	rm -f parser.dot parser.output parser.c *.o $(EXE)
