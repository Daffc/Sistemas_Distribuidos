/*
    Programa: 
        Tarefa 1: Fazer cada um dos processos testar o seguinte no anel. 
        Implemente o teste com a função status() do SMPL e imprimir (printf) o resultado de cada teste executado. 
        Por exemplo: “O processo i testou o processo j correto no tempo tal.” 

    Autor: Douglas Affonso Clementino
    Data da última modificação: 29/10/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "../libs/smpl.h"

#define T_SIMULACAO 100.0 // Define o tempo total da simuação.

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

    // Imprimindo header de log.
    printf(
        "Sistemas Distribuidos 2021/ERE4: Trabalho Prático 1, Tarefa .\n"
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
            
                // Calcula id de nodo que será testado por nodo atual.
                next = (token + 1) % N;

                // Verifica se state de processo 'next', imprimindo teste e resultado (FALHO ou CORRETO) em log o resultado.
                if(status(nodo[next].id) != 0){
                    printf("[%3.1f] O nodo [%d] testa o nodo [%d] FALHO.\n", time(), token, next);
                }
                else{
                    printf("[%3.1f] O nodo [%d] testa o nodo [%d] CORRETO.\n", time(), token, next);
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
    
    free(nodo);
    exit(0);
}

