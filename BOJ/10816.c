#include <stdio.h>
#define SWAP(T,a,b) do {T tmp = a; a = b; b = tmp; } while(0)


int partition(int s, int e, int data[]);

void qsort(int s, int e, int arr[]){

	if(s >= e)
		return;

	int m = partition(s,e,arr);

	qsort(s,m-1,arr);
	qsort(m+1,e,arr);
}

int partition(int s, int e, int data[]){
	int pivot= data[s];
	int left = s+1;
	int right = e;


	while( left <= right ){
		while( (data[right] >= pivot) && (left <= right) )
			right--;

		while( (data[left] <= pivot) && (left <= right) )
			left ++;

		if( left <= right )
			SWAP(int, data[left], data[right] );

	}
	SWAP(int, data[right],data[s]);
	return right;

}

int findf(int ndata[], int number, int s, int e, int bf){
	int flag = (s+e)/2;

	if(flag < 0 || flag == bf)
		return 0;

	if(number == ndata[flag]){
		int count = 0;
		count ++;
		int tmp = flag;
		flag++;
		tmp--;
		while(number == ndata[flag]){
			count++;
			flag++;
		}
		while(number == ndata[tmp]){
			count++;
			tmp--;
		}
		return count;
	}


	if(number > ndata[flag])
		return findf(ndata, number, flag ,e, flag);

	if(number < ndata[flag])
		return findf(ndata, number, s, flag, flag);

}



int main(){
	int N = 500000;
	int M = 500000;
	int ndata[N];
	int mdata[M];
	int ans[M];
	scanf("%d",&N);
	for(int i =0; i < N; i++){
		scanf("%d",&ndata[i]);
	}

	scanf("%d",&M);

	for(int i =0; i < M; i++){
		scanf("%d",&mdata[i]);
	}

	qsort(0,N-1,ndata);	

	for(int i = 0; i < M; i++){
		printf("%d ",findf(ndata, mdata[i], 0, N-1, 0));

	}

}
