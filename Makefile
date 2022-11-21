# Sample Makefile to compile C programs
FILE = hw4
CC = gcc
CFLAGS = -Wall -o #replace -g with -O when not debugging




build: $(FILE).c
	$(CC) queue.c  $(FILE).c -o $(FILE) -lm -lpthread

db:
	gbd -tui $(FILE)

run: 
	./$(FILE) queue.c $(FILE).c -o $@ $^