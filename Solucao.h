//
// Created by igor on 24/09/19.
//

#ifndef HGVRSP_SOLUCAO_H
#define HGVRSP_SOLUCAO_H

#include <list>

namespace Solucao
{

    typedef struct
    {
        int cliente;
        //float distanciaPeriodo[5];  //Distancia em que o veiculo percorreu em cada periodo para chegar no cliente
        int temoChegada;
        int tempoSaida;

    } ClienteRota;

    typedef struct
    {

        std::list<ClienteRota *> *listaClientes;


    }Veiculo;

    class Solucao
    {

        bool **vetorClientes; //Cada rota tem um vetor para quardar se atende ou não o cliente i
        std::list<ClienteRota *> veiculoFicticio;
        float *combustivel;
        float *carga;
    };

}

#endif //HGVRSP_SOLUCAO_H
