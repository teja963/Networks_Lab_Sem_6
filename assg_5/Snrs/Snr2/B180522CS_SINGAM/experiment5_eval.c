#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<stdbool.h>

struct Edge
{
	int src,dest,wgt;
};

typedef struct Graphs
{
	int V,E;
	struct Edge *edges;
}*Graph;

Graph create(int v,int e)
{
	Graph graph = (Graph)malloc(sizeof(struct Graphs));	
	graph->V = v;
	graph->E = e;
	graph->edges = malloc(e*sizeof(struct Edge));
	return graph;
}

typedef struct routeTable
{
	int dist[50];
	int pred[50];
}rt;

rt *routes;

int minDist(int dist[],bool set[],int V)
{
	int min = INT_MAX,index;
	for(int v=0;v<V;v++)
	{
		if(set[v]==false&&dist[v]<min)
		{
			min = dist[v];
			index = v;
		}
	}
	return index;
}

int dijkstra(Graph g,int s,bool active[])
{
	int V=g->V, e=g->E;
	bool set[V];

	for(int i=0;i<V;i++)
	{
		set[i] = false;
		routes[s].dist[i] = INT_MAX;
		routes[s].pred[i] = s;
	}

	routes[s].dist[s] = 0;

	for(int i=0;i<V-1;i++)
	{
		int minNode = minDist(routes[s].dist,set,V);
		set[minNode] = true;
		for(int j=0;j<e;j++)
		{
			int u = g->edges[j].src;
			int v = g->edges[j].dest;
			int w = g->edges[j].wgt;

			if(u == minNode) 
			{
				if(active[v] == true && set[v]==false && routes[s].dist[v] > w + routes[s].dist[u])
				{
					routes[s].dist[v] = w + routes[s].dist[u];
					routes[s].pred[v] = u;
				}
			}	
			else if(v == minNode)
			{
				if(active[u] == true && set[u]==false && routes[s].dist[u] > w + routes[s].dist[v])
				{
					routes[s].dist[u] = w + routes[s].dist[v];
					routes[s].pred[u] = v;
				}
			}
		}
	}	
}

int main()
{
	int v,e,s=0;
	scanf("%d %d",&v,&e);
	Graph graph = create(v,e);
	int x,y,z;
	for(int i=0;i<e;i++)
	{
		scanf("%d %d %d",&x,&y,&z);
		graph->edges[i].src = x;
		graph->edges[i].dest = y;
		graph->edges[i].wgt = z;
	}
	
	routes = (rt *)malloc(1*sizeof(rt));
	
	int drop;
	scanf("%d",&drop);	

	bool active[v];

	for(int i=0;i<v;i++)
	{
		if(drop != i)
			active[i]=true;
		else
			active[i]=false;
	}

	
	if(drop == 0 || drop == v-1)
	{
		printf("-1\n");
		exit(1);
	}

	dijkstra(graph,0,active);

	if( routes[s].dist[v-1] == INT_MAX)
		printf("-1\n");
	else
		printf("%d\n", routes[s].dist[v-1]);

}





