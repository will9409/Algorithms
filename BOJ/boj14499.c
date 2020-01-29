#include <stdio.h>
#define SWAP(T, a, b, c, d) do {T tmp = a; a = b; b = c; c = d; d = tmp;}while(0)

typedef struct dice{
	int up;
	int north;
	int west;
	int east;
	int south;
	int bottom;
	int xloc;
	int yloc;
} DICE;

void diceInit(DICE *d1, int y, int x){
	d1->up = 0;
	d1->north = 0;
	d1->west = 0;
	d1->east = 0;
	d1->south = 0;
	d1->bottom = 0;
	d1->xloc = x;
	d1->yloc = y;
}


int diceRoll(DICE *d, int command){
	if( command == 1 ){
		SWAP(int, d->up, d->east, d->bottom, d->west);
		d->xloc++;
	} else if( command == 2 ){
		SWAP(int, d->up, d->west, d->bottom, d->east);
		d->xloc--;
	} else if( command == 3 ){
		SWAP(int, d->up, d->south, d->bottom, d->north);
		d->yloc--;
	} else if( command == 4 ){
		SWAP(int, d->up, d->north, d->bottom, d->south);
		d->yloc++;
	}

	return d->bottom;

}

void action(int m[][20], DICE *dice, int ymax, int xmax, int com){
	int x = dice->xloc;
	int y = dice->yloc;
	if(com == 1 && x+1 < xmax){
		if(m[y][x+1] == 0){
			m[y][x+1] = diceRoll(dice, com);
			printf("%d\n",dice->up);
		} else {
			diceRoll(dice, com);
			dice->bottom = m[y][x+1];
			m[y][x+1] = 0;
			printf("%d\n",dice->up);
		}
	} else if(com == 2 && x-1 > -1){
		if(m[y][x-1] == 0){
			m[y][x-1] = diceRoll(dice, com);
			printf("%d\n",dice->up);
		} else {
			diceRoll(dice, com);
			dice->bottom = m[y][x-1];
			m[y][x-1] = 0;
			printf("%d\n",dice->up);
		}
	} else if(com == 3 && y-1 > -1){
		if(m[y-1][x] == 0){
			m[y-1][x] = diceRoll(dice, com);
			printf("%d\n",dice->up);
		} else {
			diceRoll(dice, com);
			dice->bottom = m[y-1][x];
			m[y-1][x] = 0;
			printf("%d\n", dice->up);
		}
	} else if(com == 4 && y+1 < ymax){
		if(m[y+1][x] == 0){
			m[y+1][x] = diceRoll(dice, com);
			printf("%d\n",dice->up);
		} else {
			diceRoll(dice, com);
			dice->bottom = m[y+1][x];
			m[y+1][x] = 0;
			printf("%d\n", dice->up);
		}
	}
}


int main(){
	int map[20][20] = {0, };
	int command[1000] = {0, };
	int r, c;
	int N;
	int M;
	int x;
	int y;
	int K;

	scanf("%d %d %d %d %d", &N, &M, &y, &x, &K);
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			scanf("%d",&map[i][j]);
		}
	}

	for(int k = 0; k < K; k++){
		scanf("%d",&command[k]);
	}

	DICE dice1;
	diceInit(&dice1, y, x);

	for(int i = 0 ; i < K; i++){
		action(map, &dice1, N, M, command[i]);
	}


}
