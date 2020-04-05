//
// Created by igor on 10/02/2020.
//

#ifndef HGVRSP_MOVIMENTOS_H
#define HGVRSP_MOVIMENTOS_H

#include "Instancia.h"
#include "Solucao.h"


namespace Movimentos
{


    struct ResultadosRota
    {
        double poluicao, combustivel;
        int peso;
        bool viavel;
        int posicaoVet;
        Solucao::Veiculo *veiculo;
        Solucao::Veiculo *veiculoSecundario;
        double poluicaoSecundario, combustivelSecundario;
        int pesoSecundario, posicaoVetSecundario;

    };

    ResultadosRota mvIntraRotaShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                    Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao= false);

    ResultadosRota mvIntraRotaSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,Solucao::ClienteRota *vetClienteRotaBest,
                                    Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao= false);

    ResultadosRota mvInterRotasShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                     Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest, bool pertubacao = false);

    ResultadosRota mvInterRotasSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                     Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest, bool pertubacao = false);


    // Recalcula a rota até posicaoAlvo, excluindo, caso exista,  clienteEscolhido.
    ResultadosRota recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                 Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin);


    //Calcula rota ate o final.
    ResultadosRota calculaFimRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                  auto proximoClienteIt, int peso, Solucao::ClienteRota *vetClienteRotaAux,
                                  int posicaoVet, double poluicao, double combustivel,
                                  const int clienteEscolhido, const int substituto = -1, const int pesoTotal=-1, const int maisclientes = 0);

    void copiaSolucao(Solucao::ClienteRota *bestPtr, Solucao::ClienteRota *auxPtr, double *poluicaoBest,
                      double *combustivelBest, ResultadosRota resultado);


}

#endif //HGVRSP_MOVIMENTOS_H
