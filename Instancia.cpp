//
// Created by igor on 10/09/19.
//

#include "Instancia.h"
#include <fstream>

Instancia::Instancia(std::string arquivo)
{

    std::string clientes = arquivo.substr(3,4);

    if(arquivo[5] >= '0' && arquivo[5]<= '9')
        clientes += arquivo[5];

    numClientes = std::atoi(clientes.c_str());



    std::ifstream file;
    file.open(arquivo, std::ios::out);

    if(file.is_open())
    {

    }
    else
    {
        std::cout<<"Nao foi possivel abrir o arquivo: "<<arquivo<<std::endl;
        exit(-1);
    }

}

Instancia::~Instancia()
{

}