#include <stdio.h>


int main(){
	int ee = 1;
	int ss = 1;
	int mm = 1;
	int e = 1;
	int s = 1;
	int m = 1;
	int y = 1;
	scanf("%d %d %d", &ee, &ss, &mm);
	while(1){
		if( e == ee && s == ss && m == mm ){
			break;
		}
		e++;
		s++;
		m++;
		if( e  >= 16 )
			e = 1;
		if( s  >= 29 )
			s = 1;
		if( m  >= 20 )
			m = 1;
		y++;
		
	}
			printf("%d\n",y);
}
