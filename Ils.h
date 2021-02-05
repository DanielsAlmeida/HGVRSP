//
// Created by igor on 20/10/2020.
//
#include "Solucao.h"
#include "Vnd.h"
#include "HashRotas.h"
#include "Instancia.h"
#include "Modelo.h"
#include "Modelo_1_rota.h"
#include "Construtivo.h"
#include "EstatisticasQualidade.h"

#ifndef HGVRSP_ILS_H
#define HGVRSP_ILS_H


namespace Ils
{

    struct Parametros
    {
        int interacaoInicioMip;
        int intervaloEsperaMip;
        int interacoesSemMelhora;
        int intervaloResetarSolucao;
        int numRotasMip;
        int interacoesIls;
        int numSolucoesMip;
        int interacoesGrasp;
    };

    void ils(const Instancia::Instancia *const instancia, Solucao::Solucao **solucao, const u_int64_t numInteracoesMax,
             const u_int64_t numInteracoesMaxSemMelhora, const double tempoLimite, const int opcao,
             Solucao::ClienteRota **vetVetorClienteRota, HashRotas::HashRotas *hashRotas, int **vetGuardaRota,
             Vnd::EstatisticaMv *vetEstatistica, double *vetLimiteTempo, int **matRotas,
             Modelo_1_rota::Modelo *modelo1Rota,
             Modelo::Modelo *modelo, double *tempoModelo2Rotas, u_int64_t *interacoesIls,
             u_int64_t *ultimaAtualizacaoIls,
             Construtivo::Candidato *vetorCandidatos, double *vetorParametors, Solucao::ClienteRota **matrixClienteBest,
             Movimentos_Paradas::TempoCriaRota *tempoCriaRota, Construtivo::GuardaCandInteracoes *vetCandInteracoes,
             const double alvo, Alvo::Alvo *alvoTempo, list<EstatisticasQualidade> &listaEstQual, int k_pertubacao,
             Parametros parametros);

    double calculaSolucaoHeuristica(const Instancia::Instancia *instancia, Solucao::Solucao *solucao, HashRotas::HashRotas *hashRotas,
                                    Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorAux, double *vetorLimiteTempo);



}

#endif //HGVRSP_ILS_H
