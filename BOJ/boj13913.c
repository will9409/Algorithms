#include <stdio.h>
#include <stdlib.h>
#define size 100001

// 배열에 각 값이 어디에서 출발했는지 입력할 수 있다.


typedef struct node{
	int loc;
	struct node *next;
} NODE;

typedef struct queue{
	struct node *head;
	struct node *tail;
} QUEUE;

NODE *create_node(QUEUE *q, int t){
	NODE *nodep;
	if((nodep = (NODE *)malloc(sizeof(*nodep))) == NULL)
		printf("malloc failed.\n");

	nodep->loc = t;
	nodep->next = NULL;
	return nodep;
}

void enqueue(QUEUE *q, int t){
	NODE *nodep;
	nodep = create_node(q, t);

	if(q->head == NULL){
		q->head = nodep;
		q->tail = nodep;
	} else {
		q->tail->next = nodep;
		q->tail = nodep;
	}
}

int pop_front(QUEUE *q){
	if( q->head == NULL)
		printf("pop failed.\n");
	else {
		NODE *nodep = q->head;
		int t = nodep->loc;
		q->head = nodep->next;
		free(nodep);

		return t;
	}

}

void bfs(QUEUE *q, int v[size][2], int n, int k ){

	if(n - 1 > -1 && v[n-1][0] == 0){
		enqueue(q, n-1);
		v[n-1][0] = v[n][0] +1;
		v[n-1][1] = n;
	}
	if(n + 1 <= size && v[n+1][0] == 0){
		enqueue(q, n+1);
		v[n+1][0] = v[n][0]+1;
		v[n+1][1] = n;
	}
	if(n * 2 <= size && v[n*2][0] == 0){
		enqueue(q, n*2);
		v[n*2][0] = v[n][0]+1;
		v[n*2][1] = n;
	}
	if(v[k][0] != 0){
		return;
	}

	while(q->head != NULL){
		bfs(q, v, pop_front(q), k);
	}

}


void print(int v[size][2],int n, int k){
	if( n == k){
		return;
	}
	print(v,n, v[k][1]);
	printf("%d ",v[k][1]);
}

int main()
{
	int N;
	int K;
	int visit[size][2] = {0, };
	scanf("%d %d", &N, &K);
	QUEUE q1;
	q1.head = NULL;
	q1.tail = NULL;
	if( N == K ){
		printf("%d\n", 0);
	} else {
		bfs(&q1, visit, N, K);
		printf("%d\n", visit[K][0]);
	}

	print(visit, N, K);
	printf("%d\n",K);

}
