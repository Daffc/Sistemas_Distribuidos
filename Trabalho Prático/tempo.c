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
#include "nodo.h"
#include "evento.h"

#define T_SIMULACAO 120.0 // Define o tempo total da simuação.

tnodo *nodo;

// Função chamada toda a vez que o nodo correto 'id' indentifica um novo evento, atualizando seu vetor de testes 'testes'.
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
    testador = (int *) malloc(qnt_nodos * sizeof(int));
    
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


// Imprime Vector State de 'nodo[id]'
void imprimeVectorState(tnodo *nodo, int id, int qnt_nodos){
    int i;

    printf("\tVector State de nodo [%d]: {", id);
    for(i = 0; i < qnt_nodos-1; i++){
        printf("%d, ", nodo[id].state[i]);
    }
    printf("%d}\n", nodo[id].state[i]);
}

// Nodo 'n_testador' testa nodo 'n_testado', chamando a função 'atualizaVectorState' a atualizando vector state de 'n_testador' de acordo com o estado de 'n_testado'.
void testarNodo(tnodo *nodo, int n_testador, int n_testado, int qnt_nodos, tevento *evento){
    int i;

    // Caso nodo 'n_testado' esteja CORRETO.
    if(status(nodo[n_testado].id) == CORRETO){
        printf("\tNodo [%d] testa o nodo [%d] CORRETO.\n", n_testador, n_testado);
        
        // Verifica se é a primeira vez que nodo 'n_testado' está sendo observado por nodo 'n_testador' direta ou indiretamente.
        if(nodo[n_testador].state[n_testado] == DESCONHECIDO)
            nodo[n_testador].state[n_testado] = CORRETO;
        
        // Caso nodo 'n_testado' conste como FALHO em vector state de nodo 'n_testador', atualizar entrada 'n_testado'.
        if(nodo[n_testador].state[n_testado] % 2 == FALHO){
            nodo[n_testador].state[n_testado] += 1;
            identificaEvento(n_testador, n_testado, evento, qnt_nodos);
            defineTestes(nodo, n_testador, qnt_nodos);  // Novo evento identificado, atualiazar vetor de testes de nodo 'testador'.
        }

        // Comparando Voctor States de nodos 'n_testador' e 'n_testado' e atualizando 'n_testador'.
        for(i = 0; i < qnt_nodos; i++){
            // Caso 'i' seja diferente dos nodos envolvidos.
            if((i != n_testador) && (i != n_testado)){
                // Caso entrada 'i' de vector state de nodo 'n_testador' tenha valor menor (desatualizado) comparado a entrada 'i' de vector state de nodo 'n_testado', 
                // atualizar entrada 'i' vector state de nodo 'n_testador' com entrada 'i' de vector state de nodo 'n_testado'.
                if (nodo[n_testador].state[i] < nodo[n_testado].state[i]){
                    printf("\t\tRecuperando entrada [%d].\n", i);
                    nodo[n_testador].state[i] = nodo[n_testado].state[i];
                    identificaEvento(n_testador, i, evento, qnt_nodos);        
                    defineTestes(nodo, n_testador, qnt_nodos);          // Novo evento identificado, atualiazar vetor de testes de nodo 'testador'.
                }
            }
        }
    }
    // Caso nodo 'n_testado' esteja FALHO.
    else{
        printf("\tNodo [%d] testa o nodo [%d] FALHO.\n", n_testador, n_testado);

        // Verifica se é a primeira vez que nodo 'n_testado' está sendo observado por nodo 'n_testador' direta ou indiretamente.
        if(nodo[n_testador].state[n_testado] == DESCONHECIDO){
            nodo[n_testador].state[n_testado] = FALHO;
            identificaEvento(n_testador, n_testado, evento, qnt_nodos);
            defineTestes(nodo, n_testador, qnt_nodos);  // Novo evento identificado, atualiazar vetor de testes de nodo 'testador'.
        }
        
        // Caso nodo 'n_testado' conste como CORRETO em vector state de nodo 'n_testador', atualizar entrada 'n_testado'.
        if(nodo[n_testador].state[n_testado] % 2 == CORRETO){
            nodo[n_testador].state[n_testado] += 1;
            identificaEvento(n_testador, n_testado, evento, qnt_nodos);
            defineTestes(nodo, n_testador, qnt_nodos);  // Novo evento identificado, atualiazar vetor de testes de nodo 'testador'.
        }
    }

}

// Define Agendamento de eventos da simualação.
void agendarEventos(){
    schedule(FAULT, 31.0, 1);   // O processo 1 falha no tempo 0.
    // schedule(REPAIR, 61.0, 1);  // O processo 1 recupera no tempo 31.
}

// Imprime informações dos nodos antes de iniciar simulação.
void imprimeInicializacao(tnodo *nodo, int qnt_nodos){
    int i,
        j;

    printf("Definições Iniciais de nodos:\n");
    for(i = 0; i < qnt_nodos; i++){
        printf("\tNodo [%d]:\n", i);
        printf("\t\tEstado: %d\n", status(nodo[i].id));

        printf("\t\tVetor de Testes: {");
        for(j = 0; j < nodo[i].qnt_testes - 1; j++){
            printf("%d, ", nodo[i].testes[j]);
        }
        printf("\t\tVector State: {");
        for(j = 0; j < qnt_nodos - 1; j++){
            printf("%d, ", nodo[i].state[j]);
        }
        printf("%d}\n", nodo[i].state[j]);
    }
    printf("\n\n");
}

// Inicializa estrutura 'tevento' de acordo com a quantidade de nodos 'qnt_nodos'.
void inicializaEvento(tevento *evento, int qnt_nodos){
    evento->diagnosticando = 0;                              // Indica que evento atual já foi diagnosticado (uma vez que não existe evento).
    evento->alertados = (char *) malloc(qnt_nodos);          // Alocando espaço para vetor de nodos alertados sobre evento.
    evento->rodada_completa = (char *) malloc(qnt_nodos);    // Alocando espaço para vetor de nodos com rodada completa.
}

int main (int argc, char *argv[]){
    static int  N,          // Número total de processos, recebido por linha de comando.
                token,      // O processo que "está executando" em um instante de tempo.
                event,      // Evento.
                r,
                i;

    static char fa_name[5]; // Nome da facility.

    tevento evento;         // Estrutura para gerenciamento de evento.
    

    if(argc != 2){
        puts("Uso correto: 'tempo <num-nodos>'.");
        exit(1);
    }

    N = atoi(argv[1]);      // Recuperando quantidade de nodos da entrada de argumentos.

    // Imprimindo header de log.
    printf(
        "Sistemas Distribuidos 2021/ERE4: TRABALHO PRÁTICO.\n"
        "Autor: Douglas Affonso Clementino. *Data da última alteração 29/10/2021.\n"
        "Este Programa foi executado com N=%d Processos.\n"
        "\n\n",
        N
    );

    smpl(0, "Simuação 'TRABALHO PRÁTICO'.");    // Definindo Identificado para simulação.
    reset();                                    // Iniciando / reinicinado simulação.
    stream(1);                                  // Definindo 1 fluxo para simuação.
    
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

    inicializaEvento(&evento, N);



    for(i = 0; i < N; i++){
        schedule(TEST, 30.0, i);    // Escalonando testes para todos os nodos executarem no tempo 30.
        defineTestes(nodo, i, N);   // Calculando os testes que cada um dos nodos deverá relaizar.
    }

    // Imprime informações sobre inicialização da simulação.
    imprimeInicializacao(nodo, N);

    // Função que agendará eventos que ocorrerão durante simuação.
    agendarEventos();

    printf("====================== Incializando Simulação ======================\n");
    // Loop de simulação.
    while(time() < T_SIMULACAO){
        cause(&event, &token);

        switch(event){

            case TEST:

                // Caso o testador estiver falho, sair.
                if (status(nodo[token].id) != CORRETO)    
                    break; 

                printf("[%3.1f] O nodo [%d] inica testes:\n", time(), token);

                // Nodo "token" testará "qnt_testes" de acordo com indicado em seu vetor de testes "testes".
                for(i = 0; i < nodo[token].qnt_testes; i++){
                    testarNodo(nodo, token, nodo[token].testes[i], N, &evento); 
                    evento.cout_testes ++;  // Contabiliza teste em estrutura evento.
                }

                // Caso 'evento' esteja sendo diagnosticado.
                if(evento.diagnosticando){
                    // Atualiza entrada de nodo 'token' em vetor 'rodada_completa', indicando que em rodada atual, vetor token já efetuou todos os testes.
                    evento.rodada_completa[token] = 1;
                    verificaRodadaCompleta(&evento, nodo, N);
                    // imprimeEventoTeste(&evento, N);     //DEBUG:

                }
                
                imprimeVectorState(nodo, token, N);
                
                schedule(TEST, 30.0, token);
                break;

            case FAULT:
                r = request(nodo[token].id, token, 0);
                if(r != 0){
                    puts("Não foi possível falhar o nodo...");
                    break;
                }
                printf("[%3.1f] O nodo [%d] falhou.\n", time(), token);
    
                inicializaNovoEvento(nodo, &evento, N, token ,FAULT);
                break;

            case REPAIR:
                release(nodo[token].id, token);
                printf("[%3.1f] O nodo [%d] recuperou.\n", time(), token);
                schedule(TEST, 30.0, token);
                break;
        }
    }
    
    printf("====================== Finalizando Simulação ======================\n");
    // Liberando memória alocada.
    for(i = 0; i < N; i++){
        free(nodo[i].state);
        free(nodo[i].testes);
    }
    liberaEvento(&evento);
    free(nodo);
    exit(0);
}