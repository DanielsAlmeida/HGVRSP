//
// Created by igor on 27/04/2020.
//

#ifndef HGVRSP_MOVIMENTOS_PARADAS_H
#define HGVRSP_MOVIMENTOS_PARADAS_H

#include <unordered_map>
#include <boost/heap/fibonacci_heap.hpp>
#include "Instancia.h"
#include "Solucao.h"
#include "Pertubacao.h"

namespace Movimentos_Paradas
{

    struct TempoCriaRota
    {
        long double tempoCpu, maior;
        int tamVet;
        u_int64_t num;

        TempoCriaRota()
        {
            tempoCpu = 0.0;
            num = tamVet = 0;
            maior = -HUGE_VAL;
        }
    };

    struct Aresta
    {
        long double poluicao, combustivel;
        long double poluicaoRotas, combustivelRotas;
        bool vetPeriodos[5];
        long double vetDistancia[5];
        long double vetTempo[5];

        Aresta(){}

        Aresta(long double _poluicao, long double _combustivel, long double _poluicaoRotas, long double _combustivelRotas) : poluicao(_poluicao), combustivel(_combustivel),
        poluicaoRotas(_poluicaoRotas), combustivelRotas(_combustivelRotas)
        {

        }

        Aresta(const Aresta &aresta)
        {
            poluicao = aresta.poluicao;
            combustivel = aresta.combustivel;
            poluicaoRotas = aresta.poluicaoRotas;
            combustivelRotas = aresta.combustivelRotas;



            for(int i = 0; i < 5; ++i)
            {

                vetPeriodos[i] = aresta.vetPeriodos[i];
                vetDistancia[i] = aresta.vetDistancia[i];
                vetTempo[i] = aresta.vetTempo[i];

            }
        }
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
        const long double tempoSaida;
        int predecessorId;
        long double predecessorTempoSaida;
        long double poluicao;
        long double combustivel;
        Aresta *aresta;   //Aresta predecessor
        const long double final; //somente para 0; 0 - ... - i - j - 0
        handle_type handle;
        const long double tempoChegada;
        bool fechado;

        No(int _id, int _cliente, long double _tempoSaida, int _predecessorId, long double _predecessorTempoSaida, long double _poluicao, long double _combustivel, Aresta *_aresta, bool _final,
           long double _tempoChegada, long double _fechado) : id(_id), cliente(_cliente), tempoSaida(_tempoSaida), predecessorId(_predecessorId), predecessorTempoSaida(_predecessorTempoSaida),
                poluicao(_poluicao), combustivel(_combustivel), aresta(_aresta), final(_final), tempoChegada(_tempoChegada), fechado(_fechado)
        {}

        ~No()
        {
            if(aresta)
                delete aresta;
        }

    };

    struct Cliente
    {
        long double tempo;
        int id;
        Aresta aresta;

        Cliente(){}

        Cliente(const Cliente &outro)
        {
            tempo = outro.tempo;
            id = outro.id;
            aresta = outro.aresta;
        }

        void swap(const Cliente &outro)
        {
            tempo = outro.tempo;
            id = outro.id;
            aresta = outro.aresta;
        }
    };

    struct VetCliente
    {
        Cliente *vetCliente;
        int tam;
        int tamReal;

        VetCliente(int _tam, int _tamReal):  tam(_tam), tamReal(_tamReal)
        {

            vetCliente = new Cliente[_tamReal];

            if(!vetCliente)
            {
                std::cout<<"Erro ao alocar vetor de vetCliente\n";
                exit(-1);
            }


        }

        VetCliente(){}

        ~VetCliente()
        {
            delete []vetCliente;

        }

        void swap(VetCliente *outroVetCliente)
        {
            int aux = tam;
            tam = outroVetCliente->tam;
            outroVetCliente->tam = aux;

            aux = tamReal;
            tamReal = outroVetCliente->tamReal;
            outroVetCliente->tamReal = aux;

            Cliente *ptr = vetCliente;
            vetCliente = outroVetCliente->vetCliente;
            outroVetCliente->vetCliente = ptr;



        }

    };

    bool mvPercorreRotaParadas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRota);

    int buscaBinaria(Cliente *vetCliente, double tempoSaida, int tam);

    int dijkstra(std::unordered_map<int, No *> *hashNo, std::map<int, int> *mapVetor, VetCliente *vetVetCliente, const double maxCombustivel, std::string *erro);

    /** **********************************************************************************************************
     * Recebe uma rota por um vetor (vetClienteRota) e cria a rota realizando paradas.
     * A nova rota é escrita no vetor (vetClienteRota) e a poluicao e combustivel nos seus respectivos ponteiros.
     *
     * @param instancia
     * @param vetClienteRota
     * @param tam
     * @param peso
     * @param tipoVeiculo
     * @param combustivel
     * @param poluicao
     * @param folga
     * @return bool Resultado
     ********************************************************************************************************** */

    bool criaRota(const Instancia::Instancia *const instancia, Solucao::ClienteRota *vetClienteRota, int tam,
                  const int peso, const int tipoVeiculo, double *combustivel, double *poluicao,
                  double *folga, TempoCriaRota *tempoCriaRota, double *vetLimiteTempo,
                  Solucao::ClienteRota *vetClienteRotaAux,
                  PertubacaoInviabilidade *inviabilidadeEstatisticas);

    void geraLimiteTempo(const Instancia::Instancia *const instancia, Solucao::ClienteRota *vetClienteRota, const int tam, const int tipoVeiculo, double *vetLimiteTempo);

    int comparaCliente(const void *p0, const void *p1);

}


#endif //HGVRSP_MOVIMENTOS_PARADAS_H
