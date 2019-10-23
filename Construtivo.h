//
// Created by igor on 21/10/19.
//
#include "Instancia.h"
#include "Solucao.h"
#include <list>

#ifndef HGVRSP_CONSTRUTIVO_H
#define HGVRSP_CONSTRUTIVO_H

namespace Construtivo
{

    Solucao::Solucao* geraSolucao(Instancia::Instancia *instancia, bool (*comparador)(Instancia::Cliente *, Instancia::Cliente * ));
    bool determinaHorario(const Solucao::ClienteRota* const cliente1, Solucao::ClienteRota *cliente2, Instancia::Instancia *instancia);

};


#endif //HGVRSP_CONSTRUTIVO_H
