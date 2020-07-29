//
// Created by igor on 17/10/19.
//Corrigido 26/11/19

#include "Instancia.h"
#include "Solucao.h"
#include <math.h>

typedef std::list<Solucao::Veiculo*>::iterator IteratorVeiculo;
typedef std::list<Solucao::ClienteRota*>::iterator IteratorCliente;

using namespace std;

#ifndef HGVRSP_VERIFICASOLUCAO_H
#define HGVRSP_VERIFICASOLUCAO_H


namespace VerificaSolucao
{


    bool verificaSolucao(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, string *texto,
                             double *distanciaTotal);
    double calculaPoluicao(double velocidade, double tempoViagem, const Instancia::Instancia *const instancia);
    double calculaPoluicaoRestrigindoVelocidade(const Instancia::Instancia *const instancia, int no1, int no2, double horaSaida, double distancia);


    long double
    poluicaoRota(const Instancia::Instancia *const instancia, int tipoVeiculo, long double distanciaParcial, int i, int j, int k);
    double poluicaoCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, double carga, double distanciaTotal);

    long double
    combustivelRota(const Instancia::Instancia *const instancia, int tipoVeiculo, long double distanciaParcial, int i, int j, int k);
    long double combustivelCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, double carga, double distanciaTotal);

    bool verificaVeiculo(Solucao::Veiculo *veiculo, const Instancia::Instancia *const instancia);
    bool verificaCombustivel(const long double combustivel, const Solucao::Veiculo *veiculo, const Instancia::Instancia *const instancia);
    bool verificaCombustivel(const long double combustivel, const int tipoVeiculo, const Instancia::Instancia *const instancia);

    bool verificaVeiculoRotaMip(Solucao::Veiculo *veiculo, const Instancia::Instancia *const instancia,
                                double *distanciaTotal, string *erro);




}

#endif //HGVRSP_VERIFICASOLUCAO_H
