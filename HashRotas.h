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

        HashNo *best;

        double poluicaoH;
    };

    class HashRotas
    {
    public:

        u_int64_t tamTabela;
        std::vector<HashNo*> *tabelaHash;
        int numRotas;

        HashRotas(int numClientes);
        ~HashRotas();
        u_int64_t getHash(Solucao::Veiculo *veiculo);
        u_int64_t getHash(Solucao::ClienteRota *clienteRota, const int tam, const int tipo);

        bool getVeiculo(Solucao::ClienteRota *clienteRota, const int tam, const int tipo, double *poluicao, double *combustivel);
        HashNo* getVeiculo(Solucao::Veiculo *veiculo);

        bool insereVeiculo(Solucao::ClienteRota *clienteRotaOriginal, Solucao::ClienteRota *clienteBest,
                           double poluicaoBest, double combustivelBest, const int tam, const bool tipo,
                           const int carga);

        bool insereVeiculo(Solucao::ClienteRota *vetClienteRota, double poluicaoBest, double combustivelBest, const int tam, const bool tipo,
                           const int carga);

        void estatisticasHash(float *tamanhoMedio_, int *maior_);

    };


}

#endif //HGVRSP_HASHROTAS_H
