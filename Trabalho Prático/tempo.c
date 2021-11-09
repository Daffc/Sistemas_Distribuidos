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

typedef struct{
    int nodo;       // ID de Nodo que sofreu o evento.
    int type;       // Tipo de Evento que esta sendo gerenciado
    int r_inicio;   // Rodada em que evento foi sofrido.
    int r_fim;      // Rodada em que evento foi diagnosticado.
    real t_inicio;  // Tempo em que eveto foi sofrido.
    real t_fim;     // Tempo em que evendo foi diagnosticado.
    int *aletados;  // Vetor que lista todos os nodos que estão cientes de evento, (utilizado para verificar se evento foi diagnosticado).
} evento;

tnodo *nodo;


void defineTestes(tnodo *nodo, int id, int qnt_nodos){
    int s;
    int i;
    int j;
    int *testador;
    int qtn_cis;
    int qtn_cluster;

    // Calcula e armazena a quantidade de clusersde acordo com a quantidade total de nodos.
    qtn_cluster = log2(qnt_nodos) + 1;

    // Armazenando vetor de para retorno da função cisj.
    testador = (int *) malloc ((qnt_nodos / 2) * sizeof(int));
    
    // Zera quantidade de testes que devem ser realizados pelo nodo 'id'.
    nodo[id].qnt_testes = 0;

    // Para cada cluster 's' de todos os N nodos.
    for (s = 1; s <= qtn_cluster; s++){

        // intera sobre todos os 'N' nodos.
        for(i = 0; i < qnt_nodos; i++){

            // Caso nodo 'i' seja diferente de nodo testador 'id' (não se auto-testar).
            if(i != id){

                // Recupere lista ordenada de testadores de nodo 'i' em cluster 's' (função cisj).
                qtn_cis = cisj(i, s, -1, testador);

                // Percorre todos os possíveis testadores de nodo 'i' em ordem de função cisj.                         
                for(j = 0; j < qtn_cis; j++){
                    
                    // Verifica se nodo testador[j] existe (casos em que 'qnt_nodos' não é potência de 2) e se o estado de nodo 'testador[j]' é DESCONHECIDO ou CORRETO.
                    if((testador[j] < qnt_nodos) && ((nodo[id].state[testador[j]] == DESCONHECIDO) || (nodo[id].state[testador[j]] %2 == CORRETO))){

                        // Verifica se primeiro nodo válido e correto encontrado é o nodo 'id'.                                    
                        if(testador[j] == id){
                            nodo[id].testes[nodo[id].qnt_testes] = i;   // Adiciona nodo 'i' a lista de testes de nodo 'id'.
                            nodo[id].qnt_testes ++;                     // Incrementa o contador de nodos testados por nodo 'id'.
                        }
                        // Nodo correto encontrado, ecerrar loop sobre lista 'cis'.
                        break;
                    }
                }
            }
        }
    }

    free(testador);
}

// Atualiza entrada de vector state de nodo 'n_testador' na lista 'nodo' com informações referentes a nodo 'n_testado', levando em consideração 
// o estado de 'n_testado' (CORRETO ou FALHO) e a comparação de entradas de ambos os vector states.  
void atualizaVectorState(tnodo *nodo, int n_testador, int n_testado, int qnt_nodos){
    int i;

    // Caso nodo 'n_testado' esteja correto.
    if(status(nodo[n_testado].id) == 0){
        // Verifica se é a primeira vez que nodo 'n_testado' está sendo observado por nodo 'n_testador' direta ou indiretamente.
        if(nodo[n_testador].state[n_testado] == -1)
            nodo[n_testador].state[n_testado] = 0;
        
        // Caso nodo 'n_testado' conste como FALHO em vector state de nodo 'n_testador', atualizar entrada 'n_testado'.
        if(nodo[n_testador].state[n_testado] % 2 == 1){
            nodo[n_testador].state[n_testado] += 1;
            defineTestes(nodo, n_testador, qnt_nodos);
        }

        // Comparando Voctor States de nodos 'n_testador' e 'n_testado' e atualizando 'n_testador'.
        for(i = 0; i < qnt_nodos; i++){
            // Caso 'i' seja diferente dos nodos envolvidos.
            if((i != n_testador) && (i != n_testado)){
                // Caso entrada 'i' de vector state de nodo 'n_testador' tenha valor menor (desatualizado) comparado a entrada 'i' de vector state de nodo 'n_testado', 
                // atualizar entrada 'i' vector state de nodo 'n_testador' com entrada 'i' de vector state de nodo 'n_testado'.
                if (nodo[n_testador].state[i] < nodo[n_testado].state[i])
                    nodo[n_testador].state[i] = nodo[n_testado].state[i];
                    defineTestes(nodo, n_testador, qnt_nodos);
            }
        }
    }
        
    // Caso nodo 'n_testado' esteja falho.
    else{
        // Verifica se é a primeira vez que nodo 'n_testado' está sendo observado por nodo 'n_testador' direta ou indiretamente.
        if(nodo[n_testador].state[n_testado] == -1){
            nodo[n_testador].state[n_testado] = 1;
            defineTestes(nodo, n_testador, qnt_nodos);
        }
        
        // Caso nodo 'n_testado' conste como CORRETO em vector state de nodo 'n_testador', atualizar entrada 'n_testado'.
        if(nodo[n_testador].state[n_testado] % 2 == 0){
            nodo[n_testador].state[n_testado] += 1;
            defineTestes(nodo, n_testador, qnt_nodos);
        }
            
    }
}

// Imprime Vector State de 'nodo[id]'
void imprimeVectorState(tnodo *nodo, int id, int qnt_nodos){
    int i;

    printf("\t\tVector State de nodo [%d]: ", id);
    for(i = 0; i < qnt_nodos; i++){
        printf("[%d] = %d; ", i, nodo[id].state[i]);
    }
    printf("\n");
}

// Nodo 'n_testador' testa nodo 'n_testado', chamando a função 'atualizaVectorState' a atualizando vector state de 'n_testador' de acordo com o estado de 'n_testado'.
void testarNodo(tnodo *nodo, int n_testador, int n_testado, int qnt_nodos){
    
    // Atualizando vector state de nodo 'n_testador' de acordo com o estado e com o vector state de 'n_testado'.
    atualizaVectorState(nodo, n_testador, n_testado, qnt_nodos);

    // Caso nodo 'n_testado' esteja CORRETO.
    if(status(nodo[n_testado].id) == 0){
        printf("\tNodo [%d] testa o nodo [%d] CORRETO.\n", n_testador, n_testado);
        imprimeVectorState(nodo, n_testador, qnt_nodos);
    }
    // Caso nodo 'n_testado' esteja FALHO.
    else{
        printf("\tNodo [%d] testa o nodo [%d] FALHO.\n", n_testador, n_testado);
    }

}

int main (int argc, char *argv[]){
    static int  N,          // Número total de processos, recebido por linha de comando.
                token,      // O processo que "está executando" em um instante de tempo.
                event,      // Evento.
                r,
                s,
                i,
                j,
                next,
                qtn_nodos,
                *testador;

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

    // Allocanto e definindo vetor 'state' e vetor de 'testes' para cada nodo.
    for(i = 0; i < N; i++){
        nodo[i].state = (int *) malloc(N * sizeof(int));
        nodo[i].testes = (int *) malloc(N * sizeof(int));
        memset(nodo[i].state, -1, N * sizeof(int));
        nodo[i].state[i] = 0;
    }

    
    for(i = 0; i < N; i++){
        schedule(TEST, 30.0, i);                // Escalonando testes para todos os nodos executarem no tempo 30.
        defineTestes(nodo, i, N);  // Calculando os testes que cada um dos nodos deverá relaizar.
    }

    schedule(FAULT, 31.0, 1);   // O processo 1 falha no tempo 0.
    schedule(REPAIR, 61.0, 1);  // O processo 1 recupera no tempo 31.

    // Loop de simulação.
    while(time() < T_SIMULACAO){
        cause(&event, &token);

        switch(event){

            case TEST:

                // Caso o testador estiver falho, sair.
                if (status(nodo[token].id) != 0)    
                    break; 

                printf("[%3.1f] O nodo [%d] inica testes:\n", time(), token);

                for(i = 0; i < nodo[token].qnt_testes; i++){
                    testarNodo(nodo, token, nodo[token].testes[i], N);
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
        free(nodo[i].testes);
    }
    free(nodo);
    exit(0);
}