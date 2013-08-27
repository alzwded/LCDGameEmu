YACC = bison
YACCOPTS = --graph -k -v

parser.c parser.h: parser.y
	bison  -o parser.c  $(YACCOPTS) -- parser.y

clean:
	rm -f parser.dot parser.output parser.c
