#include <stdio.h>
#define size 501

typedef struct block {
	int loc[8];
} BLOCK;

void block_init(BLOCK *b, char *a){
	for(int i = 0; i < 8; i++)
		b->loc[i] = *(a+i)-48;
}

int block_move(BLOCK *b, char *a, int N, int M){
	int t = 0;
	for(int i = 1; i < 8; i = i+2){
		if(b->loc[i]+1 > M){
			t = 1;
			break;
		}
	}

	if(t == 0){
		for(int i = 1; i < 8; i = i+2){
			b->loc[i]++;
		}
		return 1; 

	} else if( t == 1 ) { // 가로축의 끝에 도달하면
		for(int i = 0; i < 8; i = i+2){
			if(b->loc[i] + 1 > N){ // 세로축의 끝인지 확인
				t = 2; 
				break;
			}
		}
		if( t != 2 ){ // 지금이 세로축의 끝이 아니라면
			for(int i = 0; i < 8; i++){
				if( i % 2 == 0) // 모든 좌표의 세로축 값 ++
					b->loc[i]++;
				else // 가로축의 값은 초기화
					b->loc[i] = *(a+i)-48;
			}
			return 1;
		} else if ( t == 2 )
			return 0;
	}
}

int main(){
	int N;
	int M;
	char *b[19] = {"00010203","00102030",
		"00010211","01101121","00102011","01101112",
		"00102021","00010210","00011121","02101112",
		"01112120","00101112","00011020","00010212",
		"00101121","01021011",
		"01111020","00011112","00011011"};
	int map[size][size] = {0, };
	int ans = 0;
	int temp = 0;
	BLOCK b1[19];
	for(int i = 0; i < 19; i++){
		block_init(b1+i,b[i]);
	}
	scanf("%d %d", &N, &M);
	for(int i = 0; i < N; i++){
		for(int j = 0; j < M; j++){
			scanf("%d", &map[i][j]);
		}
	}
	

	for(int i = 0 ; i < 19; i++){
		do{
			for(int j = 0; j < 8; j = j+2){
				temp += map[b1[i].loc[j]][b1[i].loc[j+1]];
			}
			if(ans < temp)
				ans = temp;
			temp = 0;
		} while (block_move(b1+i, b[i], N, M));
	}
	printf("%d\n", ans);

}
