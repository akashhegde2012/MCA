#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
const int n = 5000;
//generate random marks from 50 to 100
void give_me_random(int* marks){
	int i;
	for(i = 0;i < n;i++)
        marks[i] =(int)rand()%(100-50+1) + 50;
}
//print marks
void print(int* marks){
	int i;
	for (i = 0;i < n;i++)
		printf("%d \t",marks[i]);
	printf("\n");	
}
int compare(const void* a,const void* b){
	return (*(int*)a - *(int*)b);
}
//index of max element in marks
int find_max(int* marks){
	int i, index = 0;
	int max = marks[0];
  	for (i = 1; i < n; i++) {
    		if (marks[i] > max) {
      			max = marks[i];
      			index = i;
    		}
  	}
	return index;
}
//index of min element in marks
int find_min(int* marks) {
	int i, index = 0; 
	int min = marks[0];
	for (i = 1; i < n; i++) {
    		if (marks[i] < min) {
      			min = marks[i];
      			index = i;
    		}
  	}
  	return index;	
}
//sequential bubble sort function
void bubble_sort_sequential(int* marks, int size){
	int i,j,t;
	clock_t start = clock();
	for (i = 0;i < (n-1); i++){
    		for (j = 0 ; j < n-i-1; j++){
      			if (marks[j] > marks[j+1]){
        			t = marks[j];
        			marks[j] = marks[j+1];
        			marks[j+1] = t;
      			}
    		}
  	}
	clock_t end = clock();
	float elapsed = (float)(end - start) / CLOCKS_PER_SEC * 1000;
	printf("\nIt needed %f miliseconds \n", elapsed); 
  	printf("Sequential sorted marks:\n");
  	for (i = 0;i < n; i++ )
     		printf("%d \t", marks[i]);
	printf("\n");
}
//parallel bubble_sort_parallel 
void bubble_sort_parallel(int* marks, int rnk, int size){	
	clock_t begining = clock();
	int i;
	int second_marks[n];
	for (i = 0;i < size;i++){	
		qsort(marks, n, sizeof(int), &compare);
		int partner;		
		if(i%2 != 0){
			if (rnk % 2 == 0) {
        			partner = rnk - 1;
      			} else {
        			partner = rnk + 1;
      			}
		} 
		else {
			if (rnk % 2 == 0) {
        			partner = rnk + 1;
      			} else {
        			partner = rnk - 1;
      			}
		}
		if (partner >= size || partner < 0 ) {
      			continue;
    		}		
	 	if (rnk % 2 == 0) {
      			MPI_Send(marks, n, MPI_INT, partner, 0, MPI_COMM_WORLD);
      			MPI_Recv(second_marks, n, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    		} else {
      			MPI_Recv(second_marks, n, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      			MPI_Send(marks, n, MPI_INT, partner, 0, MPI_COMM_WORLD);
    		}
		if(rnk < partner){
			while(1){
				int min_index = find_min(second_marks);
				int max_index = find_max(marks);
				int t;
				if(second_marks[min_index] < marks[max_index]){
					t = second_marks[min_index];
					second_marks[min_index] = marks[max_index];
					marks[max_index] = t;
				}				
				else break;
			}
		} else {
			while(1){
				int min_index = find_min(marks);
				int max_index = find_max(second_marks);
				int t;
				if (second_marks[max_index] > marks[min_index]) {
          				t = second_marks[max_index];
          				second_marks[max_index] = marks[min_index];
          				marks[min_index] = t;
        			} else break;
      			}
		}
	}
	clock_t end = clock();
	float elapsed = (float)(end - begining) / CLOCKS_PER_SEC * 1000;
	printf("It needed %f miliseconds \n", elapsed);
} 
int main(int argc, char* argv[]){
	int rnk, size = 5000;
	int marks[n];
	int marks1[n];
  	MPI_Init(&argc, &argv);
  	MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
  	MPI_Comm_size(MPI_COMM_WORLD, &size);
	give_me_random(marks);
	int i = 0;
	for (i; i < n; i++){
		marks1[n] = marks[n];	
	}
	printf("marks before sorting: \n");
	print(marks);
	printf("\n\n");
	bubble_sort_parallel(marks,rnk,size);
	printf("marks after parallel sorting: \n");
	print(marks);
	printf("\n\n");
	bubble_sort_sequential(marks,size);
	MPI_Finalize( );
	return 0;
}