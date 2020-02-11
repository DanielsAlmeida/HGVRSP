//
// Created by igor on 10/02/2020.
//

#ifndef HGVRSP_MOVIMENTOS_H
#define HGVRSP_MOVIMENTOS_H

#include "Instancia.h"
#include "Solucao.h"


namespace Movimentos
{


    class ExceptionDetRota: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Erro, fuc: recalculaRota. \nMotivo: nao foi possivel determinar rota entre dois clientes. OBS: DEVERIA ser possivel determinar rota.\n";
        }
    } exceptionDetRota;

    class ExceptionEndList: public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Erro, fuc: recalculaRota. \nMotivo:Fim da lista. OBS: não DEVERIA alcancar o fim da lista.\n";
        }
    } exceptionEndList;

    typedef struct
    {
        double poluicao, combustivel;
        int peso;
        bool viavel;
        int posicaoVet;

    }ResultadosRotaParcial;

    bool mvIntraRotasReinsertion(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao = 0);
    ResultadosRotaParcial recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                        int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                        Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin);
}

#endif //HGVRSP_MOVIMENTOS_H
