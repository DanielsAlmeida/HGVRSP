//
// Created by igor on 10/02/2020.
//
/*
 * Movimentos com modelo: 0,1,2
 */
#ifndef HGVRSP_MOVIMENTOS_H
#define HGVRSP_MOVIMENTOS_H

#include "Instancia.h"
#include "Solucao.h"
#include <string>
#include "Modelo.h"
#include "HashRotas.h"

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
                                    Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                    const bool percorreVeiculos, const bool percorreClientes, const bool pertubacao,
                                    double *vetLimiteTempo, Modelo::Modelo *modelo, HashRotas::HashRotas *hashRotas,
                                    int *guardaRota);

    ResultadosRota mvIntraRotaSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                   Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                   const bool percorreVeiculos, const bool percorreClientes, bool pertubacao,
                                   double *vetLimiteTempo, Modelo::Modelo *modelo, HashRotas::HashRotas *hashRotas,
                                   int *guardaRota);

    ResultadosRota mvInterRotasShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                     Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                     Solucao::ClienteRota *vetClienteRotaSecundBest, const bool percorreVeiculos,
                                     bool pertubacao, double *vetLimiteTempo, Modelo::Modelo *modelo,
                                     HashRotas::HashRotas *hashRotas, int *guardaRota, int *guardaRota2);

    ResultadosRota mvInterRotasSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                    Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                    Solucao::ClienteRota *vetClienteRotaSecundBest,
                                    Solucao::ClienteRota *vetClienteRotaSecundAux, const bool percorreVeiculos,
                                    bool pertubacao, double *vetLimiteTempo, Modelo::Modelo *modelo,
                                    HashRotas::HashRotas *hashRotas, int *guardaRota, int *guardaRota2);

    ResultadosRota mv_2optSwapIntraRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                        Solucao::ClienteRota *vetClienteRotaBest,
                                        Solucao::ClienteRota *vetClienteRotaAux,
                                        const bool percorreVeiculos, bool pertubacao, double *vetLimiteTempo,
                                        Modelo::Modelo *modelo, HashRotas::HashRotas *hashRotas, int *guardaRota);

    ResultadosRota mv_2optSwapInterRotas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                         Solucao::ClienteRota *vetClienteRotaBest,
                                         Solucao::ClienteRota *vetClienteRotaAux,
                                         Solucao::ClienteRota *vetClienteRotaSecundBest,
                                         Solucao::ClienteRota *vetClienteRotaSecundAux, const bool percorreVeiculos,
                                         bool pertubacao, double *vetLimiteTempo, Modelo::Modelo *modelo,
                                         HashRotas::HashRotas *hashRotas, int *guardaRota, int *guardaRota2);

    Movimentos::ResultadosRota
    mvIntraRotaInverteRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                           Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                           bool pertubacao, double *vetLimiteTempo, Modelo::Modelo *modelo,
                           HashRotas::HashRotas *hashRotas,
                           int *guardaRota);

    Movimentos::ResultadosRota mvTrocarVeiculos(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                                Solucao::ClienteRota *vetClienteRotaBest,
                                                Solucao::ClienteRota *vetClienteRotaAux,
                                                Solucao::ClienteRota *vetClienteRotaSecundBest,
                                                Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao,
                                                double *vetLimiteTempo, Modelo::Modelo *modelo,
                                                HashRotas::HashRotas *hashRotas, int *guardaRota, int *guardaRota2);

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
                    Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao,
                    Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux,
                    double *vetLimiteTempo, Modelo::Modelo *modelo, HashRotas::HashRotas *hashRotas, int *guardaRota,
                    int *guardaRota2);

    void atualizaSolucao(ResultadosRota resultado, Solucao::Solucao *solucao,
                         Solucao::ClienteRota *vetClienteRotaBest,
                         Solucao::ClienteRota *vetClienteRotaSecundBest,
                         const Instancia::Instancia *const instancia, const int movimento);





}

#endif //HGVRSP_MOVIMENTOS_H
