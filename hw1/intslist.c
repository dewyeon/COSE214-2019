#include <stdlib.h> // malloc
#include <stdio.h>
#include <ctype.h> // toupper

#define QUIT	1
#define INSERT	2
#define DELETE	3
#define PRINT	4
#define SEARCH	5

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	int			data;
	struct node	*link;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations


/* internal insert function
inserts data into a new node
	return 1 if successful
	       0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, int dataIn){
 NODE *newNode;

 if(!(newNode = (NODE*) malloc(sizeof(NODE)))) {
	 free(newNode); //이거 안하니까 valgrind 했을 때 memory leak 있었음
	 return 0; //memory overflow, cannot make a new node
 }
 if(pPre==NULL){
  //Adding 1) before first node or 2) to empty list.
  newNode->link = pList->head;
  pList->head = newNode;
  newNode->data = dataIn;
  if(pList->count==0) pList->rear = newNode; //Adding to empty list - set rear
  (pList->count)++;
  return 1;
 }
 
  //Adding in middle or at end.
  newNode->link = pPre->link;
  pPre->link = newNode;
  newNode->data = dataIn;
  if(newNode->link==NULL) pList->rear = newNode; //check if it's adding at the end of the list - set rear
  (pList->count)++;
  return 1;
}/* end of _insert() */

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, int *dataOut){
	*dataOut = pLoc->data;
	if(pPre == NULL){
		//Deleting the first node
		pList->head = pLoc->link;
	}
	else{
		//Deleting any other node
		pPre->link = pLoc->link;
	}
	
	//Test for deleting the last node
	if(pLoc->link==NULL) pList->rear = pPre; //rear 설정
	
	(pList->count)--;
	free(pLoc);
}/* end of _delete() */

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, int argu){ 
 *pPre = NULL;
 *pLoc = pList->head;

 if((pList->head)==NULL){
  //empty list, got nothing to search
  return 0;
 }

 while((*pLoc)!=NULL && (((*pLoc)->data)<argu)){
  *pPre = *pLoc;
  *pLoc = (*pLoc)->link;
 }
 if((*pLoc)!=NULL && ((*pLoc)->data)==argu){
  return 1;
 }
 else{
  return 0;
 }
}/* end of _search() */

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void){
 LIST* list_head = (LIST*) malloc(sizeof(LIST));
 
 if(list_head){
	list_head->count = 0;
	list_head->pos = NULL;
	list_head->head = NULL;
	list_head->rear = NULL;
 }
 else{
	 free(list_head);
 }
 return list_head;
}/* end of createList() */

/* Deletes all data in list and recycles memory
	return	NULL head pointer
*/
LIST *destroyList( LIST *pList)
{
 NODE *tmp_node_ptr;
 tmp_node_ptr = pList->head;

 while((tmp_node_ptr)!=(NODE *)NULL){
  pList->head = pList->head->link; //set list head to successor node
  free(tmp_node_ptr); //release the previous head node's memory to heap
  tmp_node_ptr = pList->head; // set the current position (tmp) to new list head 
 }

 free(pList); // release the list head (list_head) memory to heap

 return pList;
}/* end of destroyList() */

/* Inserts data into list
	return	-1 if overflow
		0 if successful
		1 if dupe key
*/
int addNode( LIST *pList, int dataIn){
 NODE* pPre;
 NODE* pLoc;

 int found = _search(pList,&pPre, &pLoc, dataIn);
 if(found) return 1; //dupe key
 
 int success = _insert(pList, pPre, dataIn);
 if(!success) return -1; //overflow
 else return 0; //successful
}/* end of addNode() */
  
/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, int Key, int *dataOut){
	NODE* pPre;
	NODE* pLoc;
	
	int found = _search(pList, &pPre, &pLoc, Key);
	if(found){
		_delete(pList, pPre, pLoc, dataOut);
	}
	
	return found;

}/* end of removeNode() */

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, int Argu, int *dataOut){
 int found;

 NODE *pPre;
 NODE *pLoc;

 found = _search(pList, &pPre, &pLoc, Argu);
 if(found) *dataOut = pLoc->data;
 else dataOut = NULL;
 return found;
} /* end of searchList() */

/* returns number of nodes in list
*/
int listCount( LIST *pList){
 int listcount = pList->count;
 return listcount;
}

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList){
 if(!listCount) return 1;
 else return 0;
}/* end of emptyList() */

//int fullList( LIST *pList);

/* prints data from list
*/
void printList( LIST *pList){
 NODE *tmp_node_ptr = pList->head;
 while(tmp_node_ptr!=NULL){
  printf("%d ", tmp_node_ptr->data);
  tmp_node_ptr = tmp_node_ptr->link;
  printf("-> ");
 }
 printf("NULL\n");
}/* end of printList() */

/* gets user's input
*/
int get_action()
{	
	char ch;
	
	scanf( "%c", &ch);
	ch = toupper( ch);
	
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return PRINT;
		case 'I':
			return INSERT;
		case 'D':
			return DELETE;
		case 'S':
			return SEARCH;
	}
	return 0; // undefined action
}/* end of get_action() */

////////////////////////////////////////////////////////////////////////////////
int main( void)
{
	int num;
	LIST *list;
	int data;
	
	//creates a null list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	fprintf( stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");
	
	while(1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case PRINT:
				// print function call
				printList(list);
				break;
				
			case INSERT:
				fprintf( stdout, "Enter a number to insert: ");
				fscanf( stdin, "%d", &num);
				
				// insert function call
				addNode( list, num);
				
				// print function call
				printList( list);
				break;
				
			case DELETE:
				fprintf( stdout, "Enter a number to delete: ");
				fscanf( stdin, "%d", &num);
				
				// delete function call
				removeNode( list, num, &data);
				
				// print function call
				printList( list);
				break;
			
			case SEARCH:
				fprintf( stdout, "Enter a number to retrieve: ");
				fscanf( stdin, "%d", &num);
				
				// search function call
				int found;
				found = searchList( list, num, &data);
				if (found) fprintf( stdout, "Found: %d\n", data);
			    else fprintf( stdout, "Not found: %d\n", num);
				
				break;
		}
		if (action) fprintf( stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");
		
	}
	
	return 0;
}
