#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 4

void drawBoard(uint8_t board[SIZE][SIZE]) {
	uint8_t x,y;
	for (y=0;y<SIZE;y++) {
		for (x=0;x<SIZE;x++) {
		printf("\n");
		for (x=0;x<SIZE;x++) {
			if (board[x][y]!=0) {
				char s[8];
				sprintf(s,8,"%u",(uint32_t)1<<board[x][y]);
				uint8_t t = 7-strlen(s);
				printf("%*s%s%*s",t-t/2,"",s,t/2,"");
			} else {
				printf("   ·   ");
		}

uint8_t findTarget(uint8_t array[], int arrSize, uint8_t x,uint8_t stop) {
	uint8_t t;// if the position is already on the first, don't evaluate
	if (x==0) {
		return x;
	}
	for(t=x-1;t>=0;t--) {
		if (array[t]!=0) {
			if (array[t]!=array[x]) {// merge is not possible, take next position
				return t+1;
			}
			return t;
		} else {	// we should not slide further, return this one
			if (t==stop) {
				return t;
			}
		}
	}// we did not find a
	return x;
}

bool slideArray(uint8_t array[SIZE]) {
	bool success = false;
	uint8_t x,t,stop=0;

	for (x=0;x<SIZE;x++) {
		if (array[x]!=0) {
			t = findTarget(array,x,stop);// if target is not original position, then move or merge
			if (t!=x) {// if target is zero, this is a move
				if (array[t]==0) {
					array[t]=array[x];
				} else if (array[t]==array[x]) {// merge (increase power of two)
					array[t]++;	// set stop to avoid double merge
					stop = t+1;
				}
				array[x]=0;
				success = true;
			}
		}
	}
	return success;
}

void moveBoard(uint8_t board[SIZE][SIZE]) {
	uint8_t i,j,n=SIZE;
	uint8_t tmp;
	for (i=0; i<n/2; i++) {
		for (j=i; j<n-i-1; j++) {
			tmp = board[i][j];
			board[i][j] = board[j][n-i-1];
			board[j][n-i-1] = board[n-i-1][n-j-1];
			board[n-i-1][n-j-1] = board[n-j-1][i];
			board[n-j-1][i] = tmp;
		}
	}
}

bool moveUp(uint8_t board[SIZE][SIZE]) {
	bool success = false;
	uint8_t x;
	for (x=0;x<SIZE;x++) {
		success |= slideArray(board[x]);
	}
	return success;
}

bool moveLeft(uint8_t board[SIZE][SIZE]) {
	bool success;
	moveBoard(board);
	success = moveUp(board);
	moveBoard(board);
	moveBoard(board);
	moveBoard(board);
	return success;
}

bool moveDown(uint8_t board[SIZE][SIZE]) {
	bool success;
	moveBoard(board);
	moveBoard(board);
	success = moveUp(board);
	moveBoard(board);
	moveBoard(board);
	return success;
}

bool moveRight(uint8_t board[SIZE][SIZE]) {
	bool success;
	moveBoard(board);
	moveBoard(board);
	moveBoard(board);
	success = moveUp(board);
	moveBoard(board);
	return success;
}

bool findPairDown(uint8_t board[SIZE][SIZE]) {
	bool success = false;
	uint8_t x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE-1;y++) {
			if (board[x][y]==board[x][y+1]) return true;
		}
	}
	return success;
}

uint8_t countEmpty(uint8_t board[SIZE][SIZE]) {
	uint8_t x,y;
	uint8_t count=0;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				count++;
			}
		}
	}
	return count;
}
