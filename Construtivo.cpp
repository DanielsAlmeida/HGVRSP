//
// Created by igor on 21/10/19.
//

#include "Construtivo.h"
#include <cmath>

using namespace Construtivo;
using namespace std;

Solucao::Solucao* Construtivo::geraSolucao(Instancia::Instancia *instancia, bool (*comparador)(Instancia::Cliente *, Instancia::Cliente * ))
{

    //Inicializa a lista de candidatos(Clientes)
    list<Instancia::Cliente> listaCandidatos(instancia->vetorClientes, instancia->vetorClientes + sizeof(instancia->vetorClientes)/
                                                                                                  sizeof(Instancia::Cliente));
    //Ordena de acordo com o critério
    listaCandidatos.sort(comparador);

    //Cria uma solução com o mínimo de veiculos
    int minVeiculos = instancia->demandaTotal/instancia->vetorVeiculos.capacidade + 1;
    Solucao::Solucao *solucao = new Solucao::Solucao(minVeiculos);


    int melhorVeiculo, veiculoAux;
    std::list<Solucao::ClienteRota *>::iterator melhorPosicao, posicaoAux;
    float menorPoluicao, poluicaoAux;

    auto *candidato = new Solucao::ClienteRota;
    Instancia::Cliente clienteAux;

    //Vetor guarda o resto da lista
    auto *vetorClienteBest = new Solucao::ClienteRota[instancia->numClientes];
    auto *vetorClienteAux = new Solucao::ClienteRota[instancia->numClientes];

    Solucao::ClienteRota *vetorClienteSwap;

    int posVetBest, posVetAux;

    /* Enquanto a lista de candidatos for diferente de vazio, escolha um cliente, calcule o acrescimo de poluição para cada
    * posição possível da solução, desde que a solução seja viável.
    */

    while(!listaCandidatos.empty())
    {
        menorPoluicao = HUGE_VALF;

        //retira um cliente da lista

        clienteAux = listaCandidatos.front();
        listaCandidatos.pop_front();

        candidato->cliente = clienteAux.cliente;


        //Percorre os veículos
        for(auto veiculo = solucao->vetorVeiculos.begin(); veiculo != solucao->vetorVeiculos.end(); ++veiculo)
        {

            //Verifica se a capacidade max será atingida
            if((*veiculo)->carga + clienteAux.demanda > instancia->vetorVeiculos.capacidade)
                continue;

            //Percorrer os clientes.
            for(auto clienteIt = (*veiculo)->listaClientes.begin(); clienteIt != (*veiculo)->listaClientes.end(); ++clienteIt)
            {
                //Candidato tentara ser inserido após clienteIt


            }

        }

        /* Se encontrou uma solução, incluir candidato no veículo de posicao melhorVeiculo,
         *    Caso não encontrou uma solução, criar um novo veículo com o candidato.
         */

    }


}

bool Construtivo::determinaHorario(const Solucao::ClienteRota* const cliente1, Solucao::ClienteRota *cliente2, Instancia::Instancia *instancia)
{


}
