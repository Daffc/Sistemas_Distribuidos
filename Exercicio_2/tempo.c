/*
    Programa: 
        Tarefa 2: Cada processo correto executa testes até achar outro processo correto. 
            Lembre-se de tratar o caso em que todos os demais processos estão falhos. 
            Imprimir os testes e resultados. 

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
} tnodo;

tnodo *nodo;

int main (int argc, char *argv[]){
    static int  N,          // Número total de processos, recebido por linha de comando.
                token,      // O processo que "está executando" em um instante de tempo.
                event,      // Evento.
                r,
                i,
                next;

    static char fa_name[5];  // Nome da facility.

    if(argc != 2){
        puts("Uso correto: 'tempo <num-nodos>'.");
        exit(1);
    }

    N = atoi(argv[1]);      // Recuperando quantidade de nodos da entrada de argumentos.


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

                // Calcula id de primeiro nodo que será testado por nodo atual.
                next = (token + 1) % N;

                while(status(nodo[next].id) && (next != token)){

                    // Verifica se state de processo 'next', imprimindo teste e resultado (FALHO ou CORRETO) em log o resultado.
                    if(status(nodo[next].id) != 0){
                        printf("O nodo %d testa o nodo %d FALHO no tempo %3.1f.\n", token, next, time());
                    }

                    // Calcula id nodo próximo a 'next'.
                    next = (next + 1) % N;
                }

                if(next != token){
                    printf("O nodo %d testa o nodo %d CORRETO no tempo %3.1f.\n", token, next, time());
                }
                else{
                    printf("O nodo %d testa não encontrou nenhum outro nodo correto no tempo %3.1f.\n", token, time());
                }

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

    free(nodo);
    exit(0);
}

