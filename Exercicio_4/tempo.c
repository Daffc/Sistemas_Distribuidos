/*
    Programa: 
         Tarefa 4: Quando um processo correto testa outro processo correto obtém as informações de diagnóstico 
         do processo testado sobre todos os processos do sistema exceto aqueles que testou nesta rodada, além do próprio testador. 

    Autor: Douglas Affonso Clementino
    Data da última modificação: 25/10/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "../libs/smpl.h"

#define T_SIMULACAO 120.0 // Define o tempo total da simuação.

//  ------------- EVENTOS -------------
#define TEST 1      // Um processo testa outro processo.
#define FAULT 2     // Um processo correto falha.
#define REPAIR 3    // Um processo falho é corrigido.

//  ------------- ESTADOS -------------
#define CORRETO 0   // Estado de um processo correto.
#define FALHO 1     // Estado de um processo falho.


//  ------ DESCRITOR DO NODO SMPL -----
typedef struct{
    int id;     // Identificador da facility SMPL.
    int *state;
} tnodo;

tnodo *nodo;

int main (int argc, char *argv[]){
    static int  N,              // Número total de processos, recebido por linha de comando.
                token,          // O processo que "está executando" em um instante de tempo.
                event,          // Evento.
                node_counter,   // Utilizado para contar a quantiadade de nodos testados por um node durante uma rodada.
                r,
                i,
                j,
                next;

    static char fa_name[5];  // Nome da facility.

    if(argc != 2){
        puts("Uso correto: 'tempo <num-nodos>'.");
        exit(1);
    }

    N = atoi(argv[1]);      // Recuperando quantidade de nodos da entrada de argumentos.

    // Imprimindo header de log.
    printf(
        "Sistemas Distribuidos 2021/ERE4: Trabalho Prático 0, Tarefa 4.\n"
        "Autor: Douglas Affonso Clementino. *Data da última alteração 29/10/2021.\n"
        "Este Programa foi executado com N=%d Processos.\n",
        N
    );

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

    // Allocanto e definindo valor inicial 0 em entradas vetor 'state' de cada nodo.
    for(i = 0; i < N; i++){
        nodo[i].state = (int *) malloc(N * sizeof(int));
        memset(nodo[i].state, 0, N * sizeof(int));
    }

    // Escalonando testes para todos os nodos executarem no tempo 30.
    for(i = 0; i < N; i++){
        schedule(TEST, 30.0, i);
    }

    schedule(FAULT, 31.0, 1);   // O processo 1 falha no tempo 31.
    schedule(REPAIR, 61.0, 1);  // O processo 1 recupera no tempo 61.

    // Loop de simulação.
    while(time() < T_SIMULACAO){
        cause(&event, &token);

        switch(event){

            case TEST:
                // Caso o testador estiver falho, sair.
                if (status(nodo[token].id) != 0)    
                    break; 

                // Calcula id de primeiro nodo que será testado por nodo atual e atualiza entrada de state de nodo 'next'.
                next = (token + 1) % N;
                
                // Zera a contagem de nodos testados. 
                node_counter = 0;

                // Equanto nodo indicado por 'next' não estiver correto e não for o nodo atual (token)
                while(status(nodo[next].id) && (next != token)){

                    // incrementa o contador de nodos testados.
                    node_counter ++;

                    // Imprime que nodo 'next' esta FALHO.
                    printf("O nodo %d testa o nodo %d FALHO no tempo %3.1f.\n", token, next, time());

                    // Caso anteriormente nodo 'next' estivesse CORRETO, incrementar a sua entrada em vetor 'state'.
                    if((nodo[token].state[next] % 2) == 0){
                        nodo[token].state[next] ++;
                    }

                    // Calcula id nodo próximo a 'next' e atualiza entrada de state de nodo 'next'.
                    next = (next + 1) % N;
                    nodo[token].state[next] = status(nodo[next].id);
                }

                if(next != token){
                    printf("O nodo %d testa o nodo %d CORRETO no tempo %3.1f.\n", token, next, time());

                    // Caso anteriormente nodo 'next' estivesse FALHO, incrementar a sua entrada em vetor 'state'.
                    if((nodo[token].state[next] % 2) == 1){
                        nodo[token].state[next] += 1;
                    }
                    node_counter ++;
                    
                    // Transferindo nodos não testados ( N - node_counter ) a partir de nodo 'next', de vetor 'state' de nodo 'next' para 'state' de nodo 'token'.
                    for(i = 1; i < (N - node_counter); i++){

                        printf("\tRecuperando estado de node [%d] de vector state de node %d.\n", (next + i) % N, next);
                        nodo[token].state[(next + i) % N] = nodo[next].state[(next + i) % N];
                    }
                }
                else{
                    printf("O nodo %d não encontrou nenhum outro nodo correto no tempo %3.1f.\n", token, time());
                }

                // Imprime vetor 'state' de nodo 'token'.
                printf("\tVetor 'state' de nodo %d em tempo %3.1f:\n", token, time());
                printf("\t");
                for(i = 0; i < N; i++){
                    printf("[%d] = %d; ", i, nodo[token].state[i]);
                }
                printf("\n");

                schedule(TEST, 30.0, token);
                break;

            case FAULT:
                r = request(nodo[token].id, token, 0);
                if(r != 0){
                    puts("Não foi possível falhar o nodo...");
                    break;
                }
                printf("O nodo %d falhou no tempo %3.1f.\n", token, time());
                break;

            case REPAIR:
                release(nodo[token].id, token);
                printf("O nodo %d recuperou no tempo %3.1f.\n", token, time());
                schedule(TEST, 30.0, token);
                break;
        }
    }

    // Liberando memória alocada.
    for(i = 0; i < N; i++){
        free(nodo[i].state);
    }
    free(nodo);
    exit(0);
}

