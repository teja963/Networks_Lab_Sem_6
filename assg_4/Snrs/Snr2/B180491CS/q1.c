// DVR USING BELLMAN FORD ALGO: 
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /*for a from b to c = a b c*/
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
void bellmanFord(int V, int E, int Graph[V][V], int thru[V][V], int final[V][V])
{
    for(int i = 0;i < V ; i++)
    {
        for(int j = 0 ; j < V ; j++)
        {
            final[i][j]=Graph[i][j];
            thru[i][j]=i;
            for(int k = 0; k < V ; k ++ )
            {
                if ( ( ( final[i][j] > Graph[k][j]+Graph[i][k] ) || (final[i][j] == -1) ) && (  Graph[k][j] != -1 && Graph[i][k] != -1 ) )
                {
                        final[i][j] = Graph[k][j]+Graph[i][k] ;
                        thru[i][j] = k;
                }
            }
            for(int k = 0; k < V ; k++)
            {
                if( ((final[i][k]!=-1)&&(final[k][j]!=-1)) && (final[i][j]==-1) )
                {
                        final[i][j]=final[i][k]+final[k][j];
                        thru[i][j]=k;       
                }        
            }
        }
    }
}
void displayTable(int V, int final[V][V], int thru[V][V], int Graph[V][V])
{
    int temp;
    for (int i = 0; i < V ; i++)
    {
        printf("For the vertex: %d \n", i);
        printf("T\tW\tPred\n");
        for(int j = 0;j < V; j++)
        {
            (i == thru[i][j] ) ? ( temp = -1) : (temp = thru[i][j]) ;
            (Graph[i][j] != -1 && temp == -1) ? ( printf("%d\t%d \t%d \n", j, final[i][j], j )) : ( printf("%d\t%d\t%d \n", j, final[i][j], temp));
        }
        printf("\n");
    }
}
int main()
{
    int E, V;
    scanf("%d %d",&V, &E);
    int Graph[V][V], Table[V][V][V], final[V][V], thru[V][V];
    memset(thru, -1, sizeof(thru[0][0]) * V * V);
    memset(Graph, -1, sizeof(Graph[0][0]) * V * V);
    memset(Table, -1, sizeof(Table[0][0][0]) * V * V * V);
    addStuff(V, E, Graph);
    bellmanFord(V, E, Graph, thru, final);
    displayTable(V, final, thru, Graph);
}