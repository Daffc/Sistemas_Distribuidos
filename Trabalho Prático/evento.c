#include "nodo.h"
#include "evento.h"
#include "smpl.h"

// imprime informações de evento em log.
void imprimeDiagnostico(tevento *evento){
    printf("\t\tO evento em nodo [%d] foi diagnosticado:\n", evento->nodo);
    if(evento->tipo == FAULT)
        printf("\t\t\tTipo: FALHA\n");
    if(evento->tipo == REPAIR)
        printf("\t\t\tTipo: REPARAÇÂO\n");
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
void identificaEvento(int n_testador, int n_testado, tevento *evento, int qnt_nodos){
    if(evento->nodo == n_testado){
        evento->alertados[n_testador] = 1;

        // Verifica se evento foi diagnosticado e, caso verdadero, imprime informações de eevento e desabilita flag 'diagnosticando'.
        if(checaEventoDiagnosticado(evento, qnt_nodos)){
            imprimeDiagnostico(evento);
            evento->diagnosticando = 0;
        }
    }
}

// Imprime estrutura evento e seus componentes.
void imprimeEventoTeste(tevento *evento, int qnt_nodos){
    int i;
    printf("\t******************************\n");
    printf("\t*EVENTO:\n");
    printf("\t*\tnodo: %d\n", evento->nodo);
    printf("\t*\ttipo: %d\n", evento->tipo);
    printf("\t*\tdiagnosticando: %d\n", evento->diagnosticando);
    printf("\t*\tcout_rodada: %d\n", evento->cout_rodada);
    printf("\t*\tcout_testes: %d\n", evento->cout_testes);
    printf("\t*\tt_inicio: %3.1f\n", evento->t_inicio);
    printf("\t*\tt_fim: %3.1f\n", evento->t_fim);
    printf("\t*\talertados: ");
    for(i = 0; i < qnt_nodos; i++){
        printf("%d ", evento->alertados[i]);
    }
    printf("\n");
    printf("\t*\trodada_completa: ");
    for(i = 0; i < qnt_nodos; i++){
        printf("%d ", evento->rodada_completa[i]);
    }
    printf("\n");
    printf("\t******************************\n");
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

    evento->alertados[id] = 1;                  // Indica que o nodo que sofreu o evento está atualizado sobre a sua situação.
}

void verificaRodadaCompleta(tevento *evento, tnodo *nodo, int qnt_nodos){
    int i;
    int cont;

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