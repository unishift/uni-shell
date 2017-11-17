.PHONY: all clean

all: uni-shell

clean: 
	rm main.o utils.o inbuilt.o uni-shell

uni-shell: main.o utils.o inbuilt.o
	gcc -o uni-shell -Wall -fsanitize=address -g main.o utils.o inbuilt.o  

main.o: main.c
	gcc -o main.o -Wall -fsanitize=address -g -c main.c

utils.o: utils.c
	gcc -o utils.o -Wall -fsanitize=address -g -c utils.c

inbuilt.o: inbuilt.c
	gcc -o inbuilt.o -Wall -fsanitize=address -g -c inbuilt.c
