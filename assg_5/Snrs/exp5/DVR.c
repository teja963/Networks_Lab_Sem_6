#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

typedef struct graph_node Node;
typedef struct graph_edge Edge;

struct graph_node {
	int no;
	int d;
	struct graph_node *p;
};

struct graph_edge {
	int w;
	Node *u;
	Node *v;
};

void ISS(Node *NodeList, unsigned n, unsigned s) {	//Initialise Single Source 

	for(unsigned i=0; i<n; ++i) {
		NodeList[i].d = INT_MAX;
		NodeList[i].p = NULL;
	}
	NodeList[s].d = 0;
	NodeList[s].p = &NodeList[s];
}

void RELAX(Node *u, Node *v, int w) {
	if( u->d != INT_MAX)
		if( (u->d + w) < v->d ) {
			v->d = u->d + w;
			v->p = u;
		}
}

bool BELLMAN_FORD(Node *NodeList, unsigned n, Edge *EdgeList, unsigned m, unsigned s) {

	ISS(NodeList, n, s);

	for(unsigned i=0; i<n-1; ++i) {
		for(unsigned j=0; j<m; ++j) {
			RELAX(EdgeList[j].u, EdgeList[j].v, EdgeList[j].w);
			RELAX(EdgeList[j].v, EdgeList[j].u, EdgeList[j].w);
		}
	}

	for(unsigned i=0; i<m; ++i) {
		if( EdgeList[i].v->d > EdgeList[i].u->d + EdgeList[i].w )
			return false;

		if( EdgeList[i].u->d > EdgeList[i].v->d + EdgeList[i].w )
			return false;
	}

	return true;
}

void printNext(Node *src, Node * dst) {

	if(dst->p==NULL)
		printf("Not Reachable");
	else {
		while(dst->p !=src) 
			dst = dst->p;
		printf("%d", dst->no);
	}

}

void traceRoute(Node *dst) {

	if(dst->p == NULL) {
		printf("Not Reachable");
	}else if(dst->p == dst) {
		printf("%d ", dst->no); 
	}
	else {
		traceRoute(dst->p);
		printf("%u ", dst->no);
	}
}

int main() {

	unsigned n, m;	//n - no of nodes, m - no of links

	scanf("%u %u", &n, &m);

	Node *NodeList = (Node*) malloc( n * sizeof(Node));
	Edge *EdgeList = (Edge*) malloc( m * sizeof(Edge));

	for(unsigned i=0; i<n; ++i) {
		NodeList[i].no = i;
	}

	for(unsigned i=0; i<m; ++i) {
		unsigned u, v, w;
		scanf("%u %u %u", &u, &v, &w);
		EdgeList[i].w = w;
		EdgeList[i].u = &(NodeList[u]);
		EdgeList[i].v = &(NodeList[v]);
	}

	for(unsigned i=0; i<n; ++i) {
		
		if(!BELLMAN_FORD(NodeList, n, EdgeList, m, i)) {
			printf("\n Graph contains negative weighted edge\n");
			break;
		}

		printf("\nRouting Table for Node : %u\n", i);

		printf(" Dst \tNext \tCost \tPath");
		for(unsigned j=0; j<n; ++j) {
			printf("\n %u \t",j); printNext(NodeList+i, NodeList+j);
			printf("\t%d \t", NodeList[j].d); traceRoute(NodeList+j);
		}
		printf("\n");
	}

	return 0;
}
