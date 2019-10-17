#include <iostream>
#include "Instancia.h"
#include "Solucao.h"

int main()
{
    //Instancia *instancia = new Instancia("/home/igor/Documentos/HGVRSP/test/UK_10x5_1.dat");
    //delete instancia;

    auto *solucao = new Solucao::Solucao(5);

    delete solucao;

    return 0;
}