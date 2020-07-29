#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// 토큰-문서 구조체
typedef struct {
	char	*token;	// 토큰
	int		docid;	// 문서번호(document ID)
} tTokenDoc;

typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// 문서 빈도(document frequency)
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
// 토큰 구조체로부터 역색인 파일을 생성한다.
void invertedIndex( tTokenDoc *tokens, int num_tokens, char *dicfilename, char *headerfilename, char *postingfilename)
{
	FILE *fp_dic = fopen(dicfilename, "wt");
	FILE *fp_header = fopen(headerfilename, "wb");
	FILE *fp_posting = fopen(postingfilename, "wb");
	
	tHEADER header_list[30000];
	memset(header_list, 0, sizeof(tHEADER));
	int *posting_list = (int*) malloc(sizeof(int)*num_tokens);
	int freq = 1;
	int cnt_header = 0; //counter for header_list
	int cnt_posting = 0; //counter for posting_list
	
	if(fp_dic != NULL)
	{
		for(int i=0; i<num_tokens; i++)
		{	
			if((i!=0) && (strcmp(tokens[i-1].token, tokens[i].token)==0))
			{
				if(tokens[i-1].docid != tokens[i].docid)
				{
					posting_list[cnt_posting] = tokens[i].docid;
					cnt_posting++;
					freq++;
				}
				else continue;
			}
			else if((i==0) && (strcmp(tokens[i].token, tokens[i+1].token)==0))
			{
				if(tokens[i].docid != tokens[i+1].docid)
				{
					posting_list[cnt_posting] = tokens[i].docid;
					cnt_posting++;
					fprintf( fp_dic, "%s\n", tokens[i].token);
				}
				else continue;
			}
		
			else
			{
				posting_list[cnt_posting] = tokens[i].docid;
				cnt_posting++;
				fprintf( fp_dic, "%s\n", tokens[i].token);
				
				
				tHEADER newHeader;
				newHeader.index = cnt_posting-1-freq;
				newHeader.df = freq;
				header_list[cnt_header] = newHeader;
				//printf("%s \t%d: %d\n", tokens[i].token, header_list[cnt_header].index, header_list[cnt_header].df);
				cnt_header++;
				
				if(freq > 1)	freq = 1;
			}
		}/* end of for() */
		//last dic token
		cnt_posting++;
		tHEADER newHeader;
		newHeader.index = cnt_posting-1-freq;
		newHeader.df = freq;
		header_list[cnt_header] = newHeader;
		cnt_header++;
		cnt_posting--;
		//printf("%d: %d\n", header_list[cnt_header].index, header_list[cnt_header].df);
	}
	else
	{
		fprintf( stderr, "File open error: %s\n", dicfilename);
		fclose(fp_dic); fclose(fp_header); fclose(fp_posting);
		free(posting_list);
	}
	//printf("%d: %d\n", header_list[cnt_header-1].index, header_list[cnt_header-1].df);
	//printf("%d\n", posting_list[header_list[cnt_header-1].index]);
	//printf("%d\n", cnt_header); //25611
	//printf("%d\n", cnt_posting); //403856
	fwrite( header_list, sizeof(tHEADER), cnt_header, fp_header);
	fwrite( posting_list, sizeof(int), cnt_posting, fp_posting);
	
	fclose(fp_dic);
	fclose(fp_header);
	fclose(fp_posting);
	
	free(posting_list);
}/* end of invertedIndex() */

// 입력 파일로부터 토큰-문서 구조체를 생성한다.
tTokenDoc *get_tokens(char *filename, int *num_tokens)
{
	FILE *fp = fopen(filename, "rt");
	char a_file[5000];
	tTokenDoc *token_list = (tTokenDoc*) malloc(sizeof(tTokenDoc)*625000);
	
	int file_num = 0;
	int i = 0;
	
	if(fp != NULL)
	{
		while(fgets(a_file, 5000, fp)!=NULL)
		{
			int len = strlen(a_file);
			a_file[len-1] = '\0';
			
			file_num++;
			char *word = strtok(a_file, " ");
			while(word != NULL)
			{
				tTokenDoc newTokenDoc;
				newTokenDoc.token = strdup(word);
				newTokenDoc.docid = file_num;
				token_list[i] = newTokenDoc;
				
				word = strtok(NULL, " ");
				i++;
			}
		}/* end of while */
	}/* end of if */
	else
	{
		fprintf( stderr, "File open error: %s\n", filename);
		fclose(fp);
		return NULL;
	}
	
	*num_tokens = i;
	
	fclose(fp);
	return token_list;
}/* end of get_tokens() */

// qsort를 위한 비교함수 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
static int _compare(const void *n1, const void *n2)
{	
	char *token1 = (*(tTokenDoc*) n1).token;
	char *token2 = (*(tTokenDoc*) n2).token;
	
	if(strcmp(token1, token2) > 0)
		return 1;
	else if(strcmp(token1, token2) <0)
		return -1;
	else
	{
		int docid1 = (*(tTokenDoc*) n1).docid;
		int docid2 = (*(tTokenDoc*) n2).docid;
		
		if(docid1 > docid2) return 1;
		else if (docid1 < docid2) return -1;
	}
}/* end of compare() */

////////////////////////////////////////////////////////////////////////////////
static void print_tokens( tTokenDoc *tokens, int num_tokens)
{
	int i;
	
	for (i = 0; i < num_tokens; i++)
	{
		printf( "%s\t%d\n", tokens[i].token, tokens[i].docid);
	}
}

////////////////////////////////////////////////////////////////////////////////
void destroy_tokens( tTokenDoc *tokens, int num_tokens)
{
	int i;
	
	for (i = 0; i < num_tokens; i++)
	{
		free( tokens[i].token);
	}
	free( tokens);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	tTokenDoc *tokens;
	int num_tokens = 0;

	if (argc != 2)
	{
		printf( "Usage: %s FILE\n", argv[0]);
		return 2;
	}
	
	tokens = get_tokens( argv[1], &num_tokens);
	
	assert( tokens != NULL && num_tokens > 0);

#if DEBUG	
	//print_tokens( tokens, num_tokens);
#endif

	// 정렬 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
	qsort( tokens, num_tokens, sizeof( tTokenDoc), _compare);
	
	invertedIndex( tokens, num_tokens, "dic.txt", "header.idx", "posting.idx");

#if DEBUG	
	print_tokens( tokens, num_tokens);
#endif	

	destroy_tokens( tokens, num_tokens);
	
	return 0;
}

