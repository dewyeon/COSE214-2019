#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time
#include <assert.h>

#define MAX_ELEM	20

typedef struct
{
	int *heapArr;
	int	last; //last element's index
	int	capacity;
} HEAP;

HEAP *heapCreate( int capacity)
{
	HEAP* newHeap = (HEAP*) malloc(sizeof(HEAP));
	int *arr = (int*) malloc(sizeof(int) * capacity);
	newHeap->heapArr = arr;
	newHeap->capacity = capacity;
	newHeap->last = -1; 
	
	if(!newHeap) free(newHeap);
	return newHeap;
}/* end of heapCreate() */

void heapDestroy( HEAP *heap)
{
	free(heap->heapArr);
	free(heap);
	
	return;
}/* end of heapDestroy() */

int heapInsert( HEAP *heap, int data);

static void _reheapUp( HEAP *heap, int index){
	if(index==0) return;
	else{
		while(heap->heapArr[index]>heap->heapArr[(index-1)/2]){
			int temp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[(index-1)/2];
			heap->heapArr[(index-1)/2] = temp;
			index=(index-1)/2;
		}
		return;
	}
}/* end of _reheapUp() */

int heapDelete( HEAP *heap, int* data);

static void _reheapDown( HEAP *heap, int index){
	int left, right, larger;
	if(index==heap->last) return;
	else if(index < heap->last){
		left = index*2 +1;
		if((index*2+1)==heap->last) {
			larger = left;
		}
		else if((index*2+2) <= heap->last){
			right = index*2 +2;
			if(heap->heapArr[left] > heap->heapArr[right]) larger = left;
			else larger = right;
		}
		else return;
		
		if(heap->heapArr[index] < heap->heapArr[larger]){
			int temp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[larger];
			heap->heapArr[larger] = temp;
			_reheapDown(heap, larger);
		}
	}
}/* end of _reheapDown() */

void heapPrint( HEAP *heap); 

/////////////////////////////////////////////////

int heapInsert( HEAP *heap, int data){
	if(heap->last==heap->capacity) return 0;
	
	++(heap->last);
	heap->heapArr[heap->last] = data;
	_reheapUp(heap, heap->last);
	return 1;
}/* end of heapInsert() */

int heapDelete( HEAP *heap, int* data){
	if(heap->last==-1) return 0;
	
	*data = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	heap->heapArr[heap->last] = 0;
	--(heap->last);
	_reheapDown(heap, 0);
	return 1;
}/* end of heapDelete() */
	
void heapPrint( HEAP *heap){
	for(int i=0; i<=heap->last; i++){
		printf("  %d  ", heap->heapArr[i]);
	}
	printf("\n");
}/* end of heapPrint() */
	
////////////////////////////////////////////////
	
	
int main(void)
{
	HEAP *heap;
	int data;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL));
	
	for (int i = 1; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		
		heapPrint( heap);
 	}

	while (heap->last >= 0)
	{
		// delete function call
		heapDelete( heap, &data);

		fprintf( stdout, "Deleting %d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}
