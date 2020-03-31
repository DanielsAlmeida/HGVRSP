//
// Created by igor on 21/10/19.
//
#include "Instancia.h"
#include "Solucao.h"
#include <list>
#include "VerificaSolucao.h"
#include <sstream>

#ifndef HGVRSP_CONSTRUTIVO_H
#define HGVRSP_CONSTRUTIVO_H

typedef std::list<Solucao::ClienteRota *, std::allocator<Solucao::ClienteRota *>>::iterator ItClienteRota;
typedef std::tuple<bool, int, double, double> TupleBID;

#define HEURIST_DIST false

namespace Construtivo
{

    struct Candidato
    {

        double incrementoPoluicao;
        Solucao::Veiculo *veiculo;
        std::list<Solucao::ClienteRota *>::iterator posicao;
        Solucao::ClienteRota *candidato;
        double folgaRota; //Menor folga do veiculo.
        double distanciaDeposito;
        double distanciaArcos;
    };

    int compCandidato(const void* cand1, const void* cand2);
    int compCandidatoFolga(const void* cand1, const void* cand2);
    int compCandidatoDist(const void* cand1, const void* cand2);

    Solucao::Solucao *grasp(const Instancia::Instancia *const instancia, float *vetorAlfa, int tamAlfa,
                            const int numInteracoes, const int numIntAtualizarProb, bool log,
                            stringstream *strLog, const double parametro);

    Solucao::Solucao *geraSolucao(const Instancia::Instancia *const instancia, float alfa,
                                      Solucao::ClienteRota *vetorClienteBest,
                                      Solucao::ClienteRota *vetorClienteAux, string *sequencia, bool log,
                                      Construtivo::Candidato *vetorCandidatos, const double parametro);

    void insereCandidato(Candidato *candidato, const Instancia::Instancia *instancia, Solucao::ClienteRota *vetCliente);

    bool determinaHorario(Solucao::ClienteRota *cliente1, Solucao::ClienteRota *cliente2,
                              const Instancia::Instancia *const instancia, const int peso, const int tipoVeiculo);

    TupleBID viabilidadeInserirCandidato(Solucao::ClienteRota *vetorClientes, ItClienteRota iteratorCliente,
                                         const Instancia::Instancia *const instancia,
                                         Solucao::ClienteRota *candidato, double combustivelParcial,
                                         double poluicaoParcial, Solucao::Veiculo *veiculo, int peso,
                                         int posicao, double *folga);

    void atualizaProbabilidade(double *vetorProbabilidade, int *vetorFrequencia, double *solucaoAcumulada, double *vetorMedia, double *proporcao, int tam, double melhorSolucao);
    void atualizaPesos(double *beta, double *teta, int numClientes, const double parametro, const int k,
                       double *gama);




};


#endif //HGVRSP_CONSTRUTIVO_H
