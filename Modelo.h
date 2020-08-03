//
// Created by igor on 09/07/2020.
//
#include "gurobi_c++.h"
#include "Instancia.h"

#ifndef HGVRSP_MODELO_H
#define HGVRSP_MODELO_H

namespace Modelo
{

    struct Variaveis
    {

        GRBVar **X;         //Variavel binária. Indica se o arco (i,j) é percorrido
        GRBVar ***x;        //Variavel binária. Indica se um arco (i,j) é percorrido em um período k
        GRBVar *T;          //Variável contínua. Indica se o veículo é do tipo 0 ou 1.

        GRBVar ***d;        //Variavel continua. Indica a distância percorrida do arco (i,j) no período k
        GRBVar ***tao;      //Variavel continua. Indica o tempo gasto no arco (i,j) no período k
        GRBVar *l;          //Variavel continua. Indica o tempo de saída do cliente i
        GRBVar *a;          //Variavel continua. Indica o tempo de chegada do cliente i
        GRBVar **f;         //Variavel continua. Indica a carga do veículo ao percorrer o arco (i,j)
        GRBVar *C;          //Variavel continua. Guarda o combustível do veiculo de tipo T.

        GRBLinExpr *funcaoObjetivo;

        Variaveis(){}
        ~Variaveis()
        {

        }


    };

    class Modelo
    {

    public:

        Variaveis *variaveis;
        GRBModel *modelo;
        bool tipoVeiculo;

        const int numClientes;
        const int numVeiculos;
        const int numPeriodos;

        const bool usaModeloVnd;

        Modelo(Instancia::Instancia *instancia, GRBModel *grbModel, const bool usaModeloVnd_);
        int criaRota(Solucao::ClienteRota *vetClienteRota, const int tam, bool tipo, int peso,
                     const Instancia::Instancia *instancia, double *poluicao, double *combustivel);
        ~Modelo();
    };
}


#endif //HGVRSP_MODELO_H
