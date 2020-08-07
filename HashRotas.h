//
// Created by igor on 06/08/2020.
//

#include "Solucao.h"
#include <list>
#ifndef HGVRSP_HASHROTAS_H
#define HGVRSP_HASHROTAS_H

namespace HashRotas
{

    struct HashNo
    {
        Solucao::ClienteRota *veiculo;
        int tam;
        double poluicao;
        double combustivel;
        int carga;
        int tipo;
    };

    class HashRotas
    {
    public:

        u_int32_t tamTabela;
        std::list<HashNo*> *tabelaHash;

        HashRotas(int numClientes);
        ~HashRotas();
        u_int32_t getHash(Solucao::Veiculo *veiculo);
        u_int32_t getHash(Solucao::ClienteRota *clienteRota, const int tam);

        HashNo* getVeiculo(Solucao::ClienteRota *clienteRota, const int tam);
        HashNo* getVeiculo(Solucao::Veiculo *veiculo);

        bool insereVeiculo(Solucao::Veiculo *veiculo);

        void estatisticasHash(float *tamanhoMedio_, int *maior_);

    };


}

#endif //HGVRSP_HASHROTAS_H
