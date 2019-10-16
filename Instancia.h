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
    //double pVeiculo;  //Acrescimo de combustivel por carga
    //double cVeiculo;  //Taxa conversao combustivel/co2
    //int inicioJanela;
    //int fimJanela;
}Veiculo;

typedef struct
{
    float a, b, c, d, e, f, g;
}Gas;

class Instancia
{

public:

    const Periodo vetorPeriodos[5] = {{0, 108}, {108, 216}, {216, 324}, {324, 432}, {432, 580}}; //Tempo em minutos
    Cliente *vetorClientes;
    const Veiculo vetorVeiculos = {15000, 200};
    float **matrizDistancias;
    float ***matrizVelocidade; //k,i,j
    //double ****matrizCo2;      //v, k, i, j
    int numClientes, numPeriodos, Numveiculos;//, veiculosTipo2;

    const int menorVelocidade = 6; // km/h
    const int maiorVelocidade = 90;//km/h

    //Constantes dos gases
    const Gas CO =  {2.0404e0 , 4.0540e-1, -1.7566e-2, 4.1924e-4, -5.7141e-6, 4.4735e-8, -1.4931e-10};
    const Gas HC = {3.594e-1, 9.3573e-2, -5.3987e-3, 1.4956e-4, -2.2094e-6, 1.7133e-8, -5.4005e-11};
    const Gas NOX = {5.0793E+1, -1.1020E+0, 3.3824E-1, -1.2620E-2, 2.0982E-4, -1.5928E-6, 4.5487E-9};
    const Gas PM = {7.5519e-1, 8.6426e-2, -4.3024e-3, 1.1100e-4, -1.6094e-6, 1.3039e-8, -4.4033e-11};
    const Gas co2 = {1.2690e+4, 1.6564e+1, 8.6867e+1, -3.5533e+0, 6.1462e-2, -4.7730e-4, 1.3853e-6};

    Instancia(std::string arquivo);

    ~Instancia();

private:

};

#endif //HGVRSP_INSTANCIA_H
