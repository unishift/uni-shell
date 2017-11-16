.PHONY: all clean

all: uni-shell

clean: 
	rm main.o utils.o uni-shell

uni-shell: main.o utils.o
	gcc -o uni-shell -Wall -fsanitize=address -g main.o utils.o

main.o: main.c
	gcc -o main.o -Wall -fsanitize=address -g -c main.c

utils.o: utils.c
	gcc -o utils.o -Wall -fsanitize=address -g -c utils.c
