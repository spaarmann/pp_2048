CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

all: main

main: src/main.c src/renderer.c src/game.c
	$(CC) $(CFLAGS) -o 2048 src/main.c src/renderer.c src/game.c
	
clean:
	del 2048.exe
	
rebuild: clean main