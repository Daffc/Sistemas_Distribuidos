/*
    Programa: 
        Tarefa 3: Cada processo mantém localmente o vetor State[N]. Inicializa o State[N] com -1 (indicando estado “unknown”) para todos 
        os demais processos e 0 para o próprio processo. Nesta tarefa ao executar um teste, o processo atualiza a entrada correspondente 
        no vetor State[N]. Em cada intervalo de testes, mostre o vetor State[N]. 

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
        "Sistemas Distribuidos 2021/ERE4: Trabalho Prático 0, Tarefa 3.\n"
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

    schedule(FAULT, 0.0, 1);   // O processo 1 falha no tempo 0.
    schedule(REPAIR, 31.0, 1);  // O processo 1 recupera no tempo 31.

    // Escalonando todos os nodos, exceto o nodo 0 para falharem em tempo 40 até 70.
    for(i = 1; i < N; i++){
        schedule(FAULT, 40.0, i);   
        schedule(REPAIR, 70.0, i);
    }



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
                nodo[token].state[next] = status(nodo[next].id);
                
                // Equanto nodo indicado por 'next' não estiver correto e não for o nodo atual (token)
                while(status(nodo[next].id) && (next != token)){

                    // Imprime que nodo 'next' esta FALHO.
                    printf("[%3.1f] O nodo [%d] testa o nodo [%d] FALHO.\n", time(), token, next);

                    // Calcula id nodo próximo a 'next' e atualiza entrada de state de nodo 'next'.
                    next = (next + 1) % N;
                    nodo[token].state[next] = status(nodo[next].id);
                }

                if(next != token){
                    printf("[%3.1f] O nodo [%d] testa o nodo [%d] CORRETO.\n", time(), token, next);
                }
                else{
                    printf("[%3.1f] O nodo [%d] não encontrou nenhum outro nodo correto.\n", time(), token);
                }

                // Imprime vetor 'state' de nodo 'token'.
                printf("\tVetor 'state' de nodo [%d]:\n", token);
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
    exit(0);
}

