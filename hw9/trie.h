#include <ctype.h> // isupper, tolower
#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void)
{
	TRIE* newTrie = (TRIE*) malloc(sizeof(TRIE));
	if(newTrie){
		for(int i=0; i<MAX_DEGREE; i++) {
			newTrie->subtrees[i] = NULL;
		}
		newTrie->index = -1;
		return newTrie;
		}
	else {
		free(newTrie);
		return NULL;
	}
}/* end of trieCreateNode() */

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root)
{
	int cnt = 0;
	for(int i=0; i<MAX_DEGREE; i++){
		if((root->subtrees[i])!=NULL) cnt++;
	}/* end of for */
	
	if(cnt==0){
		//base case - no child
		free(root);
		root=NULL;
	}
	else{
		for(int i=0; i<MAX_DEGREE; i++){
			if((root->subtrees[i])!=NULL){
				trieDestroy(root->subtrees[i]);
			}
		}/* end of for */
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
int trieInsert( TRIE *root, char *str, int dic_index)
{
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
	if(temp->index == -1) {
		temp->index = dic_index;
		return 1;
	}
	else {
		return 0;
	}
}/* end of trieInsert() */

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str)
{
	TRIE *temp = root;
	for(int i=0; str[i]!='\0'; i++){
		if(isupper(str[i])) str[i] = tolower(str[i]);
		if((str[i]>64 && str[i]<91)||(str[i]>96 && str[i]<123)||str[i]==EOW){
			if((temp->subtrees[getIndex(str[i])])!=NULL){
				temp = temp->subtrees[getIndex(str[i])];
			}
			else return -1;
		}
		else return -1;
	}
	if((temp->index)==-1) return -1;
	else return temp->index;
}/* end of trieSearch() */

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root)
{
	if(root!=NULL){ 
		if((root->index)!=-1) printf("%d\n", root->index);
		
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
void triePrefixList( TRIE *root, char *str)
{
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
int make_permuterms( char *str, char *permuterms[])
{
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
void clear_permuterms( char *permuterms[], int size)
{
	for(int i=0; i<size; i++){
		free(permuterms[i]);
	}
}/* end of clear_permuterms() */

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str)
{
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
}/* end of trieSearchWildCard() */

/* makes a trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2trie( char *dicfile)
{
	TRIE *newTrie = trieCreateNode();
	char str[100];
	int dic_index = 0;
	FILE *fp = fopen( dicfile, "rt");
	if( fp != NULL)
	{
		while (fscanf( fp, "%s", str) == 1)
		{
			trieInsert( newTrie, str, dic_index);
			dic_index++;
		}
		fclose( fp);
		return newTrie;
	}
	else
	{
		printf("File open error: %s\n", dicfile);
		fclose( fp);
		return NULL;
	}
}/* end of dic2trie() */


/* makes a permuterm trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2permute_trie( char *dicfile)
{
	TRIE *newPermuteTrie = trieCreateNode();
	char str[100];
	char *permuterms[100];
	int num_perm;
	int dic_index = 0;
	FILE *fp = fopen( dicfile, "rt");
	if( fp != NULL)
	{
		while(fscanf( fp, "%s", str) == 1)
		{
			num_perm = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_perm; i++)
				trieInsert( newPermuteTrie, permuterms[i], dic_index);
			
			clear_permuterms( permuterms, num_perm);
			dic_index++;
		}/* end of while() */
		fclose( fp);
		return newPermuteTrie;
	}
	else
	{
		fclose( fp);
		return NULL;
	}
}/* end of dic2permute_trie() */
