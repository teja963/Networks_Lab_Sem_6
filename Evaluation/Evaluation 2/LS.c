#include<stdio.h>
#include<limits.h>

int dijkstra();
int min();

int main() {
	int r, e;
	scanf("%d%d", &r, &e);

	int adj_mat[r][r];
	int i, j, k, l;

	for(i = 0; i < r; i++)
		for(j = 0; j < r; j++)
			adj_mat[i][j] = INT_MAX >> 1;
	
	for(i = 0; i < e; i++) {
		scanf("%d%d%d", &j, &k, &l);
		adj_mat[j][k] = adj_mat[k][j] = l;
	}

	int rp;
	scanf("%d", &rp);

	for(i = 0; i < r; i++)
		adj_mat[i][rp] = adj_mat[rp][i] = INT_MAX >> 1;

	dijkstra(r, 0, adj_mat);

}

int dijkstra(int n, int source, int adj_mat[n][n]) {
	int dist[n], boolean[n];
	int i, j, k;

	for(i = 0; i < n; i++) {
		dist[i] = INT_MAX >> 1;
		boolean[i] = 1;
	}

	dist[source] = 0;

	for(i = 0; i < n; i++) {
		j = min(dist, boolean, n);
		boolean[j] = 0;

		for(k = 0; k < n; k++)
			if(boolean[k] && dist[k] > dist[j] + adj_mat[j][k])
				dist[k] = dist[j] + adj_mat[j][k];
	}

	if(dist[n - 1] != INT_MAX / 2)
		printf("%d\n", dist[n - 1]);
	else
		printf("-1\n");
}

int min(int *dist, int *boolean, int n) {
	int min = INT_MAX, min_dist = INT_MAX;

	for(int i = 0; i < n; i++)
		if(boolean[i] && min_dist > dist[i]) {
			min = i;
			min_dist = dist[i];
		} 

	return min;
}