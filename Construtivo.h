//
// Created by igor on 21/10/19.
//
#include "Instancia.h"
#include "Solucao.h"
#include <list>
#include "VerificaSolucao.h"

#ifndef HGVRSP_CONSTRUTIVO_H
#define HGVRSP_CONSTRUTIVO_H

namespace Construtivo
{
    Solucao::Solucao *reativo(Instancia::Instancia *instancia, bool (*comparador)(Instancia::Cliente &, Instancia::Cliente &),
                                  float *vetorAlfa, int tamAlfa, const int numInteracoes, const int numIntAtualizarProb);
    Solucao::Solucao *geraSolucao(Instancia::Instancia *instancia,
                                  bool (*comparador)(Instancia::Cliente &, Instancia::Cliente &), float alfa,
                                  Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux);
    bool determinaHorario( Solucao::ClienteRota*  cliente1, Solucao::ClienteRota *cliente2, Instancia::Instancia *instancia);

    std::tuple<bool, int, float, float> viabilidadeInserirCandidato(Solucao::ClienteRota *vetorClientes, std::list<Solucao::ClienteRota *,
                                                                        std::allocator<Solucao::ClienteRota *>>::iterator iteratorCliente,
                                                                        Instancia::Instancia *instancia, Solucao::ClienteRota *candidato, double combustivelParcial, double poluicaoParcial,
                                                                        Solucao::Veiculo *veiculo);

    void atualizaProbabilidade(double *vetorProbabilidade, int *vetorFrequencia, double *solucaoAcumulada,
                                   double *vetorMedia, double *proporcao, int tam, double melhorSolucao);


};


#endif //HGVRSP_CONSTRUTIVO_H
