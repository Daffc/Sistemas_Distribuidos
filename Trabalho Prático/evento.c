/*
    Biblioteca: 
        Arquivo utilizado pelo programa 'trabPrat.c', nele estão discritas funções que auxiliam no 
        processo de identificação de um nodo evento assim como informações sobre o seu diagnóstico.

    Autor: Douglas Affonso Clementino
    Data da última modificação: 29/10/2021
*/

#include "nodo.h"
#include "evento.h"
#include "smpl.h"

// Inicializa estrutura 'tevento' de acordo com a quantidade de nodos 'qnt_nodos'.
void inicializaEvento(tevento *evento, int qnt_nodos){
    evento->diagnosticando = 0;                              // Indica que evento atual já foi diagnosticado (uma vez que não existe evento).
    evento->alertados = (char *) malloc(qnt_nodos);          // Alocando espaço para vetor de nodos alertados sobre evento.
    evento->rodada_completa = (char *) malloc(qnt_nodos);    // Alocando espaço para vetor de nodos com rodada completa.
}

// imprime informações de evento em log.
void imprimeDiagnostico(tevento *evento){
    printf("\t\tO evento em nodo [%d] foi diagnosticado:\n", evento->nodo);
    if(evento->tipo == 0)
        printf("\t\t\tTipo: REPARAÇÂO\n");
    if(evento->tipo == 1)    
        printf("\t\t\tTipo: FALHA\n");
    printf("\t\t\tRodadas: %d\n", evento->cout_rodada);
    printf("\t\t\tTestes: %d\n", evento->cout_testes);
    printf("\t\t\tTempo Inicio: %3.1f\n", evento->t_inicio);
    printf("\t\t\tTempo Fim: %3.1f\n", time());
}

// Verifica se evento 'evento' foi completamente diagnosticado, retornando '1' para verdadeiro e '0' para falso.
int checaEventoDiagnosticado(tevento *evento, int qnt_nodos){
    int i;
    int cont;

    cont = 0;
    // Conta quantiade de entradas em de 'aletados' com valor '1'.
    for(i = 0; i < qnt_nodos; i++)
        cont += evento->alertados[i];

    // Caso todas as entradas de 'alertados' tenham valor '1', evento foi diagnosticado.
    if (cont == qnt_nodos)
        return 1;

    return 0;
}
// Verifica se 'n_testado' é o nodo relacionado a 'evento', caso positivo, indicando em 'evento' que nodo 'n_testado' esta ciente do evento e verifica se o diagnostico foi completo.
void identificaEvento(int n_testador, int n_testado, int tipo_evento, tevento *evento, int qnt_nodos){
    // Verifica se evento esta em fase de diagnostico (flag diagnosticando).
    if(evento->diagnosticando){
        // Verifica se nodo testado (evento percebido) é o nodo registrado pela estrutura 'evento'.
        if(evento->nodo == n_testado){
            // Verifica se evento percebido por 'n_testador' em nodo 'n_testado' é o mesmo tipo de evento gerenciado por 'evento'.
            if(evento->tipo == tipo_evento){
                // informar a estrutura 'evento' que nodo 'n_testador' está a par de sua situação.
                evento->alertados[n_testador] = 1;

                // Verifica se evento foi diagnosticado e, caso verdadero, imprime informações de evento e desabilita flag 'diagnosticando'.
                if(checaEventoDiagnosticado(evento, qnt_nodos)){
                    imprimeDiagnostico(evento);
                    evento->diagnosticando = 0;
                }

            }
        }
        
    }

}

// Inicializa valores para novo evento do tipo 'tipo' sofrido pelo nodo 'id'.
void inicializaNovoEvento(tnodo *nodo, tevento *evento, int qnt_nodos, int id, int tipo){
    int i;

    evento->nodo = id;                          // Armazena nodo que sofreu o evento.
    evento->tipo = tipo;                        // Armazena tipo de evendo sofrido por nodo 'id'.
    evento->diagnosticando = 1;                 // Indica que evento atual está em processo de diagnóstico.
    evento->cout_rodada = 1;                    // Reicinializa contador de roadadas (No mínimo 1 rodada necessária para o descobrimento de um evento).
    evento->cout_testes = 0;                    // Reicinializa contador de testes.
    evento->t_inicio = time();                  // Armazena tempo de iníncio do evento.

    for(i = 0; i < qnt_nodos; i++){
        if(status(nodo[i].id) != CORRETO){
            evento->alertados[i] = 1;           // Nodos FALHOS constarão como alertados.
            evento->rodada_completa[i] = 1;     // Nodos FALHOS constarão como rodada completa.
        }
        else{
            evento->alertados[i] = 0;           // Nodos CORRETOS constarão como NÃO alertados.
            evento->rodada_completa[i] = 0;     // Nodos FALHOS constarão como rodada NÃO completa.  
        }
    }

    evento->alertados[id] = 1;                  // Indica que o 2nodo que sofreu o evento está atualizado sobre a sua situação.
}

void verificaRodadaCompleta(tevento *evento, tnodo *nodo, int qnt_nodos){
    int i;
    int cont;

    cont = 0;
    // Conta quantidade de entradas de 'rodada_completa' com valor '1'.
    for(i = 0; i < qnt_nodos; i++)
        cont += evento->rodada_completa[i];

    // Caso todas as entradas de 'rodada_completa' tenham valor '1', todos os nodos corretos completaram seus testes, ou seja, uma rodada foi completa.
    if (cont == qnt_nodos){
        // Contabiliza rodada em intervalo de diagnóstico.
        evento->cout_rodada ++;

        // Reinicializa vetor "rodada_completa" para contabilização de nova rodada.
        for(i = 0; i < qnt_nodos; i++){
            if(status(nodo[i].id) != CORRETO){
                evento->rodada_completa[i] = 1;     // Nodos FALHOS constarão como rodada completa.
            }
            else{
                evento->rodada_completa[i] = 0;     // Nodos FALHOS constarão como rodada NÃO completa.  
            }
        }
    }
}

// Libera alocações de estrutura Evento.
void liberaEvento(tevento *evento){
    free(evento->alertados);
    free(evento->rodada_completa);
}