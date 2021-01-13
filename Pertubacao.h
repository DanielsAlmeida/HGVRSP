//
// Created by igor on 08/01/2021.
//

#include "Instancia.h"
#include "Solucao.h"

#ifndef HGVRSP_PERTUBACAO_H
#define HGVRSP_PERTUBACAO_H

namespace Pertubacao
{



    bool pertubacao_k_swap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, const int k,
                           Solucao::ClienteRota *vetClienteRota, Solucao::ClienteRota *vetClienteRotaAux,
                           double *vetLimiteTempo, PertubacaoInviabilidade *inviabilidadeEstatisticas);




}



#endif //HGVRSP_PERTUBACAO_H
