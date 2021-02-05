//
// Created by igor on 21/10/19.
//
#include "Instancia.h"
#include "Solucao.h"
#include <list>
#include "VerificaSolucao.h"
#include <sstream>
#include "boost/tuple/tuple.hpp"
#include "Vnd.h"
#include "Movimentos_Paradas.h"
#include "Modelo.h"
#include "Modelo_1_rota.h"
#include "Alvo.h"
#include "EstatisticasQualidade.h"

#ifndef HGVRSP_CONSTRUTIVO_H
#define HGVRSP_CONSTRUTIVO_H

typedef std::list<Solucao::ClienteRota *, std::allocator<Solucao::ClienteRota *>>::iterator ItClienteRota;
typedef std::tuple<bool, int, double, double> TupleBID;

#define HEURIST_DIST false

using namespace EstatisticasQualidadeN;

namespace Construtivo
{

    struct Candidato
    {

        double incrementoPoluicao;
        Solucao::Veiculo *veiculo;
        int indiceVeiculo;
        std::list<Solucao::ClienteRota *>::iterator posicao;
        Solucao::ClienteRota *candidato;
        double folgaRota; //Menor folga do veiculo.
        double distanciaDeposito;
        double distanciaArcos;
        double distanciaRotaCompleta;
        double poluicao;
        double combustivel;
    };

    struct GuardaCandInteracoes
    {
        bool valido;
        int indiceVeiculo;
        double poluicao;
        double combustivel;
        double distanciaRotaCompleta;
        int tam;
        std::list<Solucao::ClienteRota *>::iterator it;
        double folga;
        Solucao::Veiculo *veiculo;
    };

    int compCandidato(const void* cand1, const void* cand2);
    int compCandidatoFolga(const void* cand1, const void* cand2);
    int compCandidatoDist(const void* cand1, const void* cand2);
    int compCandidatoDistanciaRota(const void* cand1, const void* cand2);

    Solucao::Solucao *
    grasp(const Instancia::Instancia *const instancia, float *vetorAlfa, int tamAlfa, const int numInteracoes,
          const int numIntAtualizarProb, bool log, stringstream *strLog, boost::tuple<int, int> *VetHeuristica,
          const int tamVetHeuristica, const double *const vetorParametros, Vnd::EstatisticaMv *vetEstatisticaMv,
          Solucao::ClienteRota **matrixClienteBest, Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
          GuardaCandInteracoes *vetCandInteracoes, double *vetLimiteTempo, Modelo::Modelo *modelo,
          Modelo_1_rota::Modelo *modelo1Rota, const Instancia::TimeType timeStart, double *ptr_tempoMip2rotas,
          u_int64_t *totalInteracoes, const int opcao, const double tempoMax, const double alvo, Alvo::Alvo *alvoTempos,
          list<EstatisticasQualidade> &listaEstQual);

    Solucao::Solucao *
    geraSolucao(const Instancia::Instancia *const instancia, float alfa,
                Solucao::ClienteRota *vetorClienteBest,
                Solucao::ClienteRota *vetorClienteAux, string *sequencia, bool log,
                Construtivo::Candidato *vetorCandidatos, boost::tuple<int, int> heuristica,
                const double *const vetorParametros,
                Solucao::ClienteRota **matrixClienteBest,
                Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
                GuardaCandInteracoes *vetCandInteracoes, double *vetLimiteTempo);

    void insereCandidato(Candidato *candidato, const Instancia::Instancia *instancia,
                         Solucao::ClienteRota *vetCliente, Solucao::Solucao *solucao,
                         Movimentos_Paradas::TempoCriaRota *tempoCriaRota);

    bool determinaHorario(Solucao::ClienteRota *cliente1, Solucao::ClienteRota *cliente2,
                          const Instancia::Instancia *const instancia, const int peso, const int tipoVeiculo,
                          string *erro, double *tempoParaJanela);

    TupleBID viabilidadeInserirCandidato(Solucao::ClienteRota *vetorClientes, ItClienteRota iteratorCliente,
                                         const Instancia::Instancia *const instancia,
                                         Solucao::ClienteRota *candidato, double combustivelParcial,
                                         double poluicaoParcial, Solucao::Veiculo *veiculo, int peso,
                                         int posicao, double *folga,
                                         Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
                                         double *vetLimiteTempo, Solucao::ClienteRota *vetClienteRotaAux);

    void atualizaProbabilidade(double *vetorProbabilidade, int *vetorFrequencia, double *solucaoAcumulada, double *vetorMedia, double *proporcao, int tam, double melhorSolucao);
    void atualizaPesos(double *beta, double *teta, int numClientes, const int k, double *gama, const int Heuristica, const double *const vetorParametros);




};


#endif //HGVRSP_CONSTRUTIVO_H
