//
// Created by igor on 10/09/19.
//
#include <iostream>

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
        //double pVeiculo;  //Acrescimo de combustivel por carga
        //double cVeiculo;  //Taxa conversao combustivel/co2
        //int inicioJanela;
        //int fimJanela;
    } Veiculo;

    typedef struct
    {
        float a, b, c, d, e, f, g;

         float operator[] (int i) const
        {

            if(i == 0)
                return a;

            else if(i==1)
                return b;

            else if(i==2)
                return c;

            else if(i==3)
                return d;

            else if(i==4)
                return e;

            else if(i==5)
                return f;

            else if(i==6)
                return g;


        }

    } Gas;

    class Instancia
    {

    public:

        const Periodo vetorPeriodos[5] = {{0,   1.8},
                                          {1.8, 3.6},
                                          {3.6, 5.4},
                                          {5.4, 7.2},
                                          {7.2, 9.67}}; //Tempo em horas
        Cliente *vetorClientes;
        const Veiculo vetorVeiculos = {15000, 200};
        double **matrizDistancias;
        double ***matrizVelocidade; //i,j,k
        //double ****matrizCo2;      //v, k, i, j
        int numClientes, numPeriodos, Numveiculos;//, veiculosTipo2;

        int demandaTotal;

        const int MenorVelocidade = 6; // km/h
        const int MaiorVelocidade = 90;//km/h

        const double EsperaMax = 0.083333333; //Tempo maximo de espera para inicializar janela de tempo. 5 min.

        //Constantes dos gases
        const Gas CO = {2.0404e0, 4.0540e-1, -1.7566e-2, 4.1924e-4, -5.7141e-6, 4.4735e-8, -1.4931e-10};
        const Gas HC = {3.594e-1, 9.3573e-2, -5.3987e-3, 1.4956e-4, -2.2094e-6, 1.7133e-8, -5.4005e-11};
        const Gas NOX = {5.0793E+1, -1.1020E+0, 3.3824E-1, -1.2620E-2, 2.0982E-4, -1.5928E-6, 4.5487E-9};
        const Gas PM = {7.5519e-1, 8.6426e-2, -4.3024e-3, 1.1100e-4, -1.6094e-6, 1.3039e-8, -4.4033e-11};
        const Gas CO2 = {1.2690e+4, 1.6564e+1, 8.6867e+1, -3.5533e+0, 6.1462e-2, -4.7730e-4, 1.3853e-6};

        Instancia(std::string arquivo);
        int retornaPeriodo(float hora);

        ~Instancia();

    private:

    };

}

#endif //HGVRSP_INSTANCIA_H
