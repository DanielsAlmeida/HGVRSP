//
// Created by igor on 24/09/19.
//

#include "Solucao.h"

Solucao::Solucao::Solucao(int numVeiculos)
{
    poluicao = 0.0;
    vetorVeiculos.reserve(numVeiculos+1);
    int tipo = 0;
    veiculoFicticil = false;
    poluicaoPenalidades = 0.0;
    ultimaAtualizacao = 0;
    numSolucoesInv = 0;
    rotasMip = false;

    Veiculo *veiculo;

    for(int i = 0; i < numVeiculos; ++i)
    {

        veiculo = new Veiculo(tipo, i);
        vetorVeiculos.push_back(veiculo);

        if(tipo == 0)
            tipo +=1;
        else
            tipo -= 1;

    }

}

Solucao::Solucao::Solucao(Solucao *outro)
{

    poluicao = outro->poluicao;
    vetorVeiculos.reserve(outro->vetorVeiculos.size());
    veiculoFicticil = outro->veiculoFicticil;

    veiculoFicticil = outro->veiculoFicticil;
    poluicaoPenalidades = outro->poluicaoPenalidades;
    ultimaAtualizacao = outro->ultimaAtualizacao;
    numSolucoesInv = outro->numSolucoesInv;
    rotasMip = outro->rotasMip;


    Veiculo *veiculo;
    Veiculo *veiculoOutro = outro->vetorVeiculos[0];

    for(int i = 0; i < outro->vetorVeiculos.size(); ++i)
    {
        veiculo = new Veiculo(veiculoOutro);
        vetorVeiculos.push_back(veiculo);

        if((i+1)<outro->vetorVeiculos.size())
            veiculoOutro = outro->vetorVeiculos[i+1];
    }

}

Solucao::Solucao::~Solucao()
{

    for(auto it : vetorVeiculos)
    {

        delete it;

    }

    vetorVeiculos.erase(vetorVeiculos.begin(), vetorVeiculos.end());

}

void Solucao::Veiculo::inicialisaVeiculo(int tipo)
{

    ClienteRota *clienteRota = new ClienteRota;
    clienteRota->cliente = 0;

    if(tipo == 0)
        clienteRota->tempoSaida = 0;
    else
        clienteRota->tempoSaida = 0.5;

    clienteRota->poluicao = 0.0;
    clienteRota->distanciaAteCliente = 0.0;

    listaClientes.push_back(clienteRota);

    clienteRota = new ClienteRota;
    clienteRota->cliente = 0;
    clienteRota->tempoSaida = -1;
    clienteRota->poluicao = 0.0;
    clienteRota->distanciaAteCliente = 0.0;

    listaClientes.push_back(clienteRota);
    poluicao = 0.0;
}