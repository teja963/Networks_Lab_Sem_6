#include<stdio.h>
#include<stdlib.h>
struct route_table{
	unsigned dist[100];
	unsigned from[100];
}rt[100];

int main(){
	int nodes, links, count = 0;
	scanf("%d %d", &nodes, &links);
	int cost[100][100];
	for(int i = 1; i <= nodes; i++){			//Initializing values
		for(int j = 1; j <= nodes; j++){
			cost[i][j] = 9999;
		}
	}
	
	for(int i = 0; i < links; i++){                          //Preparing cost matrix from user input
		int a,b,c;
		scanf("%d %d %d", &a, &b, &c);
		cost[a][a] = 0;
		cost[a][b] = c;
		cost[b][a] = c;	
		
	}
	
	for(int i = 1; i <= nodes; i++){                        //route_table contains all the info abt other nodes
		for(int j = 1; j <= nodes; j++){
			rt[i].dist[j] = cost[i][j];
			rt[i].from[j] = j;
		}
	}
	
	do{							//Bellman ford algorithm
		count = 0;
		for(int i = 1; i <= nodes; i++){
			for(int j = 1; j <= nodes; j++){
				for(int k = 1; k <= nodes; k++){
					if(rt[i].dist[j] > rt[i].dist[k] + rt[k].dist[j]){
						rt[i].dist[j] = rt[i].dist[k] + rt[k].dist[j];
						rt[i].from[j] = k;
						count++;
					} 
				}
			}
		}
		
		
	}while(count != 0);
	
	for(int i = 1; i <= nodes; i++){
		printf("\nFor router: %d\n", i);
		for(int j = 1; j <= nodes; j++){
			printf("Node: %d via %d distance %d\n", j, rt[i].from[j], rt[i].dist[j]);
		}
	}
	
	return 0;
}
