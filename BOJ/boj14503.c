#include <stdio.h>

typedef struct robot{
	int toward;
	int xloc;
	int yloc;
} ROBOT;

void robotMove(ROBOT *r, int d){
	if(r->toward == 0)
		if(d)
			r->yloc--;
		else
			r->yloc++;
	if(r->toward == 3)
		if(d)
			r->xloc--;
		else
			r->xloc++;
	if(r->toward == 2)
		if(d)
			r->yloc++;
		else
			r->yloc--;
	if(r->toward == 1)
		if(d)
			r->xloc++;
		else
			r->xloc--;
		
}

void robotRotate(ROBOT *r){
	if(r->toward == 0)
		r->toward = 3;
	else if(r->toward == 3)
		r->toward = 2;
	else if(r->toward == 2)
		r->toward = 1;
	else if(r->toward == 1)
		r->toward = 0;
}

int robotChkIfWall(int map[50][50], ROBOT *r){
	if( r->toward == 0)
		if(map[r->yloc+1][r->xloc] == 1)
			return 1;

	if( r->toward == 3)
		if(map[r->yloc][r->xloc+1] == 1)
			return 1;

	if( r->toward == 2)
		if(map[r->yloc-1][r->xloc] == 1)
			return 1;

	if(r->toward == 1)
		if(map[r->yloc][r->xloc-1] == 1)
			return 1;

	return 0;
}

int chkIfClean(int map[50][50], ROBOT *r){
	if(r->toward == 0)
		if( map[r->yloc][r->xloc-1] == 0 )
			return 0;

	if(r->toward == 3)
		if( map[r->yloc+1][r->xloc] == 0)
			return 0;

	if(r->toward == 2)
		if( map[r->yloc][r->xloc+1] == 0)
			return 0;

	if(r->toward == 1)
		if( map[r->yloc-1][r->xloc] == 0)
			return 0;

	return 1;
}

void operation(int map[50][50], ROBOT *robot, int o){
	if( o == 1 )
		map[robot->yloc][robot->xloc] = 2;

	int c = 4;
	while(c > 0){
		if(chkIfClean(map, robot) == 0){
			robotRotate(robot);
			robotMove(robot,1);
			operation(map, robot, 1);
			return;
		} 
		else 
			robotRotate(robot);

		c--;
	}
	if(robotChkIfWall(map, robot) == 1){
		return;
	} else {
		robotMove(robot,0);
		operation(map, robot, 0);
		return;
	}
}


int main(){
	ROBOT robot;
	int map[50][50];
	int N = 50;
	int M = 50;
	int ans = 0;
	scanf("%d %d", &N, &M);
	scanf("%d %d %d", &robot.yloc, &robot.xloc, &robot.toward);
	// 0 == north, 1 == east, 2 == south, 3 == west
	for(int i = 0 ; i < N; i++){
		for(int j = 0; j < M; j++){
			scanf("%d", &map[i][j]); // 0 == empty, 1 == wall
		}
	}
	operation(map, &robot, 1);

	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++)
			if(map[i][j] == 2)
				ans++;

	printf("%d\n",ans);
}
