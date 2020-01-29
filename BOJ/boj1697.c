#include <stdio.h>
#include <stdlib.h>
#define size 100001

typedef struct node {
	int loc;
	struct node *next;
} NODE;

typedef struct queue{
	struct node *head;
	struct node *tail;
} QUEUE;

NODE *create_node(int l){
	NODE *nodep;
	if((nodep = (NODE *)malloc(sizeof(*nodep))) == NULL)
		printf("malloc failed.\n");

	nodep->loc = l;
	nodep->next = NULL;

	return nodep;
}

void enqueue(QUEUE *q, int loc){
	NODE *nodep = create_node(loc);

	if(q->head == NULL){
		q->head = nodep;
		q->tail = nodep;
	} else {
		q->tail->next = nodep;
		q->tail =nodep;
	}
}

int pop_front(QUEUE *q){
	if(q->head != NULL){
		int l;
		NODE *nodep = q->head;
		l = nodep->loc;
		q->head = nodep->next;
		free(nodep);
		return l;
	}
}

int is_empty(QUEUE *q){
	if(q->head == NULL)
		return 1;
	else
		return 0;
}

void bfs(QUEUE *q, int v[size], int n, int k){
	if(v[k] != 0)
		return;
	
	if(n - 1 > -1 && v[n-1] == 0){
		enqueue(q,n-1);
		v[n-1] = v[n]+1;
	}
	if(n + 1 <= size && v[n+1] == 0){
		enqueue(q,n+1);
		v[n+1] = v[n]+1;
	}
	if(n * 2 <= size && v[n*2] == 0){
		enqueue(q,n*2);
		v[n*2] = v[n]+1;
	}

	while(is_empty(q) != 1){
		bfs(q, v, pop_front(q), k);
	}
}


int main(){
	int N;
	int K;
	QUEUE q1;
	int visit[size] = {0, };
	q1.head = NULL;
	q1.tail = NULL;
	scanf("%d %d", &N, &K);
	if( N == K ){
		printf("%d\n", 0);	
	} else {
		bfs(&q1, visit, N, K);
		printf("%d\n",visit[K]);
	}

}
