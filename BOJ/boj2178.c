#include <stdio.h>
#include <stdlib.h>
#define size 100

typedef struct node {
	int xloc;
	int yloc;
	struct node *next;
} NODE;

typedef struct queue {
	struct node *head;
	struct node *tail;
} QUEUE;

NODE *create_node(int y, int x){
	NODE *nodep;
	if((nodep = (NODE *)malloc(sizeof(*nodep))) == NULL)
		printf("malloc failed.\n");

	nodep->xloc = x;
	nodep->yloc = y;

	return nodep;
}


void enqueue(QUEUE *q, int y, int x){
	NODE *nodep = create_node(y, x);

	if(q->head == NULL){
		q->head = nodep;
		q->tail = nodep;
	} else {
		q->tail->next = nodep;
		q->tail = nodep;
		nodep->next = NULL;
	}
}

void pop_front(QUEUE *q){
	NODE *nodep;
	nodep = q->head;
	q->head = nodep->next;
	free(nodep);
}

int is_empty(QUEUE *q){
	if(q->head == NULL)
		return 1;
	else
		return 0;
}


void bfs(int a[size][size], int v[size][size], QUEUE *q, int n, int m, int y, int x){
	int e = a[y][x];

	if(y + 1 < n && a[y+1][x] == 1 && v[y+1][x] == 0){
		enqueue(q, y+1, x);
		a[y+1][x] = e+1;
	}
	if(y - 1 > -1 && a[y-1][x] == 1 && v[y-1][x] == 0){
		enqueue(q, y-1, x);
		a[y-1][x] = e+1;
	}
	if(x + 1 < m && a[y][x+1] == 1 && v[y][x+1] == 0){
		enqueue(q, y, x+1);
		a[y][x+1] = e+1;
	}
	if(x - 1 > -1 && a[y][x-1] == 1 && v[y][x-1] == 0){
		enqueue(q, y, x-1);
		a[y][x-1] = e+1;
	}
	while(is_empty(q) != 1)
	{
		int nextx = q->head->xloc;
		int nexty = q->head->yloc;
		pop_front(q);
		bfs(a, v, q, n, m, nexty, nextx);
	}
}


int main(){
	int maze[size][size] = {0, }; // 1 == road , 0 == wall
	int visit[size][size] = {0, };
	int N = 0;
	int M = 0;
	char s[100] = "";
	QUEUE q1;
	q1.head = NULL;
	scanf("%d %d",&N, &M);
	for(int i = 0; i < N; i++){
		scanf("%s", &s);
		for(int j = 0; j < M; j++){
			maze[i][j] = s[j]-48;
		}
	}

	bfs(maze, visit, &q1, N, M, 0, 0);

	printf("%d\n", maze[N-1][M-1]);

}
