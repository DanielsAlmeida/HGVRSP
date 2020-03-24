//
// Created by igor on 10/02/2020.
//

#ifndef HGVRSP_MOVIMENTOS_H
#define HGVRSP_MOVIMENTOS_H

#include "Instancia.h"
#include "Solucao.h"


namespace Movimentos
{


    class ExceptionPeso: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Erro, fuc: calculaFimRota. \nMotivo: Peso passado incorreto\n";
        }
    } exceptionPeso;

    class ExceptionEndList: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Erro, fuc: recalculaRota. \nMotivo:Fim da lista, Cliente alvo NAO está na lista.\n";
        }
    } exceptionEndList;

    class ExceptionPesoNegativo: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Erro, fuc: calculaFimRota. \nPeso negativo.\n";
        }
    } exceptionPesoNegativo;

    typedef struct
    {
        double poluicao, combustivel;
        int peso;
        bool viavel;
        int posicaoVet;

    }ResultadosRota;

    bool mvIntraRotasReinsertion(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao = 0);

/// Recalcula a rota até posicaoAlvo, excluindo, caso exista,  clienteEscolhido.
/// @param instancia @param veiculo @param posicaoClienteEscolhido @param posicaoAlvo @param peso @param vetClienteRotaAux @param posicaoVet --posicao livre @param begin
    ResultadosRota recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                 Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin);

    /* ******************************************************************************************************************************************************************/
      ///Calcula rota ate o final.
      ///@param instancia @param veiculo @param posicaoProximoCliente -- Posicao do proximo cliente na lista de clientes @param peso
      /// @param vetClienteRotaAux @param posicaoVet -- proxima posicao livre
    /* *******************************************************************************************************************************************************************/
    ResultadosRota calculaFimRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                  int posicaoProximoCliente, int peso, Solucao::ClienteRota *vetClienteRotaAux,
                                  int posicaoVet, double poluicao, double combustivel);

}

#endif //HGVRSP_MOVIMENTOS_H
