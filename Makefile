.PHONY: all clean

all: uni-shell

clean: 
	rm main.o parser.o uni-shell

uni-shell: main.o parser.o
	gcc -o uni-shell -Wall -fsanitize=address -g main.o parser.o

main.o: main.c
	gcc -o main.o -Wall -fsanitize=address -g -c main.c

parser.o: parser.c
	gcc -o parser.o -Wall -fsanitize=address -g -c parser.c
