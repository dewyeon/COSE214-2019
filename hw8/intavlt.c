#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time

#define MAX_ELEM 20
#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

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
}/* end of AVL_Destroy() */


/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root){
	if(root){
		return root->height;
	}
	else return 0;
}/* end of getHeight() */

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root){
	NODE* temp;
	temp = root->left;
	if(temp->right){
		root->left = temp->right;
		root->left->height = max(getHeight(root->left->right), getHeight(root->left->left)) + 1;
	}
	else root->left = NULL;
	temp->right = root;
	
	temp->right->height = max(getHeight(temp->right->right), getHeight(temp->right->left)) + 1;
	temp->height = max(getHeight(temp->right), getHeight(temp->left)) + 1;
	
	
	return temp;
}/* end of rotateRight() */

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root){
	NODE* temp;
	temp = root->right;
	if(temp->left){
		root->right = temp->left;
		root->right->height = max(getHeight(root->right->right), getHeight(root->right->left)) + 1;
	}
	else root->right = NULL;
	temp->left = root;

	temp->left->height = max(getHeight(temp->left->right), getHeight(temp->left->left)) + 1;
	temp->height = max(getHeight(temp->right), getHeight(temp->left)) + 1;
	
	return temp;
}/* end of rotateLeft() */

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr){
	int balance = 0;
	
	if(root==NULL){
		root = newPtr;
		return root;
	}
	if(newPtr->data < root->data){
		root->left = _insert(root->left, newPtr);
		
		root->height = max(getHeight(root->right), getHeight(root->left)) + 1;
		
		if((root->right)==NULL){
			balance = getHeight(root->left) - 0;
		}
		else
			balance = getHeight(root->left) - getHeight(root->right);
		
		if(balance==2){ 
			int ll = 0, lr = 0;
			if(root->left->left) ll = getHeight(root->left->left);
			if(root->left->right) lr = getHeight(root->left->right);
			int l_bal = ll - lr;
			if(l_bal==1){
				//left subtree is left high
				//left-of-left
				root = rotateRight(root);
			}
			else if(l_bal==-1){
				//left subtree is right high
				//right-of-left
				root->left = rotateLeft(root->left);
				root = rotateRight(root);
			}
		}

	}
	else{
		root->right = _insert(root->right, newPtr);
		
		root->height = max(getHeight(root->right), getHeight(root->left)) + 1;
	
		if((root->left)==NULL){
			balance = 0 - getHeight(root->right);
		}
		else
			balance = getHeight(root->left) - getHeight(root->right);

		if(balance==-2){
			int rl = 0, rr = 0;
			if(root->right->left) rl = getHeight(root->right->left);
			if(root->right->right) rr = getHeight(root->right->right);
			int r_bal = rl - rr;
			if(r_bal==-1){
				//right subtree is right high
				//right-of-right
				root = rotateLeft(root); 
			}
			else if(r_bal==1){
				//right subtree is left high
				//left-of-right
				root->right = rotateRight(root->right);
				root = rotateLeft(root);
			}
		}
	
	}
	
	//root->height = max(getHeight(root->right), getHeight(root->left)) + 1;

	return root;
	
}/* end of _insert() */

static NODE *_makeNode( int data){
	NODE* new_node = (NODE*) malloc(sizeof(NODE));
	new_node->data = data;
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->height = 1;
	
	if(!new_node) free(new_node);
	return new_node;
}/* end of _makeNode() */

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, int key){
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

static void _traverse( NODE *root){
	//inorder travaersal
	if(root!=NULL) {
		_traverse(root->left);
		printf("%d ", root->data);
//		printf("%d(%d) ", root->data, root->height);
		_traverse(root->right);
	}/* end of if(root!=NULL) */
}/* end of _traverse() */
	
/* internal traversal function
*/
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

////////////////////////////////////////////////////////////////////////////////

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void){
	AVL_TREE* new_tree = (AVL_TREE*) malloc(sizeof(AVL_TREE));
	new_tree->root = NULL;
	new_tree->count = 0;
	if(!new_tree) {
		free(new_tree);
		return NULL;
	}
	
	return new_tree;
}/* end of AVL_Create() */

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
AVL_TREE *AVL_Destroy( AVL_TREE *pTree){
	if(pTree) _destroy(pTree->root);
	free(pTree);
	return NULL;
}/* end of AVL_Destroy() */

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, int data){
	NODE* new_node = _makeNode(data);
	pTree->root = _insert(pTree->root, new_node);
	(pTree->count)++;
	
	return 1;
}/* end of AVL_Insert() */

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *AVL_Retrieve( AVL_TREE *pTree, int key){
	NODE* nodeRetrieved = _retrieve(pTree->root, key);
	if(nodeRetrieved!=NULL)
		return &(nodeRetrieved->data);
	else
		return NULL;
}/* end of AVL_Retrieve() */

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree){
	_traverse(pTree->root);
	
	return;
}/* end of AVL_Traverse() */

void printTree( AVL_TREE *pTree){
	_infix_print(pTree->root, 0);
	
	return;
}/* end of printTree() */

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	int data;
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 0; i < MAX_ELEM; i++)
	{
      	data = rand() % (MAX_ELEM * 3) + 1; // random number
//		data = i+1; // sequential number
		
		fprintf( stdout, "%d ", data);

		// insert function call
		AVL_Insert( tree, data);
	}
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	AVL_Destroy( tree);

	return 0;
}
