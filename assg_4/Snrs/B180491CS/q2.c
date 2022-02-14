#include<stdio.h>
#include <string.h>
#define INFINITY 9999

void addStuff(int V, int E, int Graph[V][V])
{
    int src, dst, weight;
    for (int i = 0; i < V; i++) Graph[i][i] = 0;
    for(int i=0;i<E;i++)
    {
        scanf("%d %d %d",&src, &dst, &weight);
        Graph[src][dst] = weight;
        Graph[dst][src] = weight;
    }
}

void dijikstra(int V,int G[V][V],  int src)
{
	int cost[V][V], distance[V], pred[V];
	int visited[V], count, min_dist, nextnode, i,j;
	for(i=0;i < V;i++)
		for(j=0;j < V;j++)
			if(G[i][j]==0)
				cost[i][j]=INFINITY;
			else
				cost[i][j]=G[i][j];
	
	for(i=0;i< V;i++)
	{
		distance[i]=cost[src][i];
		pred[i]=src;
		visited[i]=0;
	}
	distance[src]=0;
	visited[src]=1;
	count=1;
	while(count < V-1){
		min_dist=INFINITY;
		for(i=0;i < V;i++)
			if(distance[i] < min_dist&&!visited[i])
			{
				min_dist=distance[i];
				nextnode=i;
			}
		visited[nextnode]=1;
		for(i=0;i < V;i++)
			if(!visited[i])
				if(min_dist+cost[nextnode][i] < distance[i])
				{
					distance[i]=min_dist+cost[nextnode][i];
					pred[i]=nextnode;
				}
			count++;
	}
 
	for(i=0;i < V;i++)
		if(i!=src)
		{
			j=i;
			while(pred[j] != src)
				j = pred[j];
			printf("%d\t%d\t%d\n",i, distance[i],j);
		}
		else
			printf("%d\t0\t%d\n",i,i);
}
 
 
void main(){
 	int V, E;
  	scanf("%d %d", &V, &E);
	int G[V][V], i, j, n, u, dst, src, weight;
 	memset(G, 0, sizeof(G[0][0]) * V * V);
	addStuff(V, E, G);
  	for(int i = 0; i < V; i++)
	{
		printf("From the source: %d \n", i);
		printf("To\tWt\tPred\n");
		dijikstra(V,G,i);
		printf("\n");
	}
}
