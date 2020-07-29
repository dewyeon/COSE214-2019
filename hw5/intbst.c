#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

static NODE *_makeNode( int data){
 NODE* new_node = (NODE*) malloc(sizeof(NODE));
 new_node->data = data;
 new_node->left = NULL;
 new_node->right = NULL;
 
 if(!new_node) free(new_node);
 return new_node;
}/* end of _makeNode() */

static void _destroy( NODE *root){
	if(root != NULL){
		if(root->left==NULL && root->right==NULL){
			free(root);
		}
		else{
			if(root->left!=NULL){
				_destroy(root->left);
			}
			if(root->right!=NULL){
				_destroy(root->right);
			}
			free(root);
		}
	}
}/* end of _destroy() */

static void _traverse( NODE *root){
 //inorder travaersal
 if(root!=NULL) {
	 _traverse(root->left);
	 printf("%d ", root->data);
	 _traverse(root->right);
 }/* end of if(root!=NULL) */
}/* end of _traverse() */

/* internal traversal function */
static void _infix_print( NODE *root, int level){
 if(root!=NULL){
  _infix_print(root->right, level+1);
  for(int i=0; i<level; i++){
   printf("	");
  }
  printf("%d\n", root->data);
  _infix_print(root->left, level+1);
 }
}/* end of _infix_print() */

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( void){
 TREE* new_tree = (TREE*) malloc(sizeof(TREE));
 new_tree->root = NULL;
 if(!new_tree) free(new_tree);

 return new_tree;
}/* end of BST_Create() */

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *BST_Destroy( TREE *pTree){
	if (pTree)
	{
		_destroy( pTree->root);
	}
		
	free( pTree);
	
	return NULL;
}/* end of BST_Destroy() */

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, int data){
 NODE* newNode = _makeNode(data);
 if(pTree->root==NULL){
	 pTree->root = newNode;
	 return 1;
  }
 else{
	 NODE* visit = pTree->root;
	 while(visit->left!=NULL || visit->right!=NULL){
		 if(newNode->data <= visit->data)
			 if(visit->left!=NULL)
				 visit = visit->left;
			 else break;
		 else if(newNode->data >= visit->data)
			 if(visit->right!=NULL)
				 visit = visit->right;
			 else break;
	 }/* end of while */
	 NODE** p_visit = &visit;
	 if(newNode->data <= (*p_visit)->data){ 
			 (*p_visit)->left = newNode;
			 if((*p_visit)->left) return 1;
			 else return 0;
	 }
	 else if(newNode->data >= (*p_visit)->data){
			 (*p_visit)->right = newNode;
			 if((*p_visit)->right) return 1;
			 else return 0;
	 }
  }
}/* end of BST_Insert() */

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
NODE *_retrieve( NODE *root, int key){
	if(root!=NULL){
		if(root->data == key) return root;
		else if(root->left!=NULL || root->right!=NULL){
			if(root->data > key){
				if(root->left!=NULL)
					_retrieve(root->left, key);
				else return NULL;
			}
			else {
				if(root->right!=NULL)
					_retrieve(root->right, key);
				else return NULL;
			}
		}/* end of else if */
		else return NULL;
	}/* end of if(root!=NULL) */
	else return NULL;
}/* end of _retrieve() */

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve( TREE *pTree, int key){
	NODE* nodeRetrieved = _retrieve(pTree->root, key);
	if(nodeRetrieved!=NULL)
		return &(nodeRetrieved->data);
	else
		return NULL;
}/* end of BST_Retrieve() */

NODE* findMaxNode(NODE* root){
	NODE* temp = root;
	while(temp->right!=NULL)
		temp = temp->right;
	return temp;
}/* end of findMaxNode() */

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
NODE *_delete( NODE *root, int dltKey, int *success){	
	if(root==NULL){
		*success = 0;
		return root;
	} 
	if(dltKey < root->data){
		root->left = _delete(root->left, dltKey, success);
	}
	else if(dltKey > root->data){
		root->right = _delete(root->right, dltKey, success);
	}
	else{ //node to delete == root
		//case1) root has 0 or 1 child	
		 if(root->left == NULL){
			NODE* temp = root->right;
			free(root); root=NULL; 
			*success = 1;
			return temp;
		}
		else if (root->right == NULL){
			NODE* temp = root->left;
			free(root); root=NULL;
			*success = 1;
			return temp;
		}	
		//case2) root has 2 children, Find largest node on the left subtree.
		else {
			NODE* temp = findMaxNode(root->left);
			root->data = temp->data;
			root->left = _delete(root->left, temp->data, success);
		}
	}
	return root;
}/* end of _delete() */

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete( TREE *pTree, int dltKey){
	int ret;
	pTree->root = _delete(pTree->root, dltKey, &ret);
	return ret;
}/* end of BST_Delete() */

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree){
	_traverse(pTree->root);
	
	return;
}

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree){
	_infix_print(pTree->root, 0);
	
	return;
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	int data;
	
    // creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}
	
	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 1; i < 20; i++)
	{
		data = rand() % 100 + 1; // 1 ~ 100 random number
		
		fprintf( stdout, "%d ", data);
		
		// insert funtion call
		BST_Insert( tree, data);
 	}
	fprintf( stdout, "\n");
			
	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	int ret;
	do
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		ret = scanf( "%d", &num);
		if (ret != 1) break;
		
 		ret = BST_Delete( tree, num);
		if (!ret) fprintf( stdout, "%d not found\n", num);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		
	} while(1);
	
	BST_Destroy( tree);

	return 0;
}

