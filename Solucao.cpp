//
// Created by igor on 24/09/19.
//

#include "Solucao.h"

Solucao::Solucao::Solucao(int numVeiculos)
{

    vetorVeiculos.reserve(numVeiculos);

    Veiculo *veiculo;

    for(int i = 0; i < numVeiculos; ++i)
    {

        veiculo = new Veiculo;
        vetorVeiculos.push_back(veiculo);


    }

}

Solucao::Solucao::~Solucao()
{

    Veiculo *veiculoAux;

    for(auto it : vetorVeiculos)
    {

        delete it;

    }

    vetorVeiculos.erase(vetorVeiculos.begin(), vetorVeiculos.end());

}