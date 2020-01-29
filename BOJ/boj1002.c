#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// **double floating point error**

int main(){
	int x1 = 0;
	int y1 = 0;
	int r1 = 0;
	int x2 = 0;
	int y2 = 0;
	int r2 = 0;
	int T = 0;
	scanf("%d", &T);
	for(int i = 0; i < T; i++){
		scanf("%d %d %d %d %d %d", &x1, &y1, &r1, &x2, &y2, &r2);
		long int t1 = (x2 - x1)*(x2 - x1);
		long int t2 = (y2 - y1)*(y2 - y1);
		long int len = t1 + t2;

		if((x1 == x2) && (y1 == y2) ){
			if( r1 == r2 ){
				printf("%d\n", -1);
			} else {
				printf("%d\n", 0);
			}

		} else {
			if( ( (r1 + r2)*(r1 + r2) < len) | ( (r1 - r2)*(r1 - r2) > len) ){
				printf("%d\n", 0);
			} else if( ((r1 + r2)*(r1 + r2) == len) || ((r1 - r2)*(r1 - r2) == len) ){
				printf("%d\n", 1);
			} else {
				printf("%d\n", 2);
			}
		}
	}
}
