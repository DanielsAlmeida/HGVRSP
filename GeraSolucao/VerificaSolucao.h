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


    bool geraSolucao(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, string *texto);
    double calculaPoluicao(double velocidade, double tempoViagem, const Instancia::Instancia *const instancia);
    double calculaPoluicaoRestrigindoVelocidade(const Instancia::Instancia *const instancia, int no1, int no2, double horaSaida, double distancia);


    double poluicaoRota(const Instancia::Instancia *const instancia, int tipoVeiculo, double distanciaParcial, int i, int j, int k);
    long double poluicaoCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, int carga, double distanciaTotal);

    double combustivelRota(const Instancia::Instancia *const instancia, int tipoVeiculo, double distanciaParcial, int i, int j, int k);
    double combustivelCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, int carga, double distanciaTotal);

    bool verificaVeiculo(Solucao::Veiculo *veiculo, const Instancia::Instancia *const instancia);

}

#endif //HGVRSP_VERIFICASOLUCAO_H
