//
// Created by igor on 11/04/2020.
//

#ifndef HGVRSP_VIABILIZASOLUCAO_H
#define HGVRSP_VIABILIZASOLUCAO_H

#include "Construtivo.h"
#include "Solucao.h"
#include "Instancia.h"
#include "VerificaSolucao.h"

namespace ViabilizaSolucao
{

    bool geraSolucao(Solucao::Solucao *solucao, const Instancia::Instancia *const instancia, float alfa,
                     Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux,
                     string *sequencia, bool log, Construtivo::Candidato *vetorCandidatos,
                     boost::tuple<int, int> heuristica, const double *const vetorParametros,
                     double *vetLimiteTempo, Solucao::ClienteRota **matrixClienteBest,
                     Construtivo::GuardaCandInteracoes *vetCandInteracoes);

    bool viabilizaSolucao(Solucao::Solucao *solucao, const Instancia::Instancia *const instancia, float alfa,
                          Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux,
                          string *sequencia, bool log, Construtivo::Candidato *vetorCandidatos,
                          const int interacoes, const int interacoesPorMv,
                          Solucao::ClienteRota *vetClienteRotaSecundBest,
                          Solucao::ClienteRota *vetClienteSecondAux, boost::tuple<int, int> heuristica,
                          const double *const vetorParametros, double *vetLimiteTempo,
                          Construtivo::GuardaCandInteracoes *vetCandInteracoes, Solucao::ClienteRota **matrixClienteBest);

};

#endif //HGVRSP_VIABILIZASOLUCAO_H
