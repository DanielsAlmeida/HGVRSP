//
// Created by igor on 10/02/2020.
//

#ifndef HGVRSP_MOVIMENTOS_H
#define HGVRSP_MOVIMENTOS_H

#include "Instancia.h"
#include "Solucao.h"
#include <string>
//#include "Exception.h"
typedef std::list<Solucao::ClienteRota*>::iterator ItClienteRota;

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
        bool trocaTipos;

    };

    ResultadosRota mvIntraRotaShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                    Solucao::ClienteRota *vetClienteRotaBest,
                                    Solucao::ClienteRota *vetClienteRotaAux, const bool percorreVeiculos,
                                    const bool percorreClientes, const bool pertubacao, double *vetLimiteTempo);

    ResultadosRota mvIntraRotaSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                   Solucao::ClienteRota *vetClienteRotaBest,
                                   Solucao::ClienteRota *vetClienteRotaAux, const bool percorreVeiculos,
                                   const bool percorreClientes, bool pertubacao, double *vetLimiteTempo);

    ResultadosRota mvInterRotasShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                     Solucao::ClienteRota *vetClienteRotaBest,
                                     Solucao::ClienteRota *vetClienteRotaAux,
                                     Solucao::ClienteRota *vetClienteRotaSecundBest,
                                     const bool percorreVeiculos, bool pertubacao, double *vetLimiteTempo);

    ResultadosRota mvInterRotasSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                    Solucao::ClienteRota *vetClienteRotaBest,
                                    Solucao::ClienteRota *vetClienteRotaAux,
                                    Solucao::ClienteRota *vetClienteRotaSecundBest,
                                    Solucao::ClienteRota *vetClienteRotaSecundAux, const bool percorreVeiculos,
                                    bool pertubacao, double *vetLimiteTempo);

    ResultadosRota mv_2optSwapIntraRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                        Solucao::ClienteRota *vetClienteRotaBest,
                                        Solucao::ClienteRota *vetClienteRotaAux, const bool percorreVeiculos,
                                        bool pertubacao, double *vetLimiteTempo);

    ResultadosRota mv_2optSwapInterRotas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                         Solucao::ClienteRota *vetClienteRotaBest,
                                         Solucao::ClienteRota *vetClienteRotaAux,
                                         Solucao::ClienteRota *vetClienteRotaSecundBest,
                                         Solucao::ClienteRota *vetClienteRotaSecundAux,
                                         const bool percorreVeiculos, bool pertubacao, double *vetLimiteTempo);

    Movimentos::ResultadosRota
    mvIntraRotaInverteRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                           Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                           bool pertubacao, double *vetLimiteTempo);

    Movimentos::ResultadosRota mvTrocarVeiculos(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                                Solucao::ClienteRota *vetClienteRotaBest,
                                                Solucao::ClienteRota *vetClienteRotaAux,
                                                Solucao::ClienteRota *vetClienteRotaSecundBest,
                                                Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao,
                                                double *vetLimiteTempo);

    //Peso de ResultadosRota é o peso restante do veiculo
    ResultadosRota inverteRota(ItClienteRota itInicio, ItClienteRota itFim, Solucao::ClienteRota *vetClienteRotaAux, int posicao, int peso, double poluicao,
                               double combustivel, const Instancia::Instancia *const instancia, int tipoVeiculo);

    // Recalcula a rota até posicaoAlvo, excluindo, caso exista,  clienteEscolhido.
    ResultadosRota recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                 Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin);


    //Calcula rota ate o final.
    ResultadosRota calculaFimRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, ItClienteRota proximoClienteIt, int peso, Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, double poluicao, double combustivel,
                                  const int clienteEscolhido, std::string mvStr, int *erro, const int substituto = -1, const int pesoTotal = -1, const int maisclientes = 0);

    ResultadosRota calculaFimRota_2OptInter(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, Solucao::Veiculo *veiculo2, ItClienteRota proximoClienteIt, int peso,
                                            Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, double poluicao, double combustivel, const int clienteEscolhido, std::string mvStr,
                                            const int substituto = -1, const int pesoTotal = -1, const int maisclientes = 0);

    bool recalculaCombustivelPoluicaoCargas(Solucao::Veiculo *veiculo, double *poluicao, double *combustivel, int *pesoTotal, const Instancia::Instancia *const instancia,
                                            Solucao::ClienteRota *vetClienteRotaAux, int posicao);


    void copiaSolucao(Solucao::ClienteRota *bestPtr, Solucao::ClienteRota *auxPtr, double *poluicaoBest, double *combustivelBest, ResultadosRota resultado, int *posicao);

    Movimentos::ResultadosRota
    aplicaMovimento(int movimento, const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                    Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                    bool pertubacao, Solucao::ClienteRota *vetClienteRotaSecundBest,
                    Solucao::ClienteRota *vetClienteRotaSecundAux, double *vetLimiteTempo);

    void atualizaSolucao(ResultadosRota resultado, Solucao::Solucao *solucao,
                         Solucao::ClienteRota *vetClienteRotaBest,
                         Solucao::ClienteRota *vetClienteRotaSecundBest,
                         const Instancia::Instancia *const instancia, const int movimento);





}

#endif //HGVRSP_MOVIMENTOS_H
