/*
    Header File: 
        Arquivo utilizado pelo programa 'trabPrat.c', nele estão discritas a estrutura 'node_set' e função 'cisj', 
		utilizada para definir ordem de nodos clusters de testes / testadores de acordo com lógica VCube para teste e  diagnóstico.

    Autor: Douglas Affonso Clementino
    Data da última modificação: 29/10/2021
*/

#ifndef __CISJ__

	#define __CISJ__

	#define POW_2(num) (1<<(num))
	#define VALID_J(j, s) ((POW_2(s-1)) >= j)

	/* |-- node_set.h */
	typedef struct node_set {
		int* nodes;
		ssize_t size;
		ssize_t offset;
	} node_set;

	/*
		CISI function where 'i' is id of the interested node, 's' is the consuted cluster, and 'j' position of the correpondent node in he cluster 's'.
		If j=-1, 'ret' will gatter all the nodes of cluser 's' for node 'i' in the rightous order reurning the quantity of listed nodes, otherise, it will recover the node that ocupies the 'j' position of 's' node, returning 1. 
	*/
	int cisj(int i, int s, int j, int * ret);

#endif
