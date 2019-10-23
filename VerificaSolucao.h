//
// Created by igor on 17/10/19.
//

#include "Instancia.h"
#include "Solucao.h"
#include <math.h>

#ifndef HGVRSP_VERIFICASOLUCAO_H
#define HGVRSP_VERIFICASOLUCAO_H


namespace VerificaSolucao
{


    bool verificaSolucao(Instancia::Instancia *instancia, Solucao::Solucao *solucao);
    double calculaPoluicao(double velocidade, double tempoViagem, Instancia::Instancia *instancia);
    double calculaConsumo(double velocidade, double tempoViagem, Instancia::Instancia *instancia);
}

#endif //HGVRSP_VERIFICASOLUCAO_H
