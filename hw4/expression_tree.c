#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *createTree( void){
 TREE *tree_head = (TREE*) malloc(sizeof(TREE));
 if(tree_head){
  tree_head->root = NULL;
 }
 else{
  free(tree_head);
 }
return tree_head;
}/* end of createTree() */

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *destroyTree( TREE *pTree);

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

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE *_makeNode( char ch){
 NODE *newNode = (NODE*) malloc(sizeof(NODE));
 if(newNode){
  newNode->data = ch;
  newNode->left = NULL;
  newNode->right = NULL;
 }
 else{
  free(newNode);
 }
 return newNode;
}/* end of _makeNode() */

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree( char *expr, TREE *pTree){
 NODE* node_stack[MAX_STACK_SIZE];
 int top = -1;
 
 for(int i=0; expr[i]!='\0'; i++){
  //push new node, i.e. make a exp char into a tree node
  NODE* newNode = _makeNode(expr[i]);
   //case) expr char == operator
  if(newNode->data=='+' || newNode->data=='-' || newNode->data=='*' || newNode->data=='/'){
   //pop 2 times
   //1st pop
   if(top!=-1){
	 newNode->right = node_stack[top];
	 top--;
	}
   else{
	 //AN OPERATOR APPEARED BEFORE ANY NUMBER(OPERAND) - INVALID EXP.
	 _destroy(newNode);
	 return 0; 
	}
	//2nd pop
	if(top!=-1){
	 newNode->left = node_stack[top];
	 top--;
	}
   else{
	 //AN OPERAND APPEARED BEFORE ANY NUMBER - INVALID EXP.
	 _destroy(newNode);
	 return 0; 
	}
  }/* end of if(expr[i]~~) */ 
  
  top++;
  node_stack[top] = newNode;
 }/* end of for */

 if(top==0) {
	 pTree->root = node_stack[top];
	 return 1;
 }
 else {
	 for(int i=0; i<=top; i++){
		 _destroy(node_stack[i]);
	 }
	 return 0;
 }
}/* end of postfix2tree() */

/* Print node in tree using inorder traversal
    `
/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse( NODE *root){
	if(root != NULL){
		if(isdigit(root->data)){
			printf("%c", root->data);
		}
		else{
			printf("(");
			_traverse(root->left);
			printf("%c",root->data);
			_traverse(root->right);
			printf(")");
		}
	}
}/* end of _traverse() */

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);

/* internal traversal function
*/
static void _infix_print( NODE *root, int level){
	if(root != NULL){
		if(isdigit(root->data)){
			for(int i=0; i<level; i++){
				printf("	");
			}
			printf("%c", root->data);
		}
		else{
			_infix_print(root->right, level+1);
			for(int i=0; i<level; i++){
				printf("	");
			}
			printf("%c",root->data);
			printf("\n");
			_infix_print(root->left, level+1);
		}
	}
	printf("\n");
}/* end of _infix_print() */

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix( char *expr){
	float expr_stack[MAX_STACK_SIZE];
	int top = -1;
	
	for(int i=0; expr[i]!='\0'; i++){
		if(isdigit(expr[i])){
			char cur_char = expr[i];
			top++;
			expr_stack[top] = atoi(&cur_char);
		}
		else{
			float op1, op2, cur_res;
			op2 = expr_stack[top];
			top--;
			op1 = expr_stack[top];
			top--;
			switch (expr[i]){
				case '+':
					cur_res = op1+op2;
					break;
				case '-':
					cur_res = op1-op2;
					break;
				case '*':
					cur_res = op1*op2;
					break;
				case '/':
					cur_res = op1/op2;
					break;
			}/* end of switch-case */
			top++;
			expr_stack[top] = cur_res;
		}
	}/* end of for */
	return expr_stack[top];
}/* end of evalPostfix() */

////////////////////////////////////////////////////////////////////////////////
TREE *destroyTree( TREE *pTree)
{
	if (pTree)
	{
		_destroy( pTree->root);
	}
		
	free( pTree);
	
	return NULL;	
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse (pTree->root);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();
		
	    if (!tree)
		{
		    printf( "Cannot create tree\n");
			return 100;
		}
		
		// postfix expression -> expression tree
		int ret = postfix2tree( expr, tree);
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			destroyTree(tree);
			continue;
		}
		
		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);
		
		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);
		
		// destroy tree
		destroyTree( tree);
		
		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
