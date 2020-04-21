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

    bool geraSolucao(Solucao::Solucao *solucao, const Instancia::Instancia *const instancia, float alfa, Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux,
                                 string *sequencia, bool log, Construtivo::Candidato *vetorCandidatos, const double parametroHeur1, const double parametroHeur2, const bool heurist1);

    bool viabilizaSolucao(Solucao::Solucao *solucao, const Instancia::Instancia *const instancia, float alfa, Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux,
                          string *sequencia, bool log, Construtivo::Candidato *vetorCandidatos, const double parametroHeur1, const double parametroHeur2, const bool heurist1,
                          const int interacoes, const int interacoesPorMv, Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteSecondAux);

};

#endif //HGVRSP_VIABILIZASOLUCAO_H
