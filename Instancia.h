//
// Created by igor on 10/09/19.
//
#include <iostream>

#ifndef HGVRSP_INSTANCIA_H
#define HGVRSP_INSTANCIA_H

typedef struct
{
    int inicio;
    int fim;

}Periodo;

typedef struct
{
    int cliente;
    int demanda;
    int tempoServico;
    int inicioJanela;
    int fimJanela;

}Cliente;

typedef struct
{
    int capacidade;
    int combustivel;
    double pVeiculo;  //Acrescimo de combustivel por carga
    double cVeiculo;  //Taxa conversao combustivel/co2
    int inicioJanela;
    int fimJanela;
}Veiculo;

class Instancia
{

public:

    Periodo vetorPeriodos[5] = {{0, 108}, {108, 216}, {216, 324}, {324, 432}, {432, 580}};
    Cliente *vetorClientes;
    Veiculo vetorVeiculos[2] = {{10000, 150, 2.2384e-005, 2.66374, 0, 540},{15000, 200, 1.79961e-005, 2.66374, 30, 540}};
    float **matrizDistancias;
    float ***matrizVelocidade; //k,i,j
    double ****matrizCo2;      //v, k, i, j

    int numClientes, numPeriodos, veiculosTipo1, veiculosTipo2;

    Instancia(std::string arquivo);

    ~Instancia();

private:

};

#endif //HGVRSP_INSTANCIA_H
