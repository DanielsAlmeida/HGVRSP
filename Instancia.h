//
// Created by igor on 10/09/19.
//
#include <iostream>
#include <cmath>
#include <map>

#ifndef HGVRSP_INSTANCIA_H
#define HGVRSP_INSTANCIA_H



namespace Instancia
{

    typedef struct
    {
        double inicio;
        double fim;

    } Periodo;

    typedef struct
    {
        int cliente;
        int demanda;
        double tempoServico;
        double inicioJanela;
        double fimJanela;
        double distanciaDeposito;
        int periodoFimJanela;

    } Cliente;

    typedef struct
    {
        int capacidade;
        int combustivel;
        double pVeiculo;  //Acrescimo de combustivel por carga
        double cVeiculo;  //Taxa conversao combustivel/co2
        double inicioJanela;
        double fimJanela;
    } Veiculo;


    class Instancia
    {

    public:


        const Periodo vetorPeriodos[5] = {{0,   1.8},
                                          {1.8, 3.6},
                                          {3.6, 5.4},
                                          {5.4, 7.2},
                                          {7.2, 9.0}}; //Tempo em horas
        Cliente *vetorClientes;
        const Veiculo vetorVeiculos[3] = {{10000,150,2.2384e-005,2.66374, 0.0, 9.0},{15000, 200, 1.79961e-005, 2.66374, 0.5, 9.0}, {FP_INFINITE, FP_INFINITE, 2.2384e-4, 2.66374, 0, 9}};
        double **matrizDistancias;
        double ***matrizVelocidade; //i,j,k
        double ****matrizCo2;      //i, j, k, v; k <- periodo, v <- veiculo
        int numClientes, numPeriodos, numVeiculos, veiculosTipo1, veiculosTipo2;

        int demandaTotal;

        Instancia(std::string arquivo);
        int retornaPeriodo(float hora) const ;

        void getClientes();

        std::map<int, int> penalizacao;


        ~Instancia();

    private:

    };

}

#endif //HGVRSP_INSTANCIA_H
