//
// Created by igor on 24/09/19.
//

#include "Solucao.h"

Solucao::Solucao::Solucao(int numVeiculos)
{
    poluicao = 0.0;
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

void Solucao::Veiculo::inicialisaVeiculo()
{

    ClienteRota *clienteRota = new ClienteRota;
    clienteRota->cliente = 0;
    clienteRota->tempoSaida = 0;

    listaClientes.push_back(clienteRota);

    clienteRota = new ClienteRota;
    clienteRota->cliente = 0;
    clienteRota->tempoSaida = -1;

    listaClientes.push_back(clienteRota);
}