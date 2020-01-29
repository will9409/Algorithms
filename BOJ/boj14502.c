#include <stdio.h>



void dfs(int arr[][8], int y, int x, int n, int m){

	arr[y][x] = 2;

	if( x+1 < m && arr[y][x+1] == 0)
		dfs(arr, y, x+1, n, m);
	if( x-1 >= 0 && arr[y][x-1] == 0)
		dfs(arr, y, x-1, n, m);
	if( y+1 < n && arr[y+1][x] == 0)
		dfs(arr, y+1, x, n, m);
	if( y-1 >= 0 && arr[y-1][x] == 0)
		dfs(arr, y-1, x, n, m);

}

int print(int c[][8], int n, int m){

	for(int i = 0 ; i < n; i++){
		for(int j = 0; j < m; j++){
			printf("%d ",c[i][j]);
			if(j == m-1)
				printf("\n");
		}
	}

}

int simulate(int a[][8], int n, int m){
	int cmap[8][8] = {0, };
	int sp = 0;
	for(int y = 0; y < n; y++){
		for(int x = 0; x < m; x++){
			cmap[y][x] = a[y][x];
		}
	}

	for(int y = 0; y < n; y++){
		for(int x = 0; x < m; x++){
			if(cmap[y][x] == 2 )
				dfs(cmap, y, x, n, m);
		}
	}
	printf("==========================\n");
	print(cmap,n,m);

	for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			if(cmap[i][j] == 0)
				sp++;
		}
	}

	return sp;
}



int main()
{
	int map[8][8];
	int N = 8;
	int M = 8;
	int ans = 0;
	scanf("%d %d",&N, &M);
	printf("N = %d, M = %d\n", N, M);
	for(int y = 0 ; y < N; y++){
		for(int x = 0; x < M; x++){
			scanf("%d",&map[y][x]);
		}
	}


	for(int y1 = 0; y1 < N; y1++){
		for(int x1 = 0; x1 < M; x1++){
			if(map[y1][x1] != 0)continue;
			for(int y2 = y1; y2 < N; y2++){
				for(int x2 = (y2 == y1) ? x1+1 : 0; x2 < M; x2++){
					if(map[y2][x2] != 0)continue;
					for(int y3 = y2; y3 < N; y3++){
						for(int x3 = (y3 == y2) ? x2+1 : 0; x3 < M; x3++){
							if(map[y3][x3] != 0)continue;
							map[y1][x1] = 1;
							map[y2][x2] = 1;
							map[y3][x3] = 1;
							printf("case\n");
							print(map,N,M);
							int space = simulate(map, N, M);
							printf("space = %d\n",space);
							if(ans < space)
								ans = space;
							map[y1][x1] = 0;
							map[y2][x2] = 0;
							map[y3][x3] = 0;
						}
					}
				}
				

			}	
		}	
	

	}
	printf("%d\n",ans);




}
