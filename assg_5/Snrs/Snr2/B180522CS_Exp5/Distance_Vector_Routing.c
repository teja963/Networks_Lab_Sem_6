#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

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
};

typedef struct routeTable
{
	int dist[50];
	int pred[50];
}rt;

rt *routes;

void belman(Graph g,int s)
{
	int V=g->V,e = g->E;
	
	for(int i=0;i<V;i++)
	{
		routes[s].dist[i] = INT_MAX;
		routes[s].pred[i] = s;
	}
	routes[s].dist[s] = 0;

	for(int i=0;i<V-1;i++)
	{
		for(int j=0;j<e;j++)
		{
			int u = g->edges[j].src;
			int v = g->edges[j].dest;
			int w = g->edges[j].wgt;
			if(routes[s].dist[v] > w + routes[s].dist[u] && routes[s].dist[u] != INT_MAX)
			{
				routes[s].dist[v] = w + routes[s].dist[u];
				routes[s].pred[v] = u;
			}
			if(routes[s].dist[u] > w + routes[s].dist[v] && routes[s].dist[v] != INT_MAX)
			{
				routes[s].dist[u] = w + routes[s].dist[v];
				routes[s].pred[u] = v;
			}
		}
	}
}

void print_path(int s, int v)
{
	if(v!=s)
	{
		print_path(s, routes[s].pred[v]);
		printf("->%d", v);
	}
	else
		printf("%d", v);
}

int main()
{
	int v,e,s;
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

	routes = (rt *)malloc(v*sizeof(rt));
	for(int i=0;i<v;i++)
		belman(graph, i);

	printf("\nRouting Tables using Distance vector routing\n");
	for(int i=0;i<v;i++)
	{
		printf("\nFor router at node %d\n", i);
		printf("\tnode\tdist\tpred\tpath\n");
		for(int j=0;j<v;j++)
		{
			printf("\t%d\t%d\t%d\t", j, routes[i].dist[j], routes[i].pred[j]);
			print_path(i, j);	
			printf("\n");
		}
	}
}
