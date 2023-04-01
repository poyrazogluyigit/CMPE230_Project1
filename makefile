AdvCalc: main.o lexer.o stacks.o
	gcc main.o lexer.o stacks.o -o AdvCalc
	rm *.o

main.o: main.c lexer.h stacks.h
	gcc -c main.c

lexer.o: lexer.c lexer.h
	gcc -c lexer.c

stacks.o: stacks.c stacks.h
	gcc -c stacks.c
