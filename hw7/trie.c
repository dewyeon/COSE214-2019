#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	char 			*entry;
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void){
	TRIE* newTrie = (TRIE*) malloc(sizeof(TRIE));
	if(newTrie){
		for(int i=0; i<MAX_DEGREE; i++) {
			newTrie->subtrees[i] = NULL;
		}
		newTrie->entry = NULL;
		return newTrie;
		}
	else {
		free(newTrie);
		return NULL;
	}
}/* end of trieCreateNode() */

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root){
	int cnt = 0;
	for(int i=0; i<MAX_DEGREE; i++){
		if((root->subtrees[i])!=NULL) cnt++;
	}/* end of for */
	
	if(cnt==0){
		//base case - no child
		if((root->entry)!=NULL){
			free(root->entry);
			root->entry=NULL;
		}
		free(root);
		root=NULL;
	}
	else{
		for(int i=0; i<MAX_DEGREE; i++){
			if((root->subtrees[i])!=NULL){
				trieDestroy(root->subtrees[i]);
			}
		}/* end of for */
		if((root->entry)!=NULL){
			free(root->entry);
			root->entry=NULL;
		}
		free(root);
		root=NULL;
	}/* end of else */
	
}/* end of trieDestroy() */

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str){
	TRIE* temp = root;
	for(int i=0; str[i]!='\0'; i++){
		if((str[i]>64 && str[i]<91)||(str[i]>96 && str[i]<123)||str[i]==EOW) continue;
		else return 0;
	}
	for(int i=0; str[i]!='\0'; i++){
		if(isupper(str[i])) str[i] = tolower(str[i]);
		if(temp->subtrees[getIndex(str[i])]==NULL) { //아래에 노드가 없으면 노드 만듬(TRIE 노드)
			temp->subtrees[getIndex(str[i])] = trieCreateNode();
		}
		temp = temp->subtrees[getIndex(str[i])];
	}/* end of for */
	if(!(temp->entry)) {
		temp->entry = strdup(str);
		return 1;
	}
	else {
		return 0;
	}
}/* end of trieInsert() */

/* Retrieve trie for the requested key
	return	1 key found
			0 key not found
*/
int trieSearch( TRIE *root, char *str){
	TRIE *temp = root;
	for(int i=0; str[i]!='\0'; i++){
		if(isupper(str[i])) str[i] = tolower(str[i]);
		if((str[i]>64 && str[i]<91)||(str[i]>96 && str[i]<123)||str[i]==EOW){
			if((temp->subtrees[getIndex(str[i])])!=NULL){
				temp = temp->subtrees[getIndex(str[i])];
			}
			else return 0;
		}
		else return 0;
	}
	if((temp->entry)==NULL) return 0;
	else return 1;
}/* end of trieSearch() */

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root){
	if(root!=NULL){ //이거 안하니까 seg fault 나옴
		if((root->entry)!=NULL) printf("%s\n", root->entry);
		
		for(int i=0; i<MAX_DEGREE; i++){
			if((root->subtrees[i])!=NULL){
				trieList(root->subtrees[i]);
			}
		}
	}
	return;
}/* end of trieList() */

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
	using trieList function
*/
void triePrefixList( TRIE *root, char *str){
	TRIE* temp = root;
	for(int i=0; str[i]!='\0'; i++){
		if(isupper(str[i])) str[i] = tolower(str[i]);
		if((str[i]>64 && str[i]<91)||(str[i]>96 && str[i]<123)||str[i]==EOW){
			if((temp->subtrees[getIndex(str[i])])!=NULL){
				temp = temp->subtrees[getIndex(str[i])];
			}
			else return;
		}
		else return;
	}
	trieList(temp);
	
}/* end of triePrefixList() */

/* makes permuterms for given str
ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]){
	strcat(str, "$");
	int cnt=0; 
	int len = strlen(str);
	
	for(int i=0; i<len; i++){
		permuterms[i] = strdup(str);
		
		str[len]=str[0];
		str = str+1;
		str[len]='\0';
		cnt++;
	}
	return cnt;
}/* end of make_permuterms() */

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size){
	for(int i=0; i<size; i++){
		free(permuterms[i]);
	}
}/* end of clear_permuterms() */

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str){
	strcat(str, "$");
	int cnt=0;
	int len=strlen(str);
	while(str[len-1]!='*'){
		str[len]=str[0];
		str = str+1;
		str[len]='\0';
		cnt++;
	}
	
	char *search_str = strtok(str, "*");
	
	triePrefixList(root, search_str);
}/* end of trieSearchWildcard() */

/////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	printf( "Inserting to trie...\t");
	while (fscanf( fp, "%s", str) == 1) // words file
	{	
		ret = trieInsert( trie, str);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
				trieInsert( permute_trie, permuterms[i]);
			
			clear_permuterms( permuterms, num_p);
		}
	}
	
	printf( "[done]\n"); // Inserting to trie

	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) == 1)
	{
		if (strchr( str, '*')) // wildcard search term
		{
			trieSearchWildcard( permute_trie, str);
		}
		else // search term
		{
			ret = trieSearch( trie, str);
			printf( "[%s]%s found!\n", str, ret ? "": " not");
		}
		printf( "\nQuery: ");
	}

	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}
