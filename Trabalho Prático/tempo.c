/*
    TRABALHO PRÁTICO: 
        Especificação: Implemente a versão definitiva (2) do algoritmo distribuído VCube no ambiente de simulação SMPL, e mostre resultados para diversos valores de N e diversos eventos - um evento em um 
        processo de cada vez, um evento só ocorre depois do evento anterior ser diagnosticado. Para cada evento mostre claramente o número de testes executados e a latência para completar o diagnóstico do evento. 
        Cada nodo mantém o vetor STATE[0..N-1] de contadores de eventos, inicializado em -1 (estado “unknown”). Assume-se que os processos são inicializados sem-falha, a entrada correspondente ao próprio processo 
        no vetor STATE[] do testador é setada para zero. Ao descobrir um novo evento em um nodo testado, o testador incrementa a entrada correspondente no vetor STATE[].

        Implemente o VCube de forma que em cada intervalo de testes todos os logN clusters. são testados.

        Para a transferência de informações de diagnóstico, faça da seguinte maneira: ao testar um processo sem-falha o testador lê o vetor State[] do processo testado e obtém informações sobre novidades. 
        É importante comparar as entradas correspondentes dos vetores STATE (testador e testado) para saber se o testado tem alguma novidade. Se o valor da entrada for maior no vetor STATE do processo testado, 
        então copia a informação.

        Atenção: Data para disponibilizar o trabalho: 15 de novembro de 2020 (3 semanas de prazo!)

        Os alunos devem informar por e-mail a URL do trabalho, usando o subject "TP SISDIS ERE4-2021"

        O trabalho deve ser feito individualmente!

        Deve ser feita uma página Web, que contém:

            1.  Relatório HTML explicando como o trabalho foi feito (use desenhos, palavras, o que você quiser): o objetivo é detalhar as suas decisões para implementar seu trabalho.
            2.  Código fonte dos programas, comentados. ATENÇÃO: acrescente a todo programa a terminação ".txt" para que possa ser diretamente aberto em um browser. Exemplos: cliente.py.txt ou servidor.c.txt
            3.  Log dos testes executados: mostre explicitamente diversos casos testados, lembre-se é a partir desta listagem de testes que o professor vai medir até que ponto o trabalho está funcionando.
                Veja este programa exemplo para ilustrar a criação de um bom log. 


        Autor: Douglas Affonso Clementino
        Data da última modificação: 25/10/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"
#include "cisj.h"

#define T_SIMULACAO 120.0 // Define o tempo total da simuação.

//  ------------- EVENTOS -------------
#define TEST 1      // Um processo testa outro processo.
#define FAULT 2     // Um processo correto falha.
#define REPAIR 3    // Um processo falho é corrigido.

//  ------ DESCRITOR DO NODO SMPL -----
typedef struct{
    int id;     // Identificador da facility SMPL.
    int *state;
} tnodo;

tnodo *nodo;

int main (int argc, char *argv[]){
    static int  N,          // Número total de processos, recebido por linha de comando.
                token,      // O processo que "está executando" em um instante de tempo.
                event,      // Evento.
                r,
                s,
                i,
                j,
                next,
                qtn_cluster,
                qtn_nodos,
                *tests;

    static char fa_name[5];  // Nome da facility.

    if(argc != 2){
        puts("Uso correto: 'tempo <num-nodos>'.");
        exit(1);
    }

    N = atoi(argv[1]);      // Recuperando quantidade de nodos da entrada de argumentos.

    // Imprimindo header de log.
    printf(
        "Sistemas Distribuidos 2021/ERE4: TRABALHO PRÁTICO.\n"
        "Autor: Douglas Affonso Clementino. *Data da última alteração 29/10/2021.\n"
        "Este Programa foi executado com N=%d Processos.\n",
        N
    );

    // Calcula e armazena a quantidade de clusersde acordo com a quantidade total de nodos.
    qtn_cluster = log2(N) + 1;

    // Armazenando vetor de para retorno da função cisj.
    tests = (int *) malloc ((N / 2) * sizeof(int));

    smpl(0, "Um exemplo de simulação"); // Definindo Identificado para simulação.
    reset();                            // Iniciando / reinicinado simulação.
    stream(1);                          // Definindo 1 fluxo para simuação.
    
    // Inicialização de variáveis.
    nodo = (tnodo *) malloc(N * sizeof(tnodo)); // Allocando espaço para N nodos.

    // Redefinindo nome de nodos.
    for(i = 0; i < N; i++){
        memset(fa_name, 0, 5);
        sprintf(fa_name, "%d", i);
        nodo[i].id = facility(fa_name, 1);
    }

    // Allocanto e definindo vetor 'state' para cada nodo.
    for(i = 0; i < N; i++){
        nodo[i].state = (int *) malloc(N * sizeof(int));
        memset(nodo[i].state, -1, N * sizeof(int));
        nodo[i].state[i] = 0;
    }

    // Escalonando testes para todos os nodos executarem no tempo 30.
    for(i = 0; i < N; i++){
        schedule(TEST, 30.0, i);
    }


    // Loop de simulação.
    while(time() < T_SIMULACAO){
        cause(&event, &token);

        switch(event){

            case TEST:

                // Calcula id de primeiro nodo que será testará por nodo atual (token) e atualiza entrada de state de nodo 'next'.       
                printf("[%3.1f] O nodo [%d] é testado por:\n", time(), token);
                for(s = 1; s <= qtn_cluster; s++){
                    qtn_nodos = cisj(token, s, -1, tests);

                    // Percorre todos os nodos de cluster
                    for(i=0; i < qtn_nodos; i++){
                        // Verifica se identificador de nodo é válido (Casoso em que N não é uma potência de 2).
                        if(tests[i] < N){
                            // Busca primeiro nodo de cluster sem falha (testador);
                            if(status(nodo[tests[i]].id) == 0){
                                printf("\t c(%d, %d): %d\n", token, s, tests[i]);
                                break;
                            }
                        }
                    }
                }

                schedule(TEST, 30.0, token);
                break;

            case FAULT:
                r = request(nodo[token].id, token, 0);
                if(r != 0){
                    puts("Não foi possível falhar o nodo...");
                    break;
                }
                printf("[%3.1f] O nodo [%d] falhou.\n", time(), token);
                break;

            case REPAIR:
                release(nodo[token].id, token);
                printf("[%3.1f] O nodo [%d] recuperou.\n", time(), token);
                schedule(TEST, 30.0, token);
                break;
        }
    }

    // Liberando memória alocada.
    for(i = 0; i < N; i++){
        free(nodo[i].state);
    }
    free(nodo);
    free(tests);
    exit(0);
}