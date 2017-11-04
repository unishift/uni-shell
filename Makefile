.PHONY: all clean

all: uni-shell

uni-shell: main.o list.o parser.o
	gcc -o uni-shell -Wall -fsanitize=address -g main.o list.o parser.o

main.o: main.c
	gcc -o main.o -Wall -fsanitize=address -g -c main.c

list.o: list.c
	gcc -o list.o -Wall -fsanitize=address -g -c list.c

parser.o: parser.c
	gcc -o parser.o -Wall -fsanitize=address -g -c parser.c
