//
// Created by igor on 09/07/2020.
//
#include <chrono>
#include "gurobi_c++.h"
#include "Instancia.h"
#include "HashRotas.h"

#ifndef HGVRSP_MODELO_H
#define HGVRSP_MODELO_H

namespace Modelo
{

    struct Variaveis
    {

        GRBVar ***X;         //Variavel binária. Indica se o arco (i,j) é percorrido
        GRBVar ****x;        //Variavel binária. Indica se um arco (i,j) é percorrido em um período k
        GRBVar **T;          //Variável contínua. Indica se o veículo é do tipo 0 ou 1.

        GRBVar ****d;        //Variavel continua. Indica a distância percorrida do arco (i,j) no período k
        GRBVar ****tao;      //Variavel continua. Indica o tempo gasto no arco (i,j) no período k
        GRBVar *l;          //Variavel continua. Indica o tempo de saída do cliente i
        GRBVar *a;          //Variavel continua. Indica o tempo de chegada do cliente i
        GRBVar ***f;         //Variavel continua. Indica a carga do veículo ao percorrer o arco (i,j)
        GRBVar **C;          //Variavel continua. Guarda o combustível do veiculo de tipo T.

        GRBLinExpr *funcaoObjetivo;
        GRBConstr *restricaoTrocaClientes;
        GRBConstr *restricaoPeso_veic0;
        GRBConstr *restricaoPeso_veic1;
        GRBConstr *restricaoUmArcoJ;
        GRBConstr *restricaoUmArcoI;
        GRBConstr restricaoUmArcoDepositoVeic1;
        GRBConstr restricaoUmArcoDepositoVeic0;
//1502
        Variaveis(){}
        ~Variaveis(){}


    };

    class Modelo
    {

    public:

        Variaveis *variaveis;
        GRBModel *modelo;
        bool tipoVeiculo0;
        bool tipoVeiculo1;

        const int numClientes;
        const int numVeiculos;
        const int numPeriodos;

        const bool usaModeloVnd;

        Modelo(Instancia::Instancia *instancia, GRBModel *grbModel, const bool usaModeloVnd_);
        int criaRota(Solucao::ClienteRota *vetClienteRota, int *tam, bool tipo, int *peso,
                     const Instancia::Instancia *instancia, double *poluicao, double *combustivel,
                     const int numArcos, int *vetRotaAux, Solucao::ClienteRota *vetClienteRota2, int *tam2,
                     const bool tipo2, int *peso2, double *poluicao2, double *combustivel2,
                     int *vetRotaAux2, bool trocaClientesEntreRotas);
        ~Modelo();
    };

    /*void geraRotasOtimas(Solucao::Solucao *solucao, Modelo *modelo, Solucao::ClienteRota *vetClienteRota,
                         const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas,
                         int *vetRotasAux, int *vetRotasOt, Solucao::ClienteRota *vetClienteRota2,
                         int *vetRotasAux2);*/

    class solucaoInteira : public GRBCallback
    {
    public:

        std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> c_start = std::chrono::high_resolution_clock::now();

        solucaoInteira() = default;
        void inicializaInicio()
        {
            c_start = std::chrono::high_resolution_clock::now();
        }

    protected:

        void callback()
        {
            try
            {

                if (where == GRB_CB_MIPSOL)
                {
                    auto c_end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> tempoCpu = c_end - c_start;

                    double obj = getDoubleInfo(GRB_CB_MIPSOL_OBJ);

                    std::cout<<"Nova solucao inteira: val: "<<obj<<" tempo: "<<tempoCpu.count()<<"s\n\n";
                }

            } catch (GRBException e)
            {
                std::cout << "Error number: " << e.getErrorCode() << '\n';
                std::cout << e.getMessage() << '\n';
            }

        }
    };

}


#endif //HGVRSP_MODELO_H
