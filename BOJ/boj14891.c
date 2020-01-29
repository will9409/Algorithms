#include <stdio.h>
#define SWAP(T, a, b, c, d, e, f, g, h) do { T tmp = a; a = b; b = c; c = d; d = e; e = f; f = g; g = h; h = tmp;} while(0)

typedef struct gear{ // N == 0, S == 1
	char s[8];
} GEAR;

void spin(GEAR *g, int i, int n){
	// n == 1 , clockwise, == -1 n-clockwise
	if( n == 1 )
		SWAP(char, g[i].s[7], g[i].s[6], g[i].s[5], g[i].s[4], g[i].s[3], g[i].s[2], g[i].s[1], g[i].s[0]);
	if( n == -1 )
		SWAP(char, g[i].s[0], g[i].s[1], g[i].s[2], g[i].s[3], g[i].s[4], g[i].s[5], g[i].s[6], g[i].s[7]);
		// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

}

void initSpin(GEAR *g, int i, int n){

	if(i == 0){
		if(g[i].s[2] != g[i+1].s[6]){
			if(g[i+1].s[2] != g[i+2].s[6]){
				if(g[i+2].s[2] != g[i+3].s[6]){
					spin(g, i+3, -n);
				}
				spin(g, i+2, n);
			}
			spin(g, i+1, -n);
		}
		spin(g, i, n);
	}
       	else if(i == 1){
		if(g[i].s[6] != g[i-1].s[2])
			spin(g, i-1, -n);

		if(g[i].s[2] != g[i+1].s[6]){
			if(g[i+1].s[2] != g[i+2].s[6]){
				spin(g, i+2, n);
			}
			spin(g, i+1, -n);
		}
		spin(g, i, n);
	
	}
	else if(i == 2){
		if(g[i].s[6] != g[i-1].s[2]){
			if(g[i-1].s[6] != g[i-2].s[2]){
				spin(g, i-2, n);
			}
			spin(g, i-1, -n);
		}

		if(g[i].s[2] != g[i+1].s[6]){
			spin(g, i+1, -n);
		}
		spin(g, i, n);
	}
	else if(i == 3){
		if(g[i].s[6] != g[i-1].s[2]){
			if(g[i-1].s[6] != g[i-2].s[2]){
				if(g[i-2].s[6] != g[i-3].s[2]){
					spin(g, i-3, -n);
				}
				spin(g, i-2, n);
			}
			spin(g, i-1, -n);
		}
		spin(g, i, n);
	}
}

int main(){
	GEAR g[4];
	char state[9] = {0, };
	int K = 0;
	int d = 0;
	int n = 0;
	int ans = 0;

	for(int i = 0 ; i < 4; i++){
		scanf("%s",&state);
		for(int j = 0; j < 8; j++){
			g[i].s[j] = state[j]-48;
		}
	}
	scanf("%d",&K);
	
	for(int k = 0; k < K; k++){
		scanf("%d %d", &n, &d);
		initSpin(g, n-1, d);
	}

	for(int l = 0; l < 4; l++){
		if(g[l].s[0] == 1)
		{
			for(int m = 0; m < l; m++)
				g[l].s[0] = g[l].s[0] << 1;
			ans = ans + g[l].s[0];
		}
	}
	printf("%d\n", ans);

}

