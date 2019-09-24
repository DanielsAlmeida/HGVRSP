#include <iostream>
#include "Instancia.h"
int main()
{
    Instancia *instancia = new Instancia("/home/igor/Documentos/HGVRSP/test/UK_10x5_1.dat");
    delete instancia;
    return 0;
}