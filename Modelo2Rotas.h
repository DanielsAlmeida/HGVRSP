//
// Created by igor on 25/09/2020.
//

#include "Instancia.h"
#include "Modelo.h"

#ifndef HGVRSP_MODELO2ROTAS_H
#define HGVRSP_MODELO2ROTAS_H

namespace Modelo2Rotas
{

    /**
     *
     * Otimiza a solucao aos pares de rotas.  O mip pode trocar clientes entre as rotas.
     *
     * @param solucao   Solucao contem as rotas
     * @param modelo    Modelo gurobi
     * @param vetClienteRota  vet armazena rota temporaria
     * @param vetClienteRota2 vet armazena rota temporaria
     * @param instancia
     * @param hashRotas   quarda as melhores rotas
     * @param vetRotasAux
     * @param matRotas    matriz quarda se duas rotas i,j ja foram otimizadas. 0 e 1 se foram ou nao otmizadas e 2 se as rotas nao conseguiram melhorar
     */

    void geraRotas_comb_2Rotas(Solucao::Solucao *solucao, Modelo::Modelo *modelo,
                               Solucao::ClienteRota *vetClienteRota, Solucao::ClienteRota *vetClienteRota2,
                               const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas,
                               int *vetRotasAux, int **matRotas, int *vetRotasAux2, int numMaximoRotas);

}


#endif //HGVRSP_MODELO2ROTAS_H
