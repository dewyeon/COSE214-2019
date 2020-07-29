#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp

// User structure type definition
typedef struct 
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
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
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr){
 NODE *newNode;

 if(!(newNode = (NODE*) malloc(sizeof(NODE)))){
  free(newNode);
  return 0;
 }
 if(pPre == NULL){
  //Adding 1) before first node or 2) to empty list.
  newNode->link = pList->head;
  pList->head = newNode;
  newNode->dataPtr = dataInPtr;
  if(newNode->link == NULL)
   pList->rear = newNode; //check is it's adding at the end of the list - set rear
  (pList->count)++;
  return 1;
 }
 else{
  //Adding in middle or at end.
  newNode->link = pPre->link;
  pPre->link = newNode;
  newNode->dataPtr = dataInPtr;
  if(newNode->link==(NODE *)NULL) pList->rear = newNode; //check if end, and set rear
  (pList->count)++;
  return 1;
 }
}/* end of _insert() */

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr){
 *dataOutPtr = pLoc->dataPtr;
 if(pPre==NULL){
  //Deleting the first node
  pList->head = pLoc->link;
 }
 else{
  //Deleting any other node
  pPre->link = pLoc->link;
 }

 //Test for deleting the last node
 if(pLoc->link==NULL) pList->rear = pPre; //set rear

 (pList->count)--;
 free(pLoc->dataPtr->token);
 free(pLoc->dataPtr);
 free(pLoc);
}/* end of _delete() */

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu){
 *pPre = NULL;
 *pLoc = pList->head;

 if((pList->head)==NULL){
  //empty list, got nothing to search
  return 0;
 }
 while((*pLoc)!=NULL && strcmp( ((*pLoc)->dataPtr->token), pArgu )<0 ){
   *pPre = *pLoc;
   *pLoc = (*pLoc)->link;
 }
if((*pLoc)!=NULL && strcmp( ((*pLoc)->dataPtr->token), pArgu )==0 ){
 ((*pLoc)->dataPtr->freq)++;
 return 1; //found
}
else
 return 0; //not found, even though traversed every node
}/* end of _search() */

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void){
 LIST *list_head = (LIST*) malloc(sizeof(LIST));
 
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
} /* end of createlist() */

/* Deletes all data in list and recycles memory
	return	NULL head pointer
*/
LIST *destroyList( LIST *pList){
 NODE *tmp_node_ptr;
 tmp_node_ptr = pList->head;

 while((tmp_node_ptr)!=(NODE *)NULL){
  pList->head = pList->head->link; //set list head to the successor node
  free(tmp_node_ptr->dataPtr->token);
  free(tmp_node_ptr->dataPtr);
  free(tmp_node_ptr); //release the previous head node's memory to heap
  tmp_node_ptr = pList->head; //set the current position (tmp) to new list head
 }
 free(pList);

 return pList;
}/* end of destroyList() */

/* Inserts data into list
	return	-1 if overflow
			0 if successful
			1 if duplicated key
*/
int addNode( LIST *pList, tTOKEN *dataInPtr){
 NODE* pPre;
 NODE* pLoc;

 int found = _search(pList, &pPre, &pLoc, dataInPtr->token);
 if(found) return 1; //dupe key

 int success = _insert(pList, pPre, dataInPtr);
 if(!success) return -1; //overflow
 else return 0; //successful
}/* end of addNode() */

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut){
 NODE* pPre;
 NODE* pLoc;

 int found = _search(pList, &pPre, &pLoc, keyPtr);
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
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut){
 int found;

 NODE *pPre;
 NODE *pLoc;

 found = _search(pList, &pPre, &pLoc, pArgu);
 if(found) *pDataOut = pLoc->dataPtr;
 else *pDataOut = NULL;
 return found;
}/* end of searchList() */

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
 
 while(tmp_node_ptr!=(NODE *)NULL){
	 printf("%s	%d\n", tmp_node_ptr->dataPtr->token, tmp_node_ptr->dataPtr->freq);
	 tmp_node_ptr = tmp_node_ptr->link;
 }
}/* end of printList() */

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken( char *str){
 tTOKEN* newToken = (tTOKEN*) malloc(sizeof(tTOKEN));
 
 if(newToken){
	newToken->token = strdup(str);
	newToken->freq = 1; 
 }
 else{
  free(newToken);
 }
 
 return newToken;
}/* end of createToken() */

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
tTOKEN *destroyToken( tTOKEN *pToken){
 free(pToken->token);	
 free(pToken);
 return pToken;
}/* end of destroyToken() */

////////////////////////////////////////////////////////////////////////////////
int main( void)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	
	// creates a null list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(scanf( "%s", str) == 1)
	{
		pToken = createToken( str);
		
		// insert function call
		ret = addNode( list, pToken);
		
		if (ret == 1) // duplicated 
			destroyToken( pToken);
	}
	// print function call
	printList( list);
	
	destroyList( list);
	
	return 0;
}
