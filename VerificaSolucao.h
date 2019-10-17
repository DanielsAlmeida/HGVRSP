//
// Created by igor on 17/10/19.
//

#include "Instancia.h"
#include "Solucao.h"


#ifndef HGVRSP_VERIFICASOLUCAO_H
#define HGVRSP_VERIFICASOLUCAO_H


namespace VerificaSolucao
{


    bool verificaSolucao(Instancia::Instancia *instancia, Solucao::Solucao *solucao);
    float horaChegada(int clientePartida, int clienteDestino, float horaPartida, Solucao::Solucao *solucao);
}

#endif //HGVRSP_VERIFICASOLUCAO_H
