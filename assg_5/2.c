#include<stdio.h>
#include<stdlib.h>
void PRINT(int index, int prefered[], int start_node){
	if(index == start_node)return;
	PRINT(prefered[index], prefered, start_node);
	printf(" -> %d", index);
}

void Dijkstra(int cost_matrix[][100], int nodes, int start_node){
	int distance[100], visited[100], prefered[100];
	int count , min_distance, next_node;
	
	for(int i = 1; i <= nodes; i++){			//Initializing the values of distance, visited, prefered
		distance[i] = cost_matrix[start_node][i];
		prefered[i] = start_node;
		visited[i] = 0;
	}
	
	distance[start_node] = 0;
	visited[start_node] = 1;
	count = 1;                                             //Already got 1 path which is starting so need to traverse remaining node- 1
	
	
	while(count < nodes - 1){
		min_distance = 9999;
		
		for(int i = 1; i <= nodes; i++){               //This function will find the min_distance of all nodes and next_node to traverse
			if(min_distance > distance[i] && !visited[i]){
				min_distance = distance[i];
				next_node = i;
			}
		}
		
		visited[next_node] = 1;
		for(int i = 1; i <= nodes; i++){
			if(!visited[i]){
				if(distance[i] > min_distance + cost_matrix[next_node][i]){
					distance[i] = min_distance + cost_matrix[next_node][i];
					prefered[i] = next_node;
				}
			}
		}
		count++;
	}
	
	
	for(int i = 1; i <= nodes; i++){
			printf("\nDistance from %d to %d:  %d\n",start_node, i, distance[i]);
			if(i == start_node){
				printf("Path: %d -> %d", start_node, start_node);	
			}
			else{
				printf("Path: %d", start_node);
			}
			PRINT(i, prefered, start_node);
			
	}
	
	
	
}
int main(){
	int nodes, links, count = 0;
	scanf("%d %d", &nodes, &links);
	int cost_matrix[100][100];
	for(int i = 1; i <= nodes; i++){			//Initializing values
		for(int j = 1; j <= nodes; j++){
			cost_matrix[i][j] = 9999;
		}
	}
	
	for(int i = 0; i < links; i++){                          //Preparing cost matrix from user input
		int a,b,c;
		scanf("%d %d %d", &a, &b, &c);
		cost_matrix[a][a] = 0;
		cost_matrix[a][b] = c;
		cost_matrix[b][a] = c;	
		
	}
	
	for(int i = 1; i <= nodes; i++){			//Printng  for every node
		Dijkstra(cost_matrix, nodes, i);
		printf("\n");
	}
	
	return 0;
}
