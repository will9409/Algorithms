#include <stdio.h>
#include <stdlib.h>
#define size 100

typedef struct head{
	int towards; // 0 == up, 1 == right, 2 == down, 3 == left
	int t;
	int x;
	int y;
	struct node *headp;
	struct node *tailp;
} SHEAD;

typedef struct node{
	int x;
	int y;
	struct node *rlink;
	struct node *llink;
} SBODY;

struct node *createBody(SHEAD *head, int y, int x){
	struct node *nodep;
	if( (nodep = (struct node *)malloc(sizeof(*nodep))) == NULL )
		printf("malloc failed.\n");

	nodep->x = x;
	nodep->y = y;
	nodep->rlink = NULL;
	nodep->llink = NULL;

	return nodep;
}

void remove_tail(SHEAD *head, int map[][size]){
	if(head->tailp != NULL){
	struct node *nodep = head->tailp;
	head->tailp = nodep->rlink;
	map[nodep->y][nodep->x] = 0;
	free(nodep);
	}
}

void insert_front(SHEAD *head, struct node *node){
	if(head->headp == NULL){
		head->headp = node;
		head->tailp = node;
	} else {
		node->llink = head->headp;
		head->headp->rlink = node;
		head->headp = node;
	}
}

void snakeGrowth(SHEAD *head, int map[][size], int y, int x){
	struct node *nodep;
	if( nodep = createBody(head, y, x) )
		insert_front(head, nodep);

	map[nodep->y][nodep->x] = 1;

}

void prt(int map[size][size], int n){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			printf("%d ",map[i][j]);
		}
		printf("\n");
	}
}

void mapInit(int m[size][size], int ymax, int xmax){
	for(int i = 0; i < xmax; i++){
		m[0][i] = 1;
		m[ymax-1][i] = 1;
	}

	for(int i = 1; i < ymax-1; i++){
		m[i][xmax-1] = 1;
		m[i][0] = 1;
	}
}

void changeLoc(SHEAD *s, int n){
	if( s->towards == 1 && s->x+1 < n)
		s->x++;
	if( s->towards == 0 && s->y-1 > -1)
		s->y--;
	if( s->towards == 2 && s->y+1 < n)
		s->y++;
	if( s->towards == 3 && s->x-1 > -1)
		s->x--;
}

int snakeMove(int map[][size], SHEAD *sp, char coms[], int m){
	sp->t++;
	//prt(map,m);
	if(coms[sp->t] == 'L'){
		if(sp->towards-1 < 0)
			sp->towards = 3;
		else
			sp->towards--;
	} else if(coms[sp->t] == 'D')
		sp->towards = (sp->towards + 1)%4;
	
	changeLoc(sp, m);
	if(map[sp->y][sp->x] == 2){
		snakeGrowth(sp, map, sp->y, sp->x);
		snakeMove(map, sp, coms, m);
	} else if(map[sp->y][sp->x] == 0){
		snakeGrowth(sp, map, sp->y, sp->x);
		remove_tail(sp, map);
		snakeMove(map, sp, coms, m);
	} else if(map[sp->y][sp->x] == 1 || sp->y >= m || sp->y < 0 || sp->x >= m || sp->x < 0){
		return sp->t;
	}

}

void snakeInit(SHEAD *h){
	h->towards = 1;
	h->t = 0;
	h->x = 0;
	h->y = 0;
	h->headp = NULL;
	h->tailp = NULL;
}

int main(){
	int map[size][size] = {0, };
	char commands[10001] = {0, };
	int N = 0;
	int K = 0;
	int L = 0;
	int x, y = 0;
	SHEAD s1;
	char c; // turn 90 degree to left  when c == L, to right when c == D
	scanf("%d", &N);
	scanf("%d", &K);
	snakeInit(&s1);
	//mapInit(map, N, N);
	for(int i = 0; i < K; i++){
		scanf("%d %d", &x, &y);
		map[x-1][y-1] = 2; // 2 == apple , 1 == wall
	}
	scanf("%d", &L);
	for(int i = 0; i < L; i++){
		scanf("%d %c", &x, &c);
		commands[x+1] = c;
	}

	snakeGrowth(&s1, map, 0, 0);

	printf("%d\n",snakeMove(map, &s1, commands, N));

	while(s1.tailp != NULL)
	{
		remove_tail(&s1,map);
	}

}
