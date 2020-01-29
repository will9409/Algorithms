#include <stdio.h>
#define size 101 

void bridgeInit(int map[size][size][2]){
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			map[i][j][1] = 0;
}

int chkBlocks(int map[size][size][2], int yloc, int xloc, int len, int o){
	if( o == 0 ){
		int this = map[yloc][xloc][0];
		for(int i = 0; i < len; i++){
			if(this != map[yloc][xloc-i][0] || map[yloc][xloc-i][1] == 1)
				return 0;
		}
	} else if( o == 1 ){
		int this = map[yloc][xloc][0];
		for(int i = 0; i < len; i++){
			if(this != map[yloc][xloc+i][0] || map[yloc][xloc+i][1] == 1)
				return 0;
		}
	} else if( o == 2){
		int this = map[yloc][xloc][0];
		for(int i = 0; i < len; i++){
			if(this != map[yloc-i][xloc][0] || map[yloc-i][xloc][1] == 1)
				return 0;
		}
	} else if( o == 3){
		int this = map[yloc][xloc][0];
		for(int i = 0; i < len; i++){
			if(this != map[yloc+i][xloc][0] || map[yloc+i][xloc][1] == 1)
				return 0;
		}
	} else 
		return 0;

	return 1;
}

int simulate(int map[size][size][2], int n, int l){
	int answer = 0;

	for(int i = 0 ; i < n; i++){
		for(int j = 0; j < n-1; j++){
			int sub = map[i][j][0] - map[i][j+1][0];
			if(sub == 0){
				if( j == n-2 ){
					answer++;
					break;
				}
			       	continue;
			}
			else if(sub >= 2 || sub <= -2) break;
			else if(sub == -1){
				if(j - (l - 1) >= 0 && chkBlocks(map, i, j, l, 0)){
					for(int k = 0; k < l; k++){
						map[i][j-k][1] = 1;
					}
					if(j == n-2){
						answer++;
					}
					continue;
				} else
					break;
				}
			else if(sub == 1){
				if(j + l <= n-1 && chkBlocks(map, i, j+1, l, 1)){
					for(int k = 1; k < l+1; k++){
						map[i][j+k][1] = 1;
					}
					if(j == n-2){
						answer++;
					}
					continue;
				} else
					break;
			}
		}
	}
	bridgeInit(map);
	for(int i = 0 ; i < n; i++){
		for(int j = 0; j < n-1; j++){
			int sub = map[j][i][0] - map[j+1][i][0];
			if(sub == 0){
				if( j == n - 2 ){
					answer++;
					break;
				}
				continue;
			}
			else if(sub >= 2 || sub <= -2) break;
			else if(sub == -1){
				if(j - (l - 1) >= 0 && chkBlocks(map, j, i, l, 2)){
					for(int k = 0; k < l; k++){
						map[j-k][i][1] = 1;
					}
					if(j == n-2){
						answer++;
					}
					continue;
				} else
					break;
				}
			else if(sub == 1){
				if(j + l <= n-1 && chkBlocks(map, j+1, i, l, 3)){
					for(int k = 1; k < l+1; k++){
						map[j+k][i][1] = 1;
					}
					if(j == n-2){
						answer++;
					}
					continue;
				} else
					break;
			}
		}
	}
	return answer;
}


int main(){

	int map[size][size][2] = {0, };
	int N = 0;
	int L = 0;
	int ans = 0;

	scanf("%d %d",&N, &L);

	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			scanf("%d",&map[i][j][0]);

	ans = simulate(map, N, L);
	printf("%d\n",ans);

}
