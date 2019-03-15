#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/item.h"
#include "lib/net.h"


#define NIL		-1
#define CONNECTED	1
#define MIN(A, B) ((A) < (B) ? (A) : (B))

int time;

Net read_input() {
	int N, M, i;

	if (scanf("%d", &N) != 1) {
		fprintf(stderr,"Error reading input.");
	}

	if (scanf("%d", &M) != 1) {
		fprintf(stderr,"Error reading input.");
	}

	Net net = net_alloc(N);

	for (i = 0; i < M; i++) {
		int u;
		int v;
		if (scanf("%d", &u) != 1 || scanf("%d", &v) != 1) {
			fprintf(stderr, "Error reading file.");
		}
		net_add_connection(net, u, v);
	}

	return net;
}

int DFS_visit(Net net, Item u, int low[]) {
	time++;
	u->_d = time;
	u->_color = GRAY;
    low[u->_id - 1] = time;
	Vector adjs_vector;
	int i, children = 0;

	adjs_vector = get_adjacents(net, u);		
	int val = u->_id;
	for (i = 0; i < vector_size(adjs_vector); i++) {		
		Item v = vector_at(adjs_vector,i);
		if (v->_color == WHITE) {
			children++;
			v->_pi = u->_id;
			int tmp = DFS_visit(net, v, low);
			if (tmp > val) val = tmp;

            low[u->_id - 1] = MIN(low[u->_id - 1], low[v->_id - 1]);   

	        /*  u is not root and low of one of its children is more than u's */
	        if (u->_pi != NIL && low[v->_id -1] >= u->_d) {
	           	net_add_art_point(net, u);
	        }
        
            /*  u is root and has 2 or more children */
	        if (u->_pi == NIL && children > 1) {
	          	net_add_art_point(net, u);
	         }

		} else if (v->_id != u->_pi) { /* if the vertex isn't where it came from */
            low[u->_id -1] = MIN(low[u->_id - 1], v->_d);
        }
	}
	u->_color = BLACK;
	time++;
	u->_f = time;

	return val;
}



Vector DFS(Net net) {
	int i, N = net->_n_routers;
	Item* items = net->_routers_vec->_item_array;
    int * low = calloc(sizeof(int),N);
	Vector subnets_maxs = create_vector(1);
 
	for (i = 0; i < N; i++) {		
		items[i]->_color = WHITE;
		items[i]->_pi = NIL;
		low[i] = NIL;
	}

	time = 0;
	for (i = 0; i < N; i++) {		
		if (items[i]->_color == WHITE)
			vector_push_back(subnets_maxs, create_item(DFS_visit(net, items[i], low)));
	}
	free(low);
	return subnets_maxs;
}


void print_results(Vector subnets_net, Net net, int biggest_size) {
	int i;
	/* Number of subnets */
	printf("%d\n", vector_size(subnets_net));
	/* Id's of subnets */
	vector_sort(subnets_net, item_id_sort);
	for (i = 0; i < vector_size(subnets_net); i++) {
		printf("%d ", vector_at(subnets_net, i)->_id);
	}
	printf("\n");
	delete_vector(subnets_net, delete_item);

	/* Number of points that are dangerous */
	printf("%d\n", net_get_N_art_points(net));

	/* Biggest size of sub net */
	printf("%d", biggest_size);
}


int main() {
	Net net = read_input();   

	Vector subnets_net = DFS(net);
	int z;
	printf("Subnets: ");
	for (z = 0; z < vector_size(subnets_net); z++)
		printf("%d ", vector_at(subnets_net, z)->_id);
	printf("\n");
	/*  build new net without the ap's and do the DFS */
	Net new_net = net_create_remove_articulations(net);

	printf("Subnets: ");
	for (z = 0; z < vector_size(subnets_net); z++)
		printf("%d ", vector_at(subnets_net, z)->_id);
	printf("\n");


	delete_vector(DFS(new_net), delete_item);
	int i, biggest_size = 0;
	for (i = 0; i < new_net->_n_routers; i++) {		
		Item item = vector_at(new_net->_routers_vec,i);
		if (item->_pi == NIL)
			if ((item->_f / 2) - item->_d + 1 > biggest_size)
				biggest_size = (item->_f / 2) - item->_d + 1;
	}

	print_results(subnets_net, net, biggest_size);

	/* clean up */
    delete_net(net);
	/*delete_net(new_net);
	*/

	return 0;
}