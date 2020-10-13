//
// Created by igor on 10/09/19.
//
#include <iostream>
#include <cmath>
#include <map>
#include <chrono>
#include "Solucao.h"

#ifndef HGVRSP_INSTANCIA_H
#define HGVRSP_INSTANCIA_H



namespace Instancia
{

    typedef struct
    {
        double inicio;
        double fim;

    } Periodo;

    struct Cliente
    {
        int cliente;
        int demanda;
        double tempoServico;
        double inicioJanela;
        double fimJanela;
        double distanciaDeposito;
        int periodoFimJanela;


        bool operator == (const Cliente &cli1)
        {
            if(cli1.cliente == cliente)
                return true;
            else
                return false;
        }

    };

    typedef struct
    {
        int capacidade;
        int combustivel;
        double pVeiculo;  //Acrescimo de combustivel por carga
        double cVeiculo;  //Taxa conversao combustivel/co2
        double inicioJanela;
        double fimJanela;
    } Veiculo;

    typedef std::chrono::_V2::system_clock::time_point TimeType;

    /// Classe Instancia ...
    class Instancia
    {

    public:


        const Periodo vetorPeriodos[5] = {{0,   1.8},  //p0
                                          {1.8, 3.6},  //p1
                                          {3.6, 5.4},  //p2
                                          {5.4, 7.2},  //p3
                                          {7.2, 9.0}}; //p4   Tempo em horas
        Cliente *vetorClientes;
        const Veiculo vetorVeiculos[3] = {{10000,150,2.2384e-005,2.66374, 0.0, 9.0},{15000, 200, 1.79961e-005, 2.66374, 0.5, 9.0}, {FP_INFINITE, FP_INFINITE, 2.2384e-4, 2.66374, 0, 9}};
        double **matrizDistancias;
        double ***matrizVelocidade; //i,j,k
        double ****matrizCo2;      //i, j, k, v; k <- periodo, v <- veiculo
        int numClientes, numPeriodos, numVeiculos, veiculosTipo1, veiculosTipo2;
        double *distanciaDeposito;
        int demandaTotal;

        double tempoCpuPermitido;

        std::map<int,double> tempoLiteratura;
        std::map<int, double> tempoMedias;

        Instancia(std::string arquivo);
        int retornaPeriodo(double hora) const ;

        void getClientes();

        std::map<int, int> penalizacao;

        void atualizaPoluicaoSolucao(Solucao::Solucao *solucao) const;
        double poluicaoCientesNaoAtendidos(int num) const;

        ~Instancia();

    private:

    };

}

#endif //HGVRSP_INSTANCIA_H
