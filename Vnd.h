//
// Created by igor on 13/04/2020.
//

#ifndef HGVRSP_VND_H
#define HGVRSP_VND_H


#include "Solucao.h"
#include "Instancia.h"
#include "Movimentos.h"
#include "Modelo.h"
#include "HashRotas.h"

namespace Vnd
{

    struct EstatisticaMv
    {
        u_int64_t num;
        double gap;
        double poluicao;
        u_int64_t numTempo;
        double tempo;

        EstatisticaMv()
        {
            num = 0;
            gap = 0.0;
            poluicao = 0;
            tempo = 0.0;
            numTempo = 0;
        }

    };

    void vnd(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
             Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao,
             Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux,
             int interacao, EstatisticaMv *vetEstatisticaMv, double *vetLimiteTempo, Modelo::Modelo *modelo,
             HashRotas::HashRotas *hashRotas, int *guardaRota, int *guardaRota2, int mvNaoUtilizado);

    bool avaliaSolucao(Solucao::Solucao *solucao, Movimentos::ResultadosRota resultadosRota);

    void atualizaEstatisticaMv(EstatisticaMv *estatisticaMv, Solucao::Solucao *solucao,
                               Movimentos::ResultadosRota resultadosRota);

};


#endif //HGVRSP_VND_H
