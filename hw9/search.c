//#define DEBUG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "trie.h"

// 역색인 헤더 정보에 대한 구조체
typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// document frequency
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
static char *rtrim( char *str)
{
	char *p = str + strlen( str) - 1;
	
	while (p >= str)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') *p = 0;
		else return str;
		p--;
	}
}

static char *ltrim( char *str)
{
	char *p = str;
	
	while (*p)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') p++;
		else 
		{
			return p;
		}
	}
	return NULL;
}

static char *trim( char *str)
{
	if( str == NULL || *str == 0) return str;
	
	return rtrim(ltrim(str));
}

////////////////////////////////////////////////////////////////////////////////

// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header( char *filename)
{
	FILE *fp = fopen(filename, "rb");
	
	tHEADER *header_list = (tHEADER*) malloc(sizeof(tHEADER)*30000);
	if(fp != NULL)
	{
		if (fread( header_list, sizeof(tHEADER), 30000, fp))
		{
			fclose( fp);
			return header_list;
		}
		else 
		{
			fclose( fp);
			return NULL;
		}
	}
	else 
	{
		fprintf( stderr, "File open error: %s\n", filename);
		fclose( fp);
		return NULL;
	}
}/* end of load_header() */

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting( char *filename)
{
	FILE *fp = fopen(filename, "rb");
	
	/*just in case
	FILE *fp_2 = fp;
	int filesize;
	fseek(fp_2, 0, SEEK_END);
	filesize = ftell(fp_2);
	int read_size = filesize/sizeof(int);
	printf("read_size = %d\n", read_size);
	*/
	
	int *posting_list = (int*) malloc(sizeof(int)*624456);
	
	if(fp != NULL)
	{
		if (fread( posting_list, sizeof(int), 624456, fp))
		{
			fclose( fp);
			return posting_list;
		}
		else 
		{
			fclose( fp);
			return NULL;
		}
	}
	else 
	{
		fprintf( stderr, "File open error: %s\n", filename);
		fclose( fp);
		return NULL;
	}
}/* end of load_posting */

// 문서 집합을 화면에 출력한다.
void showDocuments( int *docs, int numdocs)
{
	for(int i=0; i<numdocs; i++)
		printf(" %d", docs[i]);
	printf("\n");
}/* end of showDocuments() */

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs)
{
	if(docs==NULL && docs2==NULL) return NULL;
	else if(docs==NULL && docs2!=NULL) { 
		*newnumdocs = numdocs2; 
		int *docs_result = (int*) malloc(sizeof(int)*(*newnumdocs));
		for(int i=0; i<(*newnumdocs); i++) docs_result[i] = docs2[i];
		return docs_result; }
	else if(docs2==NULL && docs!=NULL) { 
		*newnumdocs = numdocs; 
		int *docs_result = (int*) malloc(sizeof(int)*(*newnumdocs));
		for(int i=0; i<(*newnumdocs); i++) docs_result[i] = docs[i];
		return docs_result; }
	else {
	int larger = numdocs;
	if(larger<numdocs2) larger = numdocs2;
	int *docs_result = (int*) malloc(sizeof(int)*larger);
	int cnt1 = 0, cnt2 = 0, cnt3 = 0;
	while(cnt1 < numdocs && cnt2 < numdocs2)
	{
		if(docs[cnt1] == docs2[cnt2])
		{
			docs_result[cnt3] = docs[cnt1];
			cnt1++;
			cnt2++;
			cnt3++;
		}
		else if(docs[cnt1] < docs2[cnt2])
			cnt1++;
		else if(docs[cnt1] > docs2[cnt2])
			cnt2++;
		
		if(cnt1==numdocs || cnt2==numdocs2) break;
	}
	
	*newnumdocs = cnt3;
	return docs_result;
	}
}/* end of intersectDocuments() */

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs)
{
	if(docs==NULL && docs2==NULL) return NULL;
	else if(docs==NULL && docs2!=NULL) { 
		*newnumdocs = numdocs2; 
		int *docs_result = (int*) malloc(sizeof(int)*(*newnumdocs));
		for(int i=0; i<(*newnumdocs); i++) docs_result[i] = docs2[i];
		return docs_result; }
	else if(docs2==NULL && docs!=NULL) { 
		*newnumdocs = numdocs; 
		int *docs_result = (int*) malloc(sizeof(int)*(*newnumdocs));
		for(int i=0; i<(*newnumdocs); i++) docs_result[i] = docs[i];
		return docs_result; }
	else{
	int *docs_result = (int*) malloc(sizeof(int)*(numdocs+numdocs2));
	int cnt1 = 0, cnt2 = 0, cnt3 = 0;
	while(cnt2 < numdocs2 && cnt1 < numdocs)
	{
		if(docs[cnt1] == docs2[cnt2])
		{
			docs_result[cnt3] = docs[cnt1];
			cnt1++;
			cnt2++;
			cnt3++;
		}
		else if(docs[cnt1] < docs2[cnt2])
		{
			docs_result[cnt3] = docs[cnt1];
			cnt3++;
			cnt1++;
		}
		else if(docs[cnt1] > docs2[cnt2])
		{
			docs_result[cnt3] = docs2[cnt2];
			cnt3++;
			cnt2++;
		}
	}
	if(cnt1 == numdocs)
	{
		for(int i=cnt2; i<numdocs2; i++)
		{
			docs_result[cnt3] = docs2[cnt2];	
			cnt3++;
			cnt2++;
		}
	}
	else if(cnt2 == numdocs2)
	{
		for(int i=cnt1; i<numdocs; i++)
		{
			docs_result[cnt3] = docs[cnt1];
			cnt3++;
			cnt1++;
		}
	}
	*newnumdocs = cnt3;
	return docs_result;
	}
}/* end of unionDocuments() */

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *getDocuments( tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs)
{
	int ret;
	int index;
	
	ret = trieSearch( trie, term);
	if( ret != -1)
	{
		index = header[ret].index;
		*(numdocs) = header[ret].df;
		int *docs = (int*) malloc(sizeof(int)*(*numdocs));
		
		for(int i=0; i<*(numdocs); i++)
			docs[i] = posting[index+i];
		
		if(docs==NULL) 
			free(docs);
		return docs;
	}
	else return NULL;
}/* end of getDocuments() */

// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *searchDocuments( tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs)
{
	query = trim(query);
	int *result = NULL;
	
	if(strchr( query, '&')||strchr( query, '|')) // boolean search term exists
	{
		char *and_ptr = strrchr(query, '&');
		char *or_ptr = strrchr(query, '|');
		
		if(and_ptr > or_ptr) // '&' at the rightmost
		{
			char *right_operand = and_ptr + 1;
			*and_ptr = '\0';
			int left_num, right_num;
			int *left_operand_res = searchDocuments( header, posting, trie, query, &left_num);
			int *right_operand_res = getDocuments( header, posting, trie, right_operand, &right_num);
			result = intersectDocuments( left_operand_res, left_num, right_operand_res, right_num, numdocs);
			free(left_operand_res);
			free(right_operand_res);
			return result;
		}
		else if(and_ptr < or_ptr) // '|' at the rightmost
		{
			char *right_operand = or_ptr + 1;
			*or_ptr = '\0';
			int left_num, right_num;
			int *left_operand_res = searchDocuments( header, posting, trie, query, &left_num);
			int *right_operand_res = getDocuments( header, posting, trie, right_operand, &right_num);
			result =  unionDocuments( left_operand_res, left_num, right_operand_res, right_num, numdocs);
			free(left_operand_res);
			free(right_operand_res);
			return result;
		}
			
	}
	else // single search term
	{
		return getDocuments( header, posting, trie, query, numdocs);
	}
	
}/* end of searchDocuments() */

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	tHEADER *header;
	int *posting;
	TRIE *trie;
	char query[100];
	int index;
	
	header = load_header( "header.idx");
	if (header == NULL) return 1;
	
	posting = load_posting( "posting.idx");
	if (posting == NULL) return 1;
		
	trie = dic2trie( "dic.txt");

	printf( "\nQuery: ");
	while (fgets( query, 100, stdin) != NULL)
	{
		int numdocs;
		int *docs = searchDocuments( header, posting, trie, query, &numdocs);
		
		if (docs == NULL) printf( "not found!\n");
		else 
		{
			showDocuments( docs, numdocs);
			free( docs);
		}
		printf( "\nQuery: ");
	}
	
	free( header);
	free( posting);
	trieDestroy( trie);
	
	return 0;
}
