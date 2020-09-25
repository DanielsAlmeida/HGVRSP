//
// Created by igor on 25/09/2020.
//

#include "Instancia.h"
#include "Modelo.h"

#ifndef HGVRSP_MODELO2ROTAS_H
#define HGVRSP_MODELO2ROTAS_H

namespace Modelo2Rotas
{

    void geraRotas_comb_2Rotas(Solucao::Solucao *solucao, Modelo::Modelo *modelo, Solucao::ClienteRota *vetClienteRota, Solucao::ClienteRota *vetClienteRota2,
                               const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas, int *vetRotasAux, bool **matRotas);

}


#endif //HGVRSP_MODELO2ROTAS_H
