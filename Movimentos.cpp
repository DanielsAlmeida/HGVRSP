//
// Created by igor on 10/02/2020.
//

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"

using namespace Movimentos;

bool Movimentos::mvIntraRotasReinsertion(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                        Solucao::ClienteRota *vetClienteRotaAux,bool pertubacao)
{


    //Escolhe um veiculo

    int veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
        veiculoEscolhido = 0;

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    while(true)
    {
        if(veiculo->listaClientes.size() > 2)
            break;
        else
        {
            veiculoEscolhido += 1;
            veiculoEscolhido %= solucao->vetorVeiculos.size();

            if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
                veiculoEscolhido = 0;

            veiculo = solucao->vetorVeiculos[veiculoEscolhido];
        }

    }


    int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 1);
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);


    bool inicio = false;
    int peso = veiculo->carga;
    int posicaoVetorLivre = -1;


    for(auto it = veiculo->listaClientes.begin(); it != veiculo->listaClientes.end(); ++it)
    {
        if((*it)->cliente == (*clienteEscolhido)->cliente)
            continue;

        if(posicaoVetorLivre == -1)
        {


            if(Construtivo::determinaHorario(&vetClienteRotaAux[0], &vetClienteRotaAux[1], instancia, peso, veiculo->tipo))
            {
                auto itAux = it;
                itAux++;

                int posicao = 2;

                peso -= instancia->vetorClientes[(**clienteEscolhido).cliente].demanda;
                bool viavel = true;
                double poluicao = 0.0;
                double combustivel = instancia->vetorVeiculos[veiculo->tipo].combustivel;

                while(itAux != veiculo->listaClientes.end())
                {
                    vetClienteRotaAux[posicao] = **itAux;

                    if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicao-1], &vetClienteRotaAux[posicao], instancia,peso, veiculo->tipo))
                    {
                        viavel = false;
                        break;
                    }

                    peso -= instancia->vetorClientes[(**itAux).cliente].demanda;
                    combustivel -= vetClienteRotaAux[posicao].combustivel;
                    poluicao += vetClienteRotaAux[posicao].poluicao;
                    itAux++;
                    posicao += 1;

                    if(combustivel <= 0.0)
                    {
                        viavel = false;
                        break;
                    }
                }

                if(!viavel)
                {
                    continue;
                }
                else
                {

                }

            }

        }


    }



}

/* ******************************************************************************************************************************************************************************************************
 *
 * Recalcula a rota até posicaoAlvo, exclui clienteEscolhido
 *
 ******************************************************************************************************************************************************************************************************** */

ResultadosRotaParcial Movimentos::recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                                Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin)
{

    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto clienteAlvo = std::next(veiculo->listaClientes.begin(), posicaoAlvo);

    double poluicao , combustivel;
    poluicao = combustivel = 0.0;


    // Não existe rotas em vetClienteRota. Copiar lista para vetClienteRota, até posicaoAlvo. Se clienteEscolhido for achado, deve-se calcular a rota sem ele.

    for(auto it = std::next(veiculo->listaClientes.begin(), begin); it != veiculo->listaClientes.end(); )
    {
        if((*it)->cliente != (*clienteEscolhido)->cliente && (*it)->cliente != (*clienteAlvo)->cliente)
        {
            //Copia it para o vetor e atualiza as variaveis.
            vetClienteRotaAux[posicaoVet] = **it;
            peso -= instancia->vetorClientes[(**it).cliente].demanda;
            poluicao += (**it).poluicao;
            combustivel += (**it).combustivel;
        }
        else if((*it)->cliente == (*clienteAlvo)->cliente)
        {
            //Somente insere it no vetor.
            vetClienteRotaAux[posicaoVet] = **it;
            peso -= instancia->vetorClientes[(**it).cliente].demanda;
            poluicao += (**it).poluicao;
            combustivel += (**it).combustivel;

            break;
        }
        else
        {
            //it == clienteEscolhido

            auto itAntes = it;
            itAntes--;
            it++;
            peso -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;

            vetClienteRotaAux[posicaoVet+1] = **it;

            //Rota entre o cliente anterior de it e o cliente posterior de it
            if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet+1],instancia, peso, veiculo->tipo))
            {
                ResultadosRotaParcial resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                return resultado;
            }

            //Atualiza valores
            poluicao += vetClienteRotaAux[posicaoVet+1].poluicao;
            combustivel+= vetClienteRotaAux[posicaoVet+1].combustivel;
            peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet+1].cliente].demanda;

            //Calcula rota até clienteAlvo
            posicaoVet += 1;

            while(true)
            {
                it++;

                if(it == (*veiculo).listaClientes.end())
                    throw exceptionEndList;

                vetClienteRotaAux[posicaoVet+1] = **it;
                if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet+1],instancia, peso, veiculo->tipo))
                {
                    ResultadosRotaParcial resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                //Atualiza valores
                poluicao += vetClienteRotaAux[posicaoVet+1].poluicao;
                combustivel+= vetClienteRotaAux[posicaoVet+1].combustivel;
                peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet].cliente].demanda;
                posicaoVet += 1;

                if(combustivel > instancia->vetorVeiculos[veiculo->tipo].combustivel)
                {
                    ResultadosRotaParcial resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                if((*it)->cliente == (*clienteAlvo)->cliente)
                {
                    posicaoVet += 1;
                    break;
                }


            }

        }

        posicaoVet += 1;
        ++it;
    }

    ResultadosRotaParcial resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicaoVet};
    return resultado;

}
