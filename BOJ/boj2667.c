#include <stdio.h>
#define size 25
#define SWAP(T, a, b) do {T tmp = a; a = b; b = tmp;} while(0)

int partition(int a[], int s, int e){
	int left;
	int right;
	int pivot;
	left = s+1;
	right = e;
	pivot = a[s];

	while(left <= right){
		while(a[left] <= pivot && left <= right)
			left++;
		while(a[right] >= pivot && left <= right)
			right--;

		if(left <= right)
			SWAP(int, a[left], a[right]);
	}
	
	SWAP(int, a[s], a[right]);

	return right;

}

void quick_sort(int a[], int s, int e){
	if(s >= e)
		return;
	
	int m = partition(a, s, e);
	
	quick_sort(a, s, m-1);
	quick_sort(a, m+1, e);
}



int bfs(int map[size][size], int v[size][size], int y, int x, int n){
	int e;
	e = 1;
	if(v[y][x] == 0){
		v[y][x] = 1;
		map[y][x] = 0;
	}


	if(y+1 < n && map[y+1][x] == 1 && v[y+1][x] == 0)
		e += bfs(map, v, y+1, x, n);
	if(y-1 >= 0 && map[y-1][x] == 1 && v[y-1][x] == 0)
		e += bfs(map, v, y-1, x, n);
	if(x+1 < n && map[y][x+1] == 1 && v[y][x+1] == 0)
		e += bfs(map, v, y, x+1, n);
	if(x-1 >= 0 && map[y][x-1] == 1 && v[y][x-1] == 0)
		e += bfs(map, v, y, x-1, n);
	
	return e;
}




int main(){
	int map[size][size] = {0, };
	int visit[size][size] = {0, };
	int blocks[size] = {0, };
	char s[size] = {0, };
	int k = 0;
	int N = 5;
	scanf("%d",&N);
	for(int i = 0; i < N; i++){
		scanf("%s",&s);
		for(int j = 0; j < N; j++){
			map[i][j] = s[j]-48;
		}
	}

	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(map[i][j] == 1 && visit[i][j] == 0){
				blocks[k] = bfs(map, visit, i, j, N);
				k++;
			}
		}
	}

	printf("%d\n",k);
	quick_sort(blocks, 0, k-1);
	for(int i = 0; i < k; i++)
		printf("%d\n",blocks[i]);
	

}
