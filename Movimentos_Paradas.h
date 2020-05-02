//
// Created by igor on 27/04/2020.
//

#ifndef HGVRSP_MOVIMENTOS_PARADAS_H
#define HGVRSP_MOVIMENTOS_PARADAS_H

#include <unordered_map>
#include <boost/heap/fibonacci_heap.hpp>
#include "Instancia.h"
#include "Solucao.h"

namespace Movimentos_Paradas
{



    struct Aresta
    {
        double poluicao, combustivel;
        double poluicaoRotas, combustivelRotas;
        bool vetPeriodos[5];
    };

    struct HeapNo
    {
        int id;
        double poluicao;
    };

    struct compare_HeapNo
    {
        bool operator()(const HeapNo &n1, const HeapNo &n2) const
        {
            return n1.poluicao > n2.poluicao; //min heap
        }
    };

    typedef boost::heap::fibonacci_heap<HeapNo, boost::heap::compare<compare_HeapNo>>::handle_type handle_type;

    struct No
    {
        const int id;
        const int cliente;
        const double tempoSaida;
        int predecessorId;
        double predecessorTempoSaida;
        double poluicao;
        double combustivel;
        Aresta *aresta;   //Aresta predecessor
        const bool final; //somente para 0; 0 - ... - i - j - 0
        handle_type handle;
        const double tempoChegada;
        bool fechado;

    };

    struct Cliente
    {
        double tempo;
        int id;
        Aresta aresta;
    };

    struct VetCliente
    {
        Cliente *vetCliente;
        int tam;
        int tamReal;
    };

    bool mvPercorreRotaParadas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRota);
    int buscaBinaria(Cliente *vetCliente, double tempoSaida, int tam);
    int dijkstra(std::unordered_map<int, No*> *hashNo, std::map<int,int> *mapVetor, VetCliente *vetVetCliente, double maxCombustivel);


}


#endif //HGVRSP_MOVIMENTOS_PARADAS_H
