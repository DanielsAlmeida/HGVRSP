//
// Created by igor on 13/04/2020.
//

#ifndef HGVRSP_VND_H
#define HGVRSP_VND_H

#include "Construtivo.h"
#include "Solucao.h"
#include "Instancia.h"
#include "Movimentos.h"

namespace Vnd
{

    void vnd(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
             Solucao::ClienteRota *vetClienteRotaBest,
             Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao, Solucao::ClienteRota *vetClienteRotaSecundBest,
             Solucao::ClienteRota *vetClienteRotaSecundAux);

    bool avaliaSolucao(Solucao::Solucao *solucao, Movimentos::ResultadosRota resultadosRota);

}


#endif //HGVRSP_VND_H
