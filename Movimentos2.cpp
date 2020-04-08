//
// Created by igor on 07/04/2020.
//

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"

using namespace Movimentos;

ResultadosRota Movimentos::inverteRota(auto itInicio, auto itFim, Solucao::ClienteRota *vetClienteRotaAux, int posicao, int peso, double poluicao,
                           double combustivel, const Instancia::Instancia *const instancia, int tipoVeiculo)
{
    //Percorre o veiculo na ordem inversa
    for(;;itFim--)
    {

        vetClienteRotaAux[posicao+1].cliente = (*itFim)->cliente;

        //Calcula horário, poluicao e combustivel
        if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, tipoVeiculo))
            return {.viavel = false};

        //Atualia poluicao e combustivel
        poluicao += vetClienteRotaAux[posicao+1].poluicao;
        combustivel += vetClienteRotaAux[posicao+1].combustivel;

        //Verifica capacidade
        if(!VerificaSolucao::verificaCombustivel(combustivel, tipoVeiculo, instancia))
            return {.viavel = false};

        //Atualia peso
        peso -= instancia->vetorClientes[(*itFim)->cliente].demanda;

        posicao++;
        if(itFim == itInicio)
            break;
    }

    return {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicao};

}

/*
 * Escolher uma rota e uma aresta. Percorrer as outras aresta e invertendo os clientes entre as duas arestas. As arestas não podem ser do mesmo cliente.
 *
 */
ResultadosRota Movimentos::mv_2optSwapIntraRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,Solucao::ClienteRota *vetClienteRotaBest,
                                    Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{

    int veiculoEscolhido;      //guarda o veiculo


    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);


    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    //Seleciona outro veiculo, enquanto o veiculo for vazio
    while(veiculo->listaClientes.size() <= 2)
    {
        veiculoEscolhido++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

    }

    //Escolhe um cliente
    const int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;
    bool inicioRota = true;

    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0, combustivelAux, poluicaoAux;
    double combustivelBest, poluicaoBest = HUGE_VALF;
    int posicaoBestVeic, posicaoAux, pesoBest;

    int pesoVeic = veiculo->carga;
    int pesoAux;

    vetClienteRotaAux[0] = **veiculo->listaClientes.begin();
    int posicao = 0;
    int indexListaClientes = 1;
    bool clientePassouEscolhido = false;

    for(auto clienteIt = std::next(veiculo->listaClientes.begin(),1); clienteIt != veiculo->listaClientes.end(); )
    {
        //Cliente escolhido é a clienteIt.  duas arestas do mesmo cliente
        if(indexListaClientes == posicaoClienteEscolhido)
        {
            indexListaClientes += 2;
            clienteIt++;
            clienteIt++;

            clientePassouEscolhido = true;
            continue;
        }

        //Seleciona as possicões para inverter
        auto fim = clienteEscolhido;
        auto inicio = clienteIt;

        if(clientePassouEscolhido)
        {
            fim = clienteIt;
            inicio = clienteEscolhido;
        }

        //Inverte rota inicio fim.
        ResultadosRota resultadosRota = inverteRota(inicio, fim, vetClienteRotaAux, posicao, pesoVeic, poluicaoRotaParcial,
                                                    combustivelRotaParcial, instancia, veiculo->tipo);

        if(resultadosRota.viavel)
        {
            //Atualiza variaveis temporárias
            pesoAux = resultadosRota.peso;
            combustivelAux = resultadosRota.combustivel;
            poluicaoAux = resultadosRota.poluicao;
            posicaoAux = resultadosRota.posicaoVet;
            fim++;

            //Calcula o fim da rota
            resultadosRota = calculaFimRota(instancia, veiculo, fim, pesoAux, vetClienteRotaAux, posicaoAux, poluicaoAux, combustivelAux, -1, -1, veiculo->carga, 0);

            if(resultadosRota.viavel)
            {
                if(resultadosRota.poluicao < poluicaoBest)
                {
                    pesoBest = pesoAux;

                    copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota, &posicaoBestVeic);

                    if(pertubacao)
                        break;
                }
            }

        }

        if(!clientePassouEscolhido)
        {
            //Copia clienteIt para o vetor

            pesoVeic -= instancia->vetorClientes[(*clienteIt)->cliente].demanda;
            combustivelRotaParcial += (*clienteIt)->combustivel;
            poluicaoRotaParcial += (*clienteIt)->poluicao;

            vetClienteRotaAux[posicao+1] = **clienteIt;
            posicao++;

        }
        //Se clienteIt passou escolhido, ele NÃO é copiado para o vetor. Porque a cada interação de vet[posicao] com (vet[posicao+1], igual a clienteIt), aresta irá mudar!!

        clienteIt++;
        indexListaClientes++;

    }

    if(poluicaoBest == HUGE_VALF)
        return {.viavel = false};

    ResultadosRota resultadosRota;

    resultadosRota.viavel = true;
    resultadosRota.poluicao = poluicaoBest;
    resultadosRota.posicaoVet = posicaoBestVeic;
    resultadosRota.combustivel = combustivelBest;
    resultadosRota.peso = veiculo->carga;
    resultadosRota.veiculo = veiculo;
    resultadosRota.veiculoSecundario = NULL;

    return resultadosRota;

}

