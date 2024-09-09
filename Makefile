SRC = main.c
CC = gcc

all: main

debug: $(SRC)
	$(CC) --std=c99 -g $(SRC) -lm

main: $(SRC)
	$(CC) -O2 --std=c99 -Wall -Werror $(SRC) -lm

testing: $(SRC)
	$(CC) -O2 $(SRC) -lm
