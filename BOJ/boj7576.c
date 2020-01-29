#include <stdio.h>
#include <stdlib.h>
#define size 1000

typedef struct node {
	int x;
	int y;
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

	nodep->y = y;
	nodep->x = x;

	return nodep;
}

int is_empty(QUEUE *q){
	if(q->head == NULL)
		return 1;
	else
		return 0;
}

void enqueue(QUEUE *q, int y, int x){
	NODE *nodep = create_node(y, x);

	if(is_empty(q)){
		q->head = nodep;
		q->tail = nodep;
	} else {
		q->tail->next = nodep;
		q->tail = nodep;
		nodep->next = NULL;
	}
}

void pop_front(QUEUE *q){
	if(is_empty(q))
		printf("queue is empty.\n");
	else {
		NODE *nodep = q->head;
		q->head = nodep->next;
		free(nodep);
	}
}

void print(int a[size][size], int N, int M)
{
	for(int i = 0 ; i < N; i++){
		for(int j = 0; j < M; j++){
			printf("%d ",a[i][j]);
		}
		printf("\n");
	}
	printf("=====================================\n");
}


void bfs(int s[size][size], int v[size][size], QUEUE *q, int N, int M, int y, int x){
	int e = s[y][x];
	v[y][x] = 1;

	//print(s, N, M);

	if( y + 1 < N && s[y+1][x] == 0 && v[y+1][x] == 0){
		enqueue(q, y+1, x);
		s[y+1][x] = e+1;
	}
	if( y - 1 > -1 && s[y-1][x] == 0 && v[y-1][x] == 0){
		enqueue(q, y-1, x);
		s[y-1][x] = e+1;
	}
	if( x + 1 < M && s[y][x+1] == 0 && v[y][x+1] == 0){
		enqueue(q, y, x+1);
		s[y][x+1] = e+1;
	}
	if( x - 1 > -1 && s[y][x-1] == 0 && v[y][x-1] == 0){
		enqueue(q, y, x-1);
		s[y][x-1] = e+1;
	}

	while(is_empty(q) != 1){
		int ty = q->head->y;
		int tx = q->head->x;
		pop_front(q);
		bfs(s, v, q, N, M, ty, tx);
	}
}

int main(){
	int storage[size][size] = {0, }; // raw tomato == 0, or 1, empty == -1
	int visit[size][size] = {0, };
	int N = 0;
	int M = 0;
	int thisx;
	int thisy;
	int now = 0;
	QUEUE q1;
	scanf("%d %d", &M, &N);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++)
			scanf("%d", &storage[i][j]);

	for(int i = 0 ; i < N; i++){
		for(int j = 0; j < M; j++){
			if(storage[i][j] == 1){
				enqueue(&q1, i, j);
				visit[i][j] = 1;
			}
		}
	}

	thisx = q1.head->x;
	thisy = q1.head->y;
	pop_front(&q1);
	bfs(storage, visit, &q1, N, M,thisy, thisx);
	
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			if(storage[i][j] == 0){
				printf("-1\n");
				return 0;	
			}
			if(storage[i][j] > now)
				now = storage[i][j];
		}
	}
	if(now == 1)
		printf("%d\n", 0);
	else
		printf("%d\n", now-1);

}
