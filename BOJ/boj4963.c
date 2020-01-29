#include <stdio.h>
#define height 50
#define width 50

void clearMap(int a[height][width], int w, int h){
	for(int i = 0; i < h; i++)
		for(int j = 0; j < w; j++)
			a[i][j] = 0;
}

void clearVisit(int v[height][width], int w, int h){
	for(int i = 0; i < h; i++)
		for(int j = 0; j < w; j++)
			v[i][j] = 0;
}


int bfs(int a[height][width], int v[height][width], int y, int x, int w, int h){
	if(v[y][x] == 0)
		v[y][x] = 1;

	if( y+1 < h && a[y+1][x] == 1 && v[y+1][x] == 0)
		bfs(a, v, y+1, x, w, h);
	if( y-1 > -1 && a[y-1][x] == 1 && v[y-1][x] == 0)
		bfs(a, v, y-1, x, w, h);
	if( x+1 < w && a[y][x+1] == 1 && v[y][x+1] == 0)
		bfs(a, v, y, x+1, w, h);
	if( x-1 > -1 && a[y][x-1] == 1 && v[y][x-1] == 0)
		bfs(a, v, y, x-1, w, h);
	if( x-1 > -1 && y+1 < h && a[y+1][x-1] == 1 && v[y+1][x-1] == 0)
		bfs(a, v, y+1, x-1, w, h);
	if( x+1 < w && y+1 < h && a[y+1][x+1] == 1 && v[y+1][x+1] == 0)
		bfs(a, v, y+1, x+1, w, h);
	if( x-1 > -1 && y-1 > -1 && a[y-1][x-1] == 1 && v[y-1][x-1] == 0)
		bfs(a, v, y-1, x-1, w, h);
	if( x+1 < w && y-1 > -1 && a[y-1][x+1] == 1 && v[y-1][x+1] == 0)
		bfs(a, v, y-1, x+1, w, h);


}

int main(){
	int island[height][width] = {0, }; // 1 == land, 0 == sea
	int visit[height][width] = {0, };
	int data[2500] = {0, };
	int w;
	int h;
	int index = 0;
	while(1){
		int count = 0;
		scanf("%d %d",&w, &h);
		if( w == 0 && h == 0)
			break;
		for(int i = 0; i < h; i++){
			for(int j = 0; j < w; j++){
				scanf("%d", &island[i][j]);
			}
		}

		for(int i = 0; i < h; i++){
			for(int j = 0; j < w; j++){
				if(island[i][j] == 1 && visit[i][j] == 0){
					bfs(island, visit, i, j, w, h);
					count++;
				}		
			}
		}
		data[index] = count;
		clearMap(island, w, h);
		clearVisit(visit, w, h);
		index++;
	}

	for(int i = 0; i < index; i++)
		printf("%d\n", data[i]);
}
