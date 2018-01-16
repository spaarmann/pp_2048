CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -I.\lib\SDL\include
LDFLAGS = -L.\lib\SDL\lib\x64 -lSDL2main -lSDL2	-lSDL2_ttf


all: main

main: src/main.c src/common.c src/renderer.c src/game.c
	$(CC) $(CFLAGS) -o 2048.exe src\main.c src\common.c src\renderer.c src\game.c $(LDFLAGS)
	
clean:
	del 2048.exe
	
rebuild: clean main

run: main
	.\2048.exe