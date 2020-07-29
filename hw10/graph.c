#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
/*
void make_graph(int **f_graph, int graph_size)
{
	f_graph = (int **) malloc(sizeof(int*) * graph_size);
	for(int i=0; i<graph_size; i++)
	{
		f_graph[i] = (int*) malloc(sizeof(int) * graph_size);
		for(int j=0; j<graph_size; j++) f_graph[i][j] = 0;
	}
	
}/* end of make_graph() */

void data_into_graph(int **f_graph, int from, int to)
{
	//index 주의!!!
	f_graph[from-1][to-1] = 1;
	f_graph[to-1][from-1] = 1;
}/* end of data_into_graph() */

void destroy_graph(int **f_graph, int graph_size)
{
	if(f_graph)
	{
		for(int i=0; i<graph_size; i++)
		{
			if(f_graph[i]) free(f_graph[i]);
		}
		free(f_graph);
	}
}/* end of destroy_graph() */

void DFS(int **f_graph, int graph_size)
{
	int* stack = (int*) malloc( sizeof(int) * graph_size); //stack - for nodes getting in and out
	int top = -1;
	int walkPtr = 0;
	int* processed = (int*) malloc( sizeof(int) * graph_size);
	for(int i=0; i<graph_size; i++) processed[i] = 0;
	
	if(!f_graph) return;
	
	printf("DFS : ");
	for(int walkPtr=0; walkPtr<graph_size; walkPtr++)
	{
		if(processed[walkPtr] == 0) //not processed yet
		{
			processed[walkPtr] = 1; //put in the stack
			top++;
			stack[top] = walkPtr;
		}
		if(top!=-1)
		{
			while(top!=-1)
			{
				int top_value = stack[top];
				top--; //pop
				processed[top_value] = 2;
				printf("%d ", top_value+1);
				
				for(int j=0; j<graph_size; j++) //방금 pop한 pop_value와 연결된 노드들 확인, push
				{
					if(f_graph[top_value][j]==1)
					{
						if(processed[j] == 0)
						{
							processed[j] = 1;
							top++;
							stack[top] = j;
						}/* end of if */
					}/* end of if */
				}/* end of for */
			}/* end of while */
		}/* end of if */
	}/* end of for */
	
	printf("\n");
	
	free(stack);
	free(processed);
	return;
}/* end of DFS() */

void BFS(int **f_graph, int graph_size)
{
	int* queue = (int*) malloc( sizeof(int) * graph_size); //queue - for nodes getting in and out
	int front = -1, rear = 0;
	int walkPtr = 0;
	int* processed = (int*) malloc( sizeof(int) * graph_size);
	for(int i=0; i<graph_size; i++) processed[i] = 0;
	
	if(!f_graph) return;
	
	printf("BFS : ");
	for(int walkPtr=0; walkPtr<graph_size; walkPtr++)
	{
		if(processed[walkPtr] == 0) //not processed yet
		{
			processed[walkPtr] = 1; //put in the queue
			front++;
			queue[front] = walkPtr;
		}
		if(front!=-1)
		{
			while(front>=rear) //not empty queue
			{
				int rear_value = queue[rear];
				rear++; //dequeue
				processed[rear_value] = 2;
				printf("%d ", rear_value+1);
				
				for(int j=0; j<graph_size; j++) //방금 pop한 pop_value와 연결된 노드들 확인, push
				{
					if(f_graph[rear_value][j]==1)
					{
						if(processed[j] == 0)
						{
							processed[j] = 1;
							front++;
							queue[front] = j;
						}/* end of if */
					}/* end of if */
				}/* end of for */
			}/* end of while */
		}/* end of if */
	}/* end of for */
	
	printf("\n");
	
	free(queue);
	free(processed);
	return;
}/* end of BFS() */

//////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{	
	char str[100];
	int vertex_num; //number of vertices
	int edge_from, edge_to; //edge = (from, to)
	FILE *fp;
	int **graph;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if(fp == NULL)
	{ 
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	fscanf( fp, "%s %d", str, &vertex_num);
	//make_graph(graph, vertex_num);
	
	graph = (int **) malloc(sizeof(int*) * vertex_num);
	for(int i=0; i<vertex_num; i++)
	{
		graph[i] = (int*) malloc(sizeof(int) * vertex_num);
		for(int j=0; j<vertex_num; j++) graph[i][j] = 0;
	}
	
	fscanf( fp, "%s", str);
	
	while( fscanf( fp, "%d %d", &edge_from, &edge_to) == 2){
		data_into_graph(graph, edge_from, edge_to);
	}
	
	fclose( fp);
	
	DFS(graph, vertex_num);
	BFS(graph, vertex_num);
	
	destroy_graph(graph, vertex_num);
	
	return 0;
}/* end of main() */