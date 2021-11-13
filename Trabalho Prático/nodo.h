
/*
    Header File: 
        Arquivo utilizado pelo programa 'trabPrat.c', nele estão discritas a estrutura 'tnodo' e macros relacionadas ao controle dos nodos.

    Autor: Douglas Affonso Clementino
    Data da última modificação: 29/10/2021
*/

#ifndef __NODO__
    #define __NODO__

    //  ------------- EVENTOS -------------
    #define TEST 1      // Um processo testa outro processo.
    #define FAULT 2     // Um processo correto falha.
    #define REPAIR 3    // Um processo falho é corrigido.

    //  ------------- ESTADOS -------------
    #define CORRETO 0       // Um processo testa outro processo.
    #define FALHO 1         // Um processo correto falha.
    #define DESCONHECIDO -1 // Um processo falho é corrigido.

    //  ------ DESCRITOR DO NODO SMPL -----
    typedef struct{
        int id;         // Identificador da facility SMPL.
        int *state;     // Vector State de cada nodo
        int *testes;    // Vetor contendo testes que nodo deve realizar (atualizado a cada novo evento identificado).
        int qnt_testes; // Quantidade de nodos que devem ser testador (indicados por '*testes').
    } tnodo;

#endif