#ifndef __EVENTO__
    #define __EVENTO__

    #include "nodo.h"
    #include "smpl.h"
    
    typedef struct{
        int nodo;               // ID de Nodo que sofreu o evento.
        char diagnosticando;    // Flag que indica que evento atual esta em processo de diagnóstico.
        int tipo;               // Tipo de Evento que esta sendo gerenciado (Falha / Recuperação)
        int cout_rodada;        // Contador de rodadas até o diagnóstico.
        int cout_testes;        // Contador de testes até o diagnóstico.
        real t_inicio;          // Tempo em que eveto foi sofrido.
        real t_fim;             // Tempo em que evendo foi diagnosticado.
        char *alertados;         // Vetor que lista todos os nodos que estão cientes de evento (utilizado para verificar se evento foi diagnosticado).
        char *rodada_completa;   // Vetor que lista nodos que completaram sua bateria de testes, uma vez que todos os nodos corretos completam sua bateria de testes, uma nova rodada é conabilizada (count_rodada ++), reinicializando os valores deste vetor.
    } tevento;


    // imprime informações de evento em log.
    void imprimeDiagnostico(tevento *evento);

    // Verifica se evento 'evento' foi completamente diagnosticado, retornando '1' para verdadeiro e '0' para falso.
    int checaEventoDiagnosticado(tevento *evento, int qnt_nodos);

    // Verifica se 'n_testado' é o nodo relacionado a 'evento', caso positivo, indicando em 'evento' que nodo 'n_testado' esta ciente do evento e verifica se o diagnostico foi completo.
    void identificaEvento(int n_testador, int n_testado, tevento *evento, int qnt_nodos);

    // Imprime estrutura evento e seus componentes.
    void imprimeEventoTeste(tevento *evento, int qnt_nodos);

    // Inicializa valores para novo evento do tipo 'tipo' sofrido pelo nodo 'id'.
    void inicializaNovoEvento(tnodo *nodo, tevento *evento, int qnt_nodos, int id, int tipo);

    // Libera alocações de estrutura Evento.
    void liberaEvento(tevento *evento);

#endif