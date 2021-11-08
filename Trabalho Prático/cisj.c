#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cisj.h"

static node_set*
set_new(ssize_t size)
{
	node_set* new;

	new = (node_set*)malloc(sizeof(node_set));
	new->nodes = (int*)malloc(sizeof(int)*size);
	new->size = size;
	new->offset = 0;
	return new;
}

static void
set_insert(node_set* nodes, int node)
{
	if (nodes == NULL) return;
	nodes->nodes[nodes->offset++] = node;
}

static void
set_merge(node_set* dest, node_set* source)
{
	if (dest == NULL || source == NULL) return;
	memcpy(&(dest->nodes[dest->offset]), source->nodes, sizeof(int)*source->size);
	dest->offset += source->size;
}

static void
set_free(node_set* nodes)
{
	free(nodes->nodes);
	free(nodes);
}
/* node_set.h --| */

node_set*
cis(int i, int s)
{
	node_set* nodes, *other_nodes;
	int xor = i ^ POW_2(s-1);
	int j;

	/* starting node list */
	nodes = set_new(POW_2(s-1));

	/* inserting the first value (i XOR 2^^(s-1)) */
	set_insert(nodes, xor);

	/* recursion */
	for (j=1; j<=s-1; j++) {
		other_nodes = cis(xor, j);
		set_merge(nodes, other_nodes);
		set_free(other_nodes);
	}
	return nodes;
}


/*
	CISI function where 'i' is id of the interested node, 's' is the consuted cluster, and 'j' position of the correpondent node in he cluster 's'.
	If j=-1, 'ret' will gatter all the nodes of cluser 's' for node 'i' in the rightous orderm, otherise, i will recover the node that ocupies he 'j' position of 's' node. 
*/
int cisj(int i, int s, int j, int * ret)
{
	int qtn_nodes;
	node_set* nodes;

	/* have we the third parameter? */
	if (j != -1) {
		if (!VALID_J(j, s)) {
			printf("%d is not a valid node for a cluster size %d.\n", j, s);
			exit(1);
		}
	}

	/* calculating cis */
	nodes = cis(i, s);

	qtn_nodes = nodes->size;

	/* print all nodes or just one? */
	if (j != -1){
		*ret = nodes->nodes[j-1];
		set_free(nodes);
		return 1;
	}

	else {
		for (i=0; i < nodes->size; i++)
			*(ret + i) = nodes->nodes[i];
		
		set_free(nodes);
		return qtn_nodes;
	}
}

