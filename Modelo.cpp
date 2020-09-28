//
// Created by igor on 09/07/2020.
// Erro: restricao_soma_tao_k_, //Restrição 7, restringe o somatorio de tempo do périodo k

//restricao_sum_arco_i_cliente_

#include "Modelo.h"
#include <cstdio>
#include <chrono>
#include "Construtivo.h"
#include "Exception.h"
#include "HashRotas.h"
#include "time.h"
#include "Constantes.h"

#define NumTrocas 3
#define NumVeiculos 2
#define MAX_NUM_INT_MIP 1

ExceptioViabilidadeMip exceptionViabilidadeMip;

Modelo::Modelo::Modelo(Instancia::Instancia *instancia, GRBModel *grbModel, const bool usaModeloVnd_)
        : numClientes(instancia->numClientes), numVeiculos(instancia->numVeiculos), numPeriodos(instancia->numPeriodos),modelo(grbModel), usaModeloVnd(usaModeloVnd_)
{

    const int NumVeic = 2;

    //Cria o modelo
    tipoVeiculo0 = false;
    variaveis = new Variaveis;

    //Parametros do modelo
    modelo->set(GRB_StringAttr_ModelName, "HGVRSP_model");
    modelo->set(GRB_IntParam_NumericFocus, 1);
    modelo->set(GRB_IntParam_ScaleFlag, -1);
    modelo->set(GRB_IntParam_Method, GRB_METHOD_PRIMAL); //GRB_METHOD_DUAL
    modelo->set(GRB_DoubleParam_ObjScale, -0.5);
    modelo->set(GRB_IntParam_BarHomogeneous, 1);
    modelo->set(GRB_IntParam_CrossoverBasis, 1);
    modelo->set(GRB_IntParam_GomoryPasses, 3);
    modelo->set(GRB_IntParam_Cuts, 3);
    modelo->set(GRB_IntParam_Presolve, 2);
    modelo->set(GRB_DoubleParam_IntFeasTol, 1e-4);
    modelo->set(GRB_DoubleParam_FeasibilityTol, 1e-4);
    modelo->set(GRB_DoubleParam_MIPGap, 0.18);
    modelo->set(GRB_IntParam_MIPFocus, GRB_MIPFOCUS_FEASIBILITY);
    modelo->set(GRB_DoubleParam_TimeLimit, 150);



    //Cria variaveis

    //*****************************************************************************************************************************************

    //Aloca variavel X
    variaveis->X = new GRBVar **[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
    {
        variaveis->X[h] = new GRBVar*[instancia->numClientes];

    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
            variaveis->X[h][i] = new GRBVar[instancia->numClientes];
    }

    //Cria veriaveis X_(h,i,j) no modelo
    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                    variaveis->X[h][i][j] = modelo->addVar(0, 0, 0, GRB_BINARY,"X_"+std::to_string(h) + '_' + std::to_string(i) + '_' + std::to_string(j));

            }
        }
    }
    //*****************************************************************************************************************************************


    //Aloca variavel x: Variavel binária. Indica se um arco (i,j) é percorrido em um período k pelo veículo h
    variaveis->x = new GRBVar ***[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
        variaveis->x[h] = new GRBVar**[instancia->numClientes];


    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
            variaveis->x[h][i] = new GRBVar *[instancia->numClientes];
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {

            for (int j = 0; j < instancia->numClientes; ++j)
                variaveis->x[h][i][j] = new GRBVar[instancia->numPeriodos];


        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {

                        variaveis->x[h][i][j][k] = modelo->addVar(0, 1, 0, GRB_BINARY, "x_" + std::to_string(h) + '_' + std::to_string(i) + '_' +
                                                                                    std::to_string(j) + '_' + std::to_string(k));

                    }
                }
            }
        }
    }

    //*****************************************************************************************************************************************
    //Cria a variável T


    variaveis->T = new GRBVar*[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
        variaveis->T[h] = new GRBVar[2];

    for(int h = 0; h < NumVeiculos; ++h)
    {
        variaveis->T[h][0] = modelo->addVar(0, 0, 0, GRB_BINARY, "T_" + std::to_string(h) + '_' + std::to_string(0));
        variaveis->T[h][1] = modelo->addVar(1, 1, 0, GRB_BINARY, "T_" + std::to_string(h) + '_' + std::to_string(1));
    }

    //*****************************************************************************************************************************************

    //Aloca variavel d: Variavel continua. Indica a distância percorrida do arco (i,j) no período k pelo veículo h

    variaveis->d = new GRBVar ***[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
    {
        variaveis->d[h] = new GRBVar** [instancia->numClientes];
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            variaveis->d[h][i] = new GRBVar *[instancia->numClientes];
        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
                variaveis->d[h][i][j] = new GRBVar[instancia->numPeriodos];

        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {

                        variaveis->d[h][i][j][k] = modelo->addVar(1e-5, GRB_INFINITY, 0, GRB_SEMICONT, "d_" + std::to_string(h) + '_' + std::to_string(i) +
                                                                  '_' + std::to_string(j) + '_' + std::to_string(k));

                    }
                }

            }
        }
    }

    //*************************************************************************************************************************************************************

    //Aloca a variavel tal: Variavel continua. Indica o tempo gasto no arco (i,j) no período k pelo veículo h

    variaveis->tao = new GRBVar ***[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
    {
        variaveis->tao[h] = new GRBVar **[instancia->numClientes];
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            variaveis->tao[h][i] = new GRBVar *[instancia->numClientes];
        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
                variaveis->tao[h][i][j] = new GRBVar[instancia->numPeriodos];

        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {

                        variaveis->tao[h][i][j][k] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS,"tao_" + std::to_string(h) + '_' + std::to_string(i) +
                                                                  '_' + std::to_string(j) + '_' + std::to_string(k));

                    }
                }


            }
        }
    }
    //*************************************************************************************************************************************************************

    //Aloca a variavel l: Variavel continua. Indica o tempo de saída do cliente i
    variaveis->l = new GRBVar[instancia->numClientes+1];

    for (int i = 0; i < instancia->numClientes + 1; ++i)
    {
        if(i < instancia->numClientes)
            variaveis->l[i] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "l_" + std::to_string(i));
        else
            variaveis->l[i] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "l_" + std::to_string(i) + "\'");
    }

    //*************************************************************************************************************************************************************

    //Aloca a variavel a: Variavel continua. Indica o tempo de chegada do cliente i
    variaveis->a = new GRBVar[instancia->numClientes+1];

    for (int i = 0; i < instancia->numClientes + 1; ++i)
    {
        if(i < instancia->numClientes)
            variaveis->a[i] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "a_" + std::to_string(i));
        else
            variaveis->a[i] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "a_" + std::to_string(i) + "_");
    }

    //*************************************************************************************************************************************************************

    //Aloca variavel f: Variavel continua. Indica a carga do veículo ao percorrer o arco (i,j) pelo veículo h
    variaveis->f = new GRBVar **[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)

        variaveis->f[h] = new GRBVar *[instancia->numClientes];


    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
            variaveis->f[h][i] = new GRBVar[instancia->numClientes];
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 0; j < instancia->numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    variaveis->f[h][i][j] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS,
                                                           "f_" + std::to_string(h) + '_' + std::to_string(i) + '_' +
                                                           std::to_string(j));
                    variaveis->f[h][i][j].set(GRB_DoubleAttr_Start, 0);
                }
            }
        }
    }
    //*************************************************************************************************************************************************************
    //Cria a variavel C

    variaveis->C = new GRBVar* [NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
        variaveis->C[h] = new GRBVar[2];


    for(int h = 0; h < NumVeiculos; ++h)
    {
        variaveis->C[h][0] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "C_"+std::to_string(h) + "_0");
        variaveis->C[h][1] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "C_1"+std::to_string(h) + "_1");
    }

    //*************************************************************************************************************************************************************
    //Cria a funcao objetivo
    variaveis->funcaoObjetivo = new GRBLinExpr[2*NumVeiculos];

    //*************************************************************************************************************************************************************
    //*************************************************************************************************************************************************************
    //ok
    //Adiciona as restrições

    //Restrição 1. Limita a distancia da variavel d. d[i,j,k] <= D [i,j] * x [i,j,k]

    GRBLinExpr linExprDist_d = 0;

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {

                    for (int k = 0; k < numPeriodos; ++k)
                    {
                        linExprDist_d = 0;
                        linExprDist_d = variaveis->d[h][i][j][k] - instancia->matrizDistancias[i][j] * variaveis->x[h][i][j][k];

                        GRBVar var = modelo->addVar(-1e-3, 1e-3, 0, GRB_CONTINUOUS,"var_erro_rest_restringe_d_" + std::to_string(h) + '_'  +
                                                     std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k));

                        modelo->addConstr(linExprDist_d + var <= 0.0,"restringe_d_" + std::to_string(h) + '_' + std::to_string(i) + "_" + std::to_string(j) +
                                                                                "_" + std::to_string(k));

                    }
                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Cria restrição 2, Zera a variavel x se (X = 0)
    //ok
    GRBLinExpr linExpr_limitaVar_x_por_X = 0;


    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {

                    for (int k = 0; k < numPeriodos; ++k)
                    {
                        linExpr_limitaVar_x_por_X = 0;
                        linExpr_limitaVar_x_por_X = variaveis->X[h][i][j] - variaveis->x[h][i][j][k];

                        modelo->addConstr(linExpr_limitaVar_x_por_X >= 0.0,"restrige_x_por_X_" + std::to_string(h) + '_' +
                                          std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k));

                    }
                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Cria restrição 3
    //ok
    GRBLinExpr linExpr = 0;

    /*for (int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
            {

                linExpr = 0;
                linExpr = variaveis->X[i][j];

                for (int k = 0; k < numPeriodos; ++k)
                    linExpr += -variaveis->x[i][j][k];

                modelo->addConstr(linExpr <= 0, "restringe_soma_x_k_por_X_" + std::to_string(i) + "_" + std::to_string(j));
            }
        }
    }*/

    //*************************************************************************************************************************************************************
    //Restrição 4 limita o somatorio da distância parcial à distância total
    //ok

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {

                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    linExpr = 0;

                    for (int k = 0; k < numPeriodos; ++k)
                        linExpr += variaveis->d[h][i][j][k];

                    GRBVar varAux = modelo->addVar(-1e-3, 1e-3, 0, GRB_CONTINUOUS, "var_erro_rest_restringe_soma_d_k_" +
                                                   std::to_string(h) + '_' + std::to_string(i) + "_" + std::to_string(j));

                    linExpr += -instancia->matrizDistancias[i][j] * variaveis->X[h][i][j] + varAux;
                    modelo->addConstr(linExpr == 0,"restringe_soma_d_k_" + std::to_string(h) + '_' + std::to_string(i) + "_" + std::to_string(j));


                }
            }
        }
    }
    //*************************************************************************************************************************************************************
    //Restriçao 5, sub-ciclo
    //ok



    for (int j = 1; j < numClientes; ++j)
    {

        for(int h = 0; h < NumVeiculos; ++h)
        {
            for (int k2 = 1; k2 < numPeriodos; ++k2)
            {
                for (int k1 = 0; k1 < k2; ++k1)
                {
                    linExpr = 0;


                    {
                        for (int l = 0; l < numClientes; ++l) {
                            if (l == j)
                                continue;

                            if (instancia->matrizDistancias[j][l] == 0.0)
                                continue;

                            linExpr += variaveis->x[h][j][l][k1];


                        }
                    }


                    {
                        for (int i = 0; i < numClientes; ++i) {
                            if (instancia->matrizDistancias[i][j] != 0.0)
                                linExpr += variaveis->x[h][i][j][k2];
                        }
                    }

                    modelo->addConstr(linExpr <= 1,
                                      "subCiclo_" + std::to_string(j) + "_k1_" + std::to_string(k1) + "_k2_" +
                                      std::to_string(k2) + "veic_"+std::to_string(h));
                }
            }
        }




    }

    //*************************************************************************************************************************************************************
    //Calcula o tempo gasto ao percorrer o arco i,j no periodo k
    //ok

    for(int h = 0; h < NumVeiculos; ++h)
    {

        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    for (int k = 0; k < numPeriodos; ++k)
                    {
                        linExpr = 0;
                        linExpr =
                                instancia->matrizVelocidade[i][j][k] * variaveis->tao[h][i][j][k] - variaveis->d[h][i][j][k];

                        modelo->addConstr(linExpr == 0, "tempo_" +  std::to_string(h) + '_' + std::to_string(i) + "_" + std::to_string(j) + "_" +
                                                        std::to_string(k));
                    }
                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Restrição 7, restringe o somatorio de tempo do périodo k
    //ok

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int k = 0; k < numPeriodos; ++k)
        {
            linExpr = 0;

            for (int i = 0; i < numClientes; ++i)
            {
                for (int j = 0; j < numClientes; ++j)
                {
                    if (instancia->matrizDistancias[i][j] != 0.0)
                        linExpr += variaveis->tao[h][i][j][k];
                }
            }


            double val = (instancia->vetorPeriodos[0].fim - instancia->vetorPeriodos[0].inicio);

            GRBVar erro = modelo->addVar(-5e-3, 0, 0, GRB_CONTINUOUS, "var_erro_restricao_soma_tao_k_" + std::to_string(k) + "_h_"+std::to_string(h));

            modelo->addConstr(linExpr + erro <= val, "restricao_soma_tao_k_" + std::to_string(k) + "veiculo_"+std::to_string(h));
        }
    }

    //*************************************************************************************************************************************************************
    // restrição 12, tempo de saida do cliente i, em um arco (i,j) no périodo k
    //ok
    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j < numClientes; ++j)
        {
            if(instancia->matrizDistancias[i][j] != 0)
            {
                for(int h = 0; h < NumVeiculos; ++h)
                {
                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {

                        linExpr = 0;

                        if((i != 0) || h == 0)
                            linExpr = variaveis->l[i];
                        else
                            linExpr = variaveis->l[instancia->numClientes];


                        linExpr += variaveis->tao[h][i][j][k];


                        linExpr += instancia->vetorPeriodos[4].fim * variaveis->x[h][i][j][k];

                        double val = instancia->vetorPeriodos[4].fim + instancia->vetorPeriodos[k].fim;
                        if((i != 0) || (h ==0) || (j != 0))
                        {
                            modelo->addConstr(linExpr <= val,
                                              "tempoSaida_cliente_" + std::to_string(i) + "_(" + std::to_string(i) +
                                              "_" + std::to_string(j) + ")_P_" + std::to_string(k) + "_veic_"+std::to_string(h));
                        }
                        else
                        {
                            modelo->addConstr(linExpr <= val,
                                              "tempoSaida_cliente_" + std::to_string(i) + "__(" + std::to_string(i) +
                                              "_" + std::to_string(j) + ")_P_" + std::to_string(k)+ "_veic_"+std::to_string(h));
                        }
                    }
                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Tempo de chegada - 13
    //ok
    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j < numClientes; ++j)
        {
            if(instancia->matrizDistancias[i][j] != 0)
            {
                for (int h = 0; h < NumVeiculos; ++h)
                {

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {

                        linExpr = 0;

                        if((j != 0) || h == 0)
                            linExpr = variaveis->a[j];
                        else
                            linExpr = variaveis->a[instancia->numClientes];


                        linExpr += -variaveis->tao[h][i][j][k];


                        linExpr += -instancia->vetorPeriodos[4].fim * variaveis->x[h][i][j][k];

                        double val = +instancia->vetorPeriodos[k].inicio - instancia->vetorPeriodos[4].fim;

                        if((j != 0) || h == 0)
                        {
                            modelo->addConstr(linExpr >= val, "tempoChegada_" + std::to_string(j) + "_(" + std::to_string(i) + "_" + std::to_string(j) + ")_P_" +
                                              std::to_string(k) + "veiculo_" + std::to_string(h));
                        }
                        else
                        {
                            modelo->addConstr(linExpr >= val, "tempoChegada_" + std::to_string(j) + "__(" + std::to_string(i) + "_" + std::to_string(j) + ")_P_" +
                                                              std::to_string(k) + "veiculo_" + std::to_string(h));
                        }

                    }

                }

            }
        }
    }

    //*************************************************************************************************************************************************************
    //Tempo chegada de j  - 14
    //ok
    for(int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0)
            {
                for(int h = 0; h < NumVeiculos; ++h)
                {

                    linExpr = 0;

                    if((j != 0) || h == 0)
                        linExpr = variaveis->a[j] - variaveis->l[i];
                    else
                        linExpr = variaveis->a[instancia->numClientes] - variaveis->l[instancia->numClientes];


                    linExpr += -instancia->vetorPeriodos[4].fim * variaveis->X[h][i][j];


                    for (int k = 0; k < numPeriodos; ++k)
                        linExpr += -variaveis->tao[h][i][j][k];


                    if((j != 0) || h == 0)
                    {
                        modelo->addConstr(linExpr >= -instancia->vetorPeriodos[4].fim,
                                          "tempoChegada_" + std::to_string(j) + "_Soma_k_(" + std::to_string(i) + "_" +
                                          std::to_string(j) + ")_veiculo_" + std::to_string(h));
                    }
                    else
                    {
                        modelo->addConstr(linExpr >= -instancia->vetorPeriodos[4].fim,
                                          "tempoChegada_" + std::to_string(j) + "__Soma_k_(" + std::to_string(i) + "_" +
                                          std::to_string(j) + ")_veiculo_" + std::to_string(h));
                    }
                }
            }
        }

    }

    //*************************************************************************************************************************************************************
    //Restrige tempo de saida de i _ 15
    //ok
    for(int i = 1; i < numClientes; ++i)
    {
        linExpr = 0;
        linExpr = variaveis->a[i]  - variaveis->l[i];

        modelo->addConstr(linExpr <= - instancia->vetorClientes[i].tempoServico, "restringeTempoSaida_"+std::to_string(i));
    }

    //*************************************************************************************************************************************************************
    //restrição 12, janela de tempo do cliente i
    //ok


    for(int i = 1; i < numClientes; ++i)
    {
        GRBVar varAux = modelo->addVar(-1.0/60.0, 0.0, 0, GRB_CONTINUOUS,"var_erro_rest_fim_janela_"+std::to_string(i));

        modelo->addConstr(variaveis->a[i] >= instancia->vetorClientes[i].inicioJanela, "inicio_janela_"+std::to_string(i));
        modelo->addConstr(variaveis->a[i]  + varAux <= (instancia->vetorClientes[i].fimJanela), "fim_janela_"+std::to_string(i));
    }

    //*************************************************************************************************************************************************************
    //Restrições de consumo de combustível - 13, 14
    //ok
    linExpr = 0;
    GRBLinExpr linExprAux = 0;

    const int inicio = 0;

    for(int h = 0; h < NumVeiculos; ++h)
    {


        for (int t = inicio; t < 2; ++t)
        {

            linExpr = 0;

            for (int i = 0; i < numClientes; ++i)
            {
                for (int j = 0; j < numClientes; ++j)
                {
                    if (instancia->matrizDistancias[i][j] != 0)
                    {

                        for (int k = 0; k < numPeriodos; ++k)
                            linExpr += instancia->matrizCo2[i][j][k][t] * variaveis->d[h][i][j][k];
                    }
                }
            }

            for (int i = 0; i < numClientes; ++i)
            {
                for (int j = 0; j < numClientes; ++j)
                {
                    if (instancia->matrizDistancias[i][j] != 0)
                        linExpr += instancia->vetorVeiculos[t].pVeiculo * instancia->matrizDistancias[i][j] * variaveis->f[h][i][j];
                }

            }

            linExpr += -variaveis->C[h][t];

            modelo->addConstr(linExpr == 0, "combustivelVeiculo_" + std::to_string(h)+"_tipo_" + std::to_string(t));

            linExprAux = 0;


            linExprAux = instancia->vetorVeiculos[t].combustivel - variaveis->T[h][t] * instancia->vetorVeiculos[t].combustivel + variaveis->T[h][t] * (2 * instancia->vetorVeiculos[1].combustivel);
            GRBVar var = modelo->addVar(-1e-3, 0, 0, GRB_CONTINUOUS,"Var_erro_restr_limitaCombustivelTipo_" + std::to_string(t) + "veiculo_"+std::to_string(h));


            modelo->addConstr(variaveis->C[h][t] + var <= linExprAux, "limitaCombustivel_Veic"+std::to_string(h) + "_tipo_" + std::to_string(t));

        }

        modelo->addConstr(variaveis->T[h][0] + variaveis->T[h][1] <= 1, "limitaTipoVeiculo_veic_" + std::to_string(h));

    }




    //*************************************************************************************************************************************************************

    //ok

    //-23

    for(int h = 0; h < NumVeiculos; ++h)
    {

        for (int u = inicio; u < 1; ++u)
        {


            for (int k = 0; k < numPeriodos; ++k)
            {
                linExpr = 0;
                double temp = 0.0;//instancia->vetorPeriodos[4].fim;

                for (int j = 1; j < numClientes; ++j)
                {
                    if (instancia->matrizDistancias[0][j] != 0)
                        linExpr += variaveis->tao[h][0][j][k] + instancia->vetorPeriodos[4].fim * variaveis->x[h][0][j][k];
                }

                double val = instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].inicioJanela + instancia->vetorPeriodos[k].fim;


                temp = -0.5;
                modelo->addConstr(linExpr <= val + temp * variaveis->T[h][0], "r_0_limita_tempo_saida_tipo_0_e_1_0_Periodo_" + std::to_string(k) + "veiculo_"+std::to_string(h));

            }

        }
    }

    //*************************************************************************************************************************************************************
    //-24

    for(int h = 0; h < NumVeiculos; ++h)
    {

        for (int u = inicio; u < 1; ++u)
        {
            for (int j = 1; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[0][j] != 0)
                    for (int k = 0; k < numPeriodos; ++k)
                    {
                        linExpr = 0;

                        if((j!=0) || (h ==0))
                            linExpr = -variaveis->a[j];
                        else
                            linExpr = -variaveis->a[instancia->numClientes];

                        linExpr += variaveis->tao[h][0][j][k] + instancia->vetorPeriodos[4].fim * variaveis->x[h][0][j][k];
                        double val = instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].inicioJanela;

                        double temp = -0.5;//instancia->vetorPeriodos[4].fim;


                        modelo->addConstr(linExpr <= val + temp * variaveis->T[h][0], "r_1_limita_tempo_saida_veiculo_tipo_0_e_1__0_" + std::to_string(j) + "_" +
                                          std::to_string(k) + "_veiculo_"+std::to_string(h));
                    }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //ok
    //-25

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int u = inicio; u < 1; ++u)
        {

            for (int k = 0; k < numPeriodos; ++k)
            {
                linExpr = 0;
                for (int i = 1; i < numClientes; ++i)
                {
                    if (instancia->matrizDistancias[i][0] != 0)
                        linExpr += -variaveis->tao[h][i][0][k] - instancia->vetorPeriodos[4].fim * variaveis->x[h][i][0][k];
                }
                double val = -instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].fimJanela + instancia->vetorPeriodos[k].inicio;

                modelo->addConstr(linExpr >= val, "r_2_limita_tempo_chegada_veiculo_" + std::to_string(h) + "_tipo_" + std::to_string(u) + "_Periodo_" + std::to_string(k));
            }

        }

    }

    //*************************************************************************************************************************************************************

    //-26

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int u = inicio; u < 1; ++u)
        {
            for (int i = 1; i < numClientes; ++i)
            {
                if (instancia->matrizDistancias[i][0] != 0)
                    for (int k = 0; k < numPeriodos; ++k)
                    {
                        linExpr = 0;

                        if((i!=0) || (h == 0))
                            linExpr = -variaveis->l[i];
                        else
                            linExpr = -variaveis->l[instancia->numClientes];


                        linExpr += - variaveis->tao[h][i][0][k] - instancia->vetorPeriodos[4].fim * variaveis->x[h][i][0][k];

                        double val = -instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].fimJanela;


                        modelo->addConstr(linExpr >= val,"r_3_limita_tempo_chegada_veiculo_" + std::to_string(h) + "_tipo_" + std::to_string(u) + '_' +
                                          std::to_string(i) + "_0_" + std::to_string(k));


                    }
            }
        }
    }
    //*************************************************************************************************************************************************************
    //Limita tempo de chegada, -18


    //modelo->addConstr(variaveis->a[0] - instancia->vetorVeiculos[1].fimJanela*variaveis->T[0] -  instancia->vetorVeiculos[0].fimJanela * variaveis->T[1] <= 0, "Restringe_tempo_chegada_deposito_veic");


    //*************************************************************************************************************************************************************
    //Cria a função objetivo

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int u = 0; u < 2; ++u)
        {
            variaveis->funcaoObjetivo[h*2+u] = variaveis->C[h][u] * instancia->vetorVeiculos[u].cVeiculo;
        }
    }

    modelo->setObjective(variaveis->funcaoObjetivo[0] + variaveis->funcaoObjetivo[2], GRB_MINIMIZE);
    tipoVeiculo0 = false;
    tipoVeiculo1 = false;

    //*************************************************************************************************************************************************************
    //Cria restricao que permite reqlizar x trocas de clientes em uma rota

    variaveis->restricaoTrocaClientes = new GRBConstr[NumVeiculos];

    for(int h = 0; h < NumVeiculos; ++h)
    {

        linExpr = 0;

        for (int i = 0; i < instancia->numClientes; ++i)
        {
            for (int j = 1; j < instancia->numClientes; ++j)
            {

                if (instancia->matrizDistancias[i][j] != 0.0)
                    linExpr += 0 * variaveis->X[h][i][j];
            }
        }

        variaveis->restricaoTrocaClientes[h] = modelo->addConstr(linExpr >= 0, "restricaoTrocaClientes_veic_"+std::to_string(h));
    }

    //*************************************************************************************************************************************************************
    //Cria restricao de peso

    variaveis->restricaoPeso_veic0 = new GRBConstr[instancia->numClientes];
    variaveis->restricaoPeso_veic1 = new GRBConstr[instancia->numClientes];

    for(int h = 0; h < NumVeiculos; ++h)
    {

        for (int j = 1; j < instancia->numClientes; ++j)
        {
            linExpr = 0;

            for (int i = 0; i < numClientes; ++i)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    linExpr += variaveis->f[h][i][j];
                }
            }

            for (int i = 0; i < numClientes; ++i)
            {

                if (instancia->matrizDistancias[j][i] != 0.0)
                {
                    linExpr += -variaveis->f[h][j][i];
                }
            }

            int demanda = instancia->vetorClientes[j].demanda;

            for (int i = 0; i < numClientes; ++i)
            {
                if (instancia->matrizDistancias[j][i] != 0.0)
                    linExpr += -demanda * variaveis->X[h][i][j];
            }

            if(h == 0)
                variaveis->restricaoPeso_veic0[j] = modelo->addConstr(linExpr == 0, "restricaoPeso_cliente_" + std::to_string(j) + "_veic_"+std::to_string(h));

            else
                variaveis->restricaoPeso_veic1[j] = modelo->addConstr(linExpr == 0, "restricaoPeso_cliente_" + std::to_string(j) +"_veic_"+ std::to_string(h));

        }

    }

    int pesoTotal = 0;

    for(int i = 1; i < numClientes; ++i)
        pesoTotal += instancia->vetorClientes[i].demanda;

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            linExpr = 0;
            for (int j = 1; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0)
                {
                    modelo->addConstr(variaveis->f[h][i][j] <= pesoTotal * variaveis->X[h][i][j]);
                }

            }
        }
    }

/*    linExpr = 0;

    for(int i = 1; i < instancia->numClientes; ++i)
    {
        if(instancia->matrizDistancias[i][0] != 0)
            linExpr += variaveis->f[i][0];
    }

    modelo->addConstr(linExpr == 0, "variaveis_f_i_0_zeradas");*/


    //*************************************************************************************************************************************************************
    //Restrige somatorio de arcos

    variaveis->restricaoUmArcoJ = new GRBConstr[instancia->numClientes];

    for(int j = 1; j < numClientes; ++j)
    {
        linExpr =  0;
        for(int h = 0; h < NumVeiculos; ++h)
        {
            for (int i = 0; i < numClientes; ++i)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    linExpr += variaveis->X[h][i][j];
                }
            }
        }

        variaveis->restricaoUmArcoJ[j] = modelo->addConstr(linExpr == 0, "restricao_sum_arco_cliente_" + std::to_string(j));
    }

    //variaveis->restricaoUmArcoJ[0].set(GRB_DoubleAttr_RHS, 1);

    /*linExpr = 0;
    for(int j = 1; j < numClientes; ++j)
    {
        if(instancia->matrizDistancias[0][j] != 0.0)
            linExpr += variaveis->X[0][j];
    }

    variaveis->restricaoUmArcoJ[numClientes] = modelo->addConstr(linExpr == 1, "restricao_sum_arco_cliente_0-");
    variaveis->restricaoUmArcoJ[0].set(GRB_DoubleAttr_RHS, 1);*/

    //

    variaveis->restricaoUmArcoI = new GRBConstr[instancia->numClientes];

    for(int i = 1; i < numClientes; ++i)
    {
        linExpr =  0;

        for(int h = 0; h < NumVeiculos; ++h)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0.0)
                {
                    linExpr += variaveis->X[h][i][j];
                }
            }
        }

        variaveis->restricaoUmArcoI[i] = modelo->addConstr(linExpr == 0, "restricao_sum_arco_i_cliente_" + std::to_string(i));
    }

    linExpr = 0;
    GRBLinExpr linExpr1 = 0;

    for(int i  = 1; i < instancia->numClientes; ++i)
    {

        if(instancia->matrizDistancias[i][0] != 0.0)
        {
            linExpr  += variaveis->X[0][i][0];
            linExpr1 += variaveis->X[1][i][0];
        }

    }

    variaveis->restricaoUmArcoDepositoVeic0 = modelo->addConstr(linExpr == 0, "restricao_Veiculo_0_retornaDeposito");
    variaveis->restricaoUmArcoDepositoVeic1 = modelo->addConstr(linExpr1 == 0, "restricao_Veiculo_1_retornaDeposito");

    //Restricao: somente um  veiculo atende um cliente

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for(int j = 0; j < numClientes; ++j)
        {
            linExpr = 0;

            for(int i = 0; i < numClientes; ++i)
            {
                if(instancia->matrizDistancias[i][j] != 0.0)
                    linExpr += variaveis->X[h][i][j];
            }

            for(int i = 0; i < numClientes; ++i)
            {
                if(instancia->matrizDistancias[j][i] != 0.0)
                    linExpr += -variaveis->X[h][j][i];
            }

            modelo->addConstr(linExpr == 0, "restricao_cliente_eh_atendido_por_um_veic_h_"+std::to_string(h) + "_j_"+std::to_string(j));

        }
    }

    for(int h = 0; h < NumVeic; ++h)
    {



            for(int j = 1; j < numClientes; ++j)
            {

                if (instancia->matrizDistancias[0][j] != 0.0)
                {
                    linExpr = 0;
                    linExpr += variaveis->f[h][0][j];


                    for (int t = 0; t < 2; ++t)
                    {
                        linExpr += -instancia->vetorVeiculos[t].capacidade;
                        linExpr += instancia->vetorVeiculos[t].capacidade * variaveis->T[h][t];
                    }

                    modelo->addConstr(linExpr <= 0, "restricaoPeso_veic_" + std::to_string(h) + "_cliente_"+std::to_string(j));
                }
            }


    }

}
Modelo::Modelo::~Modelo()
{


    //deleteta variaveis

    //*****************************************************************************************************************************************

    //Deleteta variavel X

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for(int i = 0; i < numClientes; ++i)
            delete []variaveis->X[h][i];
    }

    for(int h = 0; h < NumVeiculos; ++h)
        delete []variaveis->X[h];

    delete []variaveis->X;


    //*****************************************************************************************************************************************

    //Deleta variavel x
    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                delete []variaveis->x[h][i][j];
            }
        }
    }


    for (int h = 0; h < NumVeiculos; ++h)
    {

        for (int i = 0; i < numClientes; ++i)
        {

            delete []variaveis->x[h][i];
        }

    }

    for (int h = 0; h < NumVeiculos; ++h)
    {

        delete []variaveis->x[h];

    }

    delete []variaveis->x;



    //*****************************************************************************************************************************************

    //Deletando variavel d

    for(int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
                delete []variaveis->d[h][i][j];
        }

    }


    for (int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            delete []variaveis->d[h][i];

        }
    }

    for (int h = 0; h < NumVeiculos; ++h)
    {
        delete []variaveis->d[h];

    }

    delete []variaveis->d;



    //*****************************************************************************************************************************************

    //Deleta variavel tao

    for (int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                delete []variaveis->tao[h][i][j];
            }
        }
    }


    for (int h = 0; h < NumVeiculos; ++h)
    {
        for (int i = 0; i < numClientes; ++i)
        {
            delete []variaveis->tao[h][i];
        }
    }

    for (int h = 0; h < NumVeiculos; ++h)
    {
        delete []variaveis->tao[h];
    }

    delete []variaveis->tao;


    //*****************************************************************************************************************************************

    //Deleta variavel a

    delete []variaveis->a;


    //*****************************************************************************************************************************************

    //Deleta variavel l

    delete []variaveis->l;


    //*****************************************************************************************************************************************

    //Deleta variavel f

    for (int h = 0; h < NumVeiculos; ++h)
    {
        for(int i = 0; i < numClientes; ++i)
        {
            delete []variaveis->f[h][i];
        }
    }

    for (int h = 0; h < NumVeiculos; ++h)
    {
        delete []variaveis->f[h];
    }

    delete [] variaveis->f;

    //*****************************************************************************************************************************************
    //Deleta variavel C

    for (int h = 0; h < NumVeiculos; ++h)
        delete []variaveis->C[h];

    delete []variaveis->C;

    //*****************************************************************************************************************************************
    //Deleta variavel T

    for (int h = 0; h < NumVeiculos; ++h)
        delete []variaveis->T[h];

    delete []variaveis->T;

    //*****************************************************************************************************************************************
    //Deleta a funçao objetivo

    delete []variaveis->funcaoObjetivo;

    //*****************************************************************************************************************************************

    delete []variaveis->restricaoTrocaClientes;

    delete []variaveis->restricaoPeso_veic0;

    delete []variaveis->restricaoPeso_veic1;

    delete []variaveis->restricaoUmArcoJ;

    delete []variaveis->restricaoUmArcoI;

    modelo = NULL;
    delete variaveis;
}

int Modelo::Modelo::criaRota(Solucao::ClienteRota *vetClienteRota, int *tam, bool tipo, int *peso, const Instancia::Instancia *instancia, double *poluicao, double *combustivel,
                             const int numArcos, int *vetRotaAux, Solucao::ClienteRota *vetClienteRota2, int *tam2, const bool tipo2, int *peso2, double *poluicao2,
                             double *combustivel2, int *vetRotaAux2, bool trocaClientesEntreRotas)

{

    if ((!vetClienteRota) && (!vetClienteRota2))
    {
        cout << "\nArquivo: Modelo.cpp\n";
        cout << "Funcao: criaRotaMip\n";
        cout << "Linha: " << __LINE__ << "\n\n";
        cout << "Motivo: vetClienteRota e vetClienteRota2 iguail a NULL\n";
        exit(-1);
    }

    auto c_start = std::chrono::high_resolution_clock::now();

    static int rota[25];
    static int rota2[25];

    int rotaOri, rotaOri2;


    if (vetClienteRota)
    {
        rotaOri = *tam;

        for (int i = 0; i < *tam; ++i)
        {
            rota[i] = vetRotaAux[i] = vetClienteRota[i].cliente;

        }
    }

    if(vetClienteRota2)
    {
        rotaOri2 = *tam2;

        for(int i = 0; i < *tam2; ++i)
        {
            rota2[i] = vetRotaAux2[i] = vetClienteRota2[i].cliente;

        }
    }

    if(vetClienteRota)
        this->tipoVeiculo0 = tipo;

    if(vetClienteRota2)
    {
        this->tipoVeiculo1 = tipo2;
        variaveis->restricaoUmArcoDepositoVeic1.set(GRB_DoubleAttr_RHS, 1);

        if(vetClienteRota)
            modelo->setObjective(variaveis->funcaoObjetivo[tipo] + variaveis->funcaoObjetivo[2 + tipo2], GRB_MINIMIZE);
        else
            modelo->setObjective(variaveis->funcaoObjetivo[2 + tipo2], GRB_MINIMIZE);

    }
    else
        modelo->setObjective(variaveis->funcaoObjetivo[tipo], GRB_MINIMIZE);

    if(vetClienteRota)
    {   variaveis->restricaoUmArcoDepositoVeic0.set(GRB_DoubleAttr_RHS, 1);
        if(tipo)
        {
            variaveis->T[0][0].set(GRB_DoubleAttr_LB, 1);
            variaveis->T[0][0].set(GRB_DoubleAttr_UB, 1);

            variaveis->T[0][1].set(GRB_DoubleAttr_LB, 0);
            variaveis->T[0][1].set(GRB_DoubleAttr_UB, 0);
        } else
        {

            variaveis->T[0][0].set(GRB_DoubleAttr_LB, 0);
            variaveis->T[0][0].set(GRB_DoubleAttr_UB, 0);

            variaveis->T[0][1].set(GRB_DoubleAttr_LB, 1);
            variaveis->T[0][1].set(GRB_DoubleAttr_UB, 1);
        }
    }
    if(vetClienteRota2)
    {
        if (tipo2)
        {
            variaveis->T[1][0].set(GRB_DoubleAttr_LB, 1);
            variaveis->T[1][0].set(GRB_DoubleAttr_UB, 1);
            variaveis->T[1][1].set(GRB_DoubleAttr_LB, 0);
            variaveis->T[1][1].set(GRB_DoubleAttr_UB, 0);
        } else
            {
            variaveis->T[1][0].set(GRB_DoubleAttr_LB, 0);
            variaveis->T[1][0].set(GRB_DoubleAttr_UB, 0);

            variaveis->T[1][1].set(GRB_DoubleAttr_LB, 1);
            variaveis->T[1][1].set(GRB_DoubleAttr_UB, 1);
        }

    }

    //Muda o LB e UB de X dos arcos entre rotas

    if(vetClienteRota && vetClienteRota2)
        modelo->set(GRB_IntParam_Method, GRB_METHOD_BARRIER); //GRB_METHOD_DUAL

    else
        modelo->set(GRB_IntParam_Method, GRB_METHOD_PRIMAL); //GRB_METHOD_DUAL


    if(vetClienteRota && vetClienteRota2 && trocaClientesEntreRotas)
    {

        int cliente0, cliente1;

        for(int veic0 = 0; veic0 < (*tam - 1); ++veic0)
        {
            cliente0 = vetClienteRota[veic0].cliente;

            for(int veic1 = 0; veic1 < (*tam2 - 1); ++veic1)
            {

                cliente1 = vetClienteRota2[veic1].cliente;

                if(instancia->matrizDistancias[cliente0][cliente1] != 0.0)
                {
                    variaveis->X[0][cliente0][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[0][cliente0][cliente1].set(GRB_DoubleAttr_UB, 1);

                    variaveis->X[1][cliente0][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[1][cliente0][cliente1].set(GRB_DoubleAttr_UB, 1);

                    variaveis->X[0][cliente0][cliente1].set(GRB_DoubleAttr_Start, 0);
                    variaveis->X[1][cliente0][cliente1].set(GRB_DoubleAttr_Start, 0);
                }

                if(instancia->matrizDistancias[cliente1][cliente0] != 0.0)
                {
                    variaveis->X[1][cliente1][cliente0].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[1][cliente1][cliente0].set(GRB_DoubleAttr_UB, 1);

                    variaveis->X[0][cliente1][cliente0].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[0][cliente1][cliente0].set(GRB_DoubleAttr_UB, 1);

                    variaveis->X[1][cliente1][cliente0].set(GRB_DoubleAttr_Start, 0);
                    variaveis->X[0][cliente1][cliente0].set(GRB_DoubleAttr_Start, 0);

                }
            }
        }

        for(int i = 0; i < (*tam); ++i)
        {
            cliente0 = vetClienteRota[i].cliente;

            for(int j = i + 1; j < (*tam - 1); ++j)
            {

                cliente1 = vetClienteRota[j].cliente;

                if(instancia->matrizDistancias[cliente0][cliente1] != 0)
                {
                    variaveis->X[1][cliente0][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[1][cliente0][cliente1].set(GRB_DoubleAttr_UB, 1);
                    variaveis->X[1][cliente0][cliente1].set(GRB_DoubleAttr_Start, 0);
                }

                if(instancia->matrizDistancias[cliente1][cliente0] != 0)
                {
                    variaveis->X[1][cliente1][cliente0].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[1][cliente1][cliente0].set(GRB_DoubleAttr_UB, 1);
                    variaveis->X[1][cliente1][cliente0].set(GRB_DoubleAttr_Start, 0);
                }
            }

        }

        for(int i = 0; i < (*tam2 - 1); ++i)
        {
            cliente0 = vetClienteRota2[i].cliente;

            for(int j = i + 1; j < *tam2; ++j)
            {

                cliente1 = vetClienteRota2[j].cliente;

                if(instancia->matrizDistancias[cliente0][cliente1] != 0)
                {
                    variaveis->X[0][cliente0][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[0][cliente0][cliente1].set(GRB_DoubleAttr_UB, 1);
                    variaveis->X[0][cliente0][cliente1].set(GRB_DoubleAttr_Start, 0);
                }

                if(instancia->matrizDistancias[cliente1][cliente0] != 0)
                {
                    variaveis->X[0][cliente1][cliente0].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[0][cliente1][cliente0].set(GRB_DoubleAttr_UB, 1);
                    variaveis->X[0][cliente1][cliente0].set(GRB_DoubleAttr_Start, 0);
                }
            }



        }

    }

    int cliente1, cliente2;

    long double combustivelAux, poluicaoAux;
    bool viavel = true;
    int pesoAux;

    //Carrega solucao inicial

    /* **************************************************************************************************************************************/
    modelo->reset(0);

    for(int h = 0; h < NumVeiculos; ++h)
    {
        Solucao::ClienteRota *vetClienteAux;
        int tamAux;

        if(h == 0)
        {
            if(!vetClienteRota)
                continue;

            vetClienteAux = vetClienteRota;
            tamAux = *tam;

            combustivelAux = *combustivel;
            poluicaoAux = *poluicao;
            pesoAux = *peso;
        }
        else
        {
            if(!vetClienteRota2)
                break;

            vetClienteAux = vetClienteRota2;
            tamAux = *tam2;

            combustivelAux = *combustivel2;
            poluicaoAux = * poluicao2;
            pesoAux = *peso2;
        }

        //Inicio
        if (viavel)
        {


            variaveis->C[h][tipo].set(GRB_DoubleAttr_Start, combustivelAux);

            for (int i = 1; i < tamAux - 1; ++i)
            {
                //Verifica se tempo de chegada é menor que inicio da janela de tempo
                if (vetClienteAux[i].tempoChegada < instancia->vetorClientes[vetClienteAux[i].cliente].inicioJanela)
                {

                    int ultimoPeriodo = -1;

                    //Encontra o ultimo periodo percorrido
                    for (int k = 4; k >= 0; --k)
                    {
                        if (vetClienteAux[i].percorrePeriodo[k])
                        {
                            ultimoPeriodo = k;
                            break;
                        }
                    }

                    //Verifica condicao invalida
                    if (ultimoPeriodo != 0)
                    {
                        cout << "\n\nErro, ultimo periodo nao eh 0\n";
                        cout << "\nArquivo: Modelo.cpp\n";
                        cout << "Funcao: criaRotaMip\n";
                        cout << "Linha: " << __LINE__ << "\n\n";
                        exit(-1);
                    }

                    //altera o tempo de cheagada do clienteJ e tempo de saida do clienteI
                    vetClienteAux[i].tempoChegada = instancia->vetorClientes[vetClienteAux[i].cliente].inicioJanela;
                    vetClienteAux[i - 1].tempoSaida = vetClienteAux[i - 1].tempoSaida + (instancia->vetorClientes[vetClienteAux[i].cliente].inicioJanela - vetClienteAux[i].tempoChegada);
                }
            }
        }

        for (int i = 0; i < tamAux - 1; ++i)
        {
            cliente1 = vetClienteAux[i].cliente;

            for (int j = i + 1; j < tamAux; ++j)
            {
                //Percorre todos os pares i,j da rota

                cliente2 = vetClienteAux[j].cliente;

                //Verifica se existe o arco i,j
                if (instancia->matrizDistancias[cliente1][cliente2] != 0.0)
                {
                    //Muda os limites superiores de X e f. Destrava x e f
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 1);
                    //variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, GRB_INFINITY); //variavel f



                    if (viavel)
                    {
                        //Velor inicial é 0
                        variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);

                        //Inicializa x

                        for (int k = 0; k < instancia->numPeriodos; ++k)
                        {
                            variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0.0);
                            variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0.0);
                        }

                    }
                    //modelo->chgCoeff(variaveis->restricaoTrocaClientes[h], variaveis->X[h][cliente1][cliente2], 1);
                }

                //Verifica se existe o arco j,i
                if (instancia->matrizDistancias[cliente2][cliente1] != 0.0)
                {
                    //Muda os limites superiores de X e f. Destrava x e f
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 1);


                    if (viavel)
                    {
                        //Velor inicial é 0
                        variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);

                        for (int k = 0; k < instancia->numPeriodos; ++k)
                        {
                            variaveis->x[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->tao[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0.0);
                            variaveis->d[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0.0);
                        }
                    }


                }
            }
        }



        for (int i = 1; i < tamAux; ++i)
        {
            //Percorre as arestas da rota

            cliente1 = vetClienteAux[i - 1].cliente;
            cliente2 = vetClienteAux[i].cliente;

            //Inicializa x
            if (viavel)
            {
                variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 1);

            }


            //inicializa peso
            if (viavel)
                variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, pesoAux);

            modelo->chgCoeff(variaveis->restricaoTrocaClientes[h], variaveis->X[h][cliente1][cliente2], 1);


            if (viavel)
            {

                //Inicializa tempo de saida do cliente1 e tempo de chegada do cliente2

                if((cliente1 != 0) || (h == 0))
                {
                    variaveis->l[cliente1].set(GRB_DoubleAttr_Start, vetClienteAux[i - 1].tempoSaida);

                }
                else
                {
                    variaveis->l[instancia->numClientes].set(GRB_DoubleAttr_Start, vetClienteAux[i - 1].tempoSaida);

                }

                if((cliente2 != 0) || (h == 0))
                    variaveis->a[cliente2].set(GRB_DoubleAttr_Start, vetClienteAux[i].tempoChegada);
                else
                    variaveis->a[instancia->numClientes].set(GRB_DoubleAttr_Start, vetClienteAux[i].tempoChegada);

                //Percorre os periodos
                for (int k = 0; k < instancia->numPeriodos; ++k)
                {
                    //Inicializa x i,j,k
                    variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, vetClienteAux[i].percorrePeriodo[k]);


                    if (vetClienteAux[i].percorrePeriodo[k])
                    {
                        //Inicializa o tempo e distancia gastos no periodo k
                        variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, vetClienteAux[i].tempoPorPeriodo[k]);
                        variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, vetClienteAux[i].distanciaPorPeriodo[k]);



                    } else
                    {
                        //periodo nao é usado. Inicializa com 0
                        variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);

                    }
                }
            }

            pesoAux -= instancia->vetorClientes[cliente2].demanda;



        }

        for (int i = 1; i < tamAux - 1; ++i)
        {
            //Muda a restricao do somatorio de X para 1
            variaveis->restricaoUmArcoJ[vetClienteAux[i].cliente].set(GRB_DoubleAttr_RHS, 1);
            variaveis->restricaoUmArcoI[vetClienteAux[i].cliente].set(GRB_DoubleAttr_RHS, 1);
        }

        //tam - 1 => numero de arcos da rota. numArcos => numero de arcos que podem ser trocados
        int num = (tamAux - 1) - numArcos;

        num = ( num < 0 ? 0 : num);


        //Muda o lado direito da restricao de trocar clientes para num. num -> numero de arcos que  nao podem ser trocados
        variaveis->restricaoTrocaClientes[h].set(GRB_DoubleAttr_RHS, num);
    }

    /* **************************************************************************************************************************************/


    double ultimaPoluicao, ultimaPoluicao2;
    int ultimoTam, ultimoTam2;

    if(vetClienteRota)
    {
        ultimaPoluicao = *poluicao;
        ultimoTam = *tam;
    }

    if(vetClienteRota2)
    {
        ultimaPoluicao2 = *poluicao2;
        ultimoTam2 = *tam2;
    }

    int numInteracoes = 0;

    int aux;

    if(numArcos == 0)
        aux = 1;
    else
        aux = 3;

    const int MaxNumInteracoes = (trocaClientesEntreRotas ? 1 : 3);

    if(trocaClientesEntreRotas)
        modelo->set(GRB_DoubleParam_MIPGap, 0.19);
    else
        modelo->set(GRB_DoubleParam_MIPGap, 0.05);

    for(int p = 0; p < MaxNumInteracoes; ++p)
    {
        ++numInteracoes;

        modelo->update();
        modelo->write("modelo.lp");

        //modelo->feasRelax(1, false, false, true);


        modelo->optimize();
        modelo->write("modelo.sol");

        if(!((modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (modelo->get(GRB_IntAttr_Status) == GRB_TIME_LIMIT)))
        {

            cout << "\nArquivo: Modelo.cpp\n";
            cout << "Funcao: criaRotaMip\n";
            cout << "Linha: " << __LINE__ << "\n\n";
            cout<<"Motivo: Modelo inviavel\n";

            modelo->computeIIS();
            modelo->write("modelo.ilp");
            exit(-1);

        }

        bool resultado = (((modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (modelo->get(GRB_IntAttr_Status) == GRB_TIME_LIMIT)));

        if (!resultado)
            return 0;

        if(vetClienteRota)
        {
            *combustivel = variaveis->C[0][tipo].get(GRB_DoubleAttr_X);
            *poluicao = *combustivel * instancia->vetorVeiculos[tipo].cVeiculo;
        }

        if(vetClienteRota2)
        {
            *combustivel2 = variaveis->C[1][tipo2].get(GRB_DoubleAttr_X);
            *poluicao2 = *combustivel2 * instancia->vetorVeiculos[tipo2].cVeiculo;
        }


        //Calcula o gap
        double gap1 = -.05, gap2 = -.05;

        if(vetClienteRota)
        {
            gap1 = (*poluicao - ultimaPoluicao)/ultimaPoluicao;
            ultimaPoluicao = *poluicao;
        }

        if(vetClienteRota2)
        {
            gap2 = (*poluicao2 - ultimaPoluicao2) / ultimaPoluicao2;
            ultimaPoluicao2 = *poluicao2;
        }



        //Zera os coeficientes da restricao de troca clientes

        Solucao::ClienteRota *vetClienteAux;
        int tamAux;

        for(int h = 0; h < NumVeiculos; ++h)
        {
            if(h == 0)
            {

                if(!vetClienteRota)
                    continue;

                vetClienteAux = vetClienteRota;
                tamAux = *tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteAux = vetClienteRota2;
                tamAux = *tam2;
            }

            for (int i = 0; i < tamAux - 1; ++i)
            {
                cliente1 = vetClienteAux[i].cliente;
                cliente2 = vetClienteAux[i + 1].cliente;

                modelo->chgCoeff(variaveis->restricaoTrocaClientes[h], variaveis->X[h][cliente1][cliente2], 0.0);
            }

            variaveis->restricaoTrocaClientes[h].set(GRB_DoubleAttr_RHS, 0);
        }
        //Recuperar a sequencia de clientes

        for(int h = 0; h < NumVeiculos; ++h)
        {

            if(h == 0)
            {

                if(!vetClienteRota)
                    continue;

                vetClienteAux = vetClienteRota;

            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteAux = vetClienteRota2;

            }

            cliente1 = 0;
            tamAux = 1;  //Deposito

            do
            {

                cliente2 = -1;

                //Percorre todas as arestas de cliente1
                for (int j = 0; j < numClientes; ++j)
                {
                    if (instancia->matrizDistancias[cliente1][j] != 0.0)
                    {
                        double l = variaveis->X[h][cliente1][j].get(GRB_DoubleAttr_X);

                        if (l < 0.1)
                        {

                            continue;
                        }

                        bool r = bool(l);

                        if (r)
                        {
                            //atualiza cliente2
                            cliente2 = j;
                            break;
                        }
                    }
                }

                //Verifica erro
                if (cliente2 == -1)
                {
                    cout << "ERRO, \nArquivo: modelo.cpp\nFuncao: criaRota\nMotivo: Deveria existir um arco\nLinha: "
                         << __LINE__ << '\n';
                    cout << "tipo: " << tipo << '\n';
                    cout << "Rota: ";
                    if(h == 0)
                    {
                        for (int i = 0; i < rotaOri; ++i)
                            cout << rota[i] << ' ';
                        cout << '\n';
                    }
                    else
                    {
                        for (int i = 0; i < rotaOri2; ++i)
                            cout << rota2[i] << ' ';
                        cout << '\n';
                    }

                    exit(-1);
                }

                //Atualiza vetor e cliente1
                vetClienteAux[tamAux].cliente = cliente2;
                cliente1 = cliente2;
                ++tamAux;

            }while(cliente1 != 0);

            if(vetClienteRota && (h == 0))
                *tam = tamAux;

            if(vetClienteRota2 && (h == 1))
                *tam2 = tamAux;
        }


        if((gap1 > -.04) && (gap2 > -.04))
            break;

        //Verifica se existe uma proxima interaca
        if((p + 1) < MaxNumInteracoes)
        {
            bool iguais = true;
            bool iguais2 = true;

            //Verifica se a seguencia é igual a da interacao anterior

            if(vetClienteRota)
            {   if(*tam == ultimoTam)
                {
                    for(int i = 0; i < *tam; ++i)
                    {
                        if(vetClienteRota[i].cliente != vetRotaAux[i])
                        {
                            iguais = false;
                            break;
                        }
                    }
                }
                else
                    iguais = false;
            }

            if(vetClienteRota2)
            {
                if(*tam2 == ultimoTam2)
                {
                    for(int i = 0; i < *tam2; ++i)
                    {
                        if(vetClienteRota2[i].cliente != vetRotaAux2[i])
                        {
                            iguais2 = false;
                            break;
                        }

                    }
                }
                else
                    iguais2 = false;
            }

            if(iguais && iguais2)
                break;

            if(vetClienteRota)
            {
                for(int i = 0; i < *tam; ++i)
                    vetRotaAux[i] = vetClienteRota[i].cliente;

                ultimoTam = *tam;
            }

            if(vetClienteRota2)
            {

                for(int i = 0; i < *tam2; ++i)
                    vetRotaAux2[i] = vetClienteRota2[i].cliente;

                ultimoTam2 = *tam2;
            }

            for(int h = 0; h < NumVeiculos; ++h)
            {
                if(h==0)
                {

                    if(!vetClienteRota)
                        continue;

                    vetClienteAux = vetClienteRota;
                    tamAux = *tam;
                }
                else
                {
                    if(!vetClienteRota2)
                        break;

                    vetClienteAux = vetClienteRota2;
                    tamAux = *tam2;
                }

                for (int i = 0; i < tamAux - 1; ++i)
                {
                    cliente1 = vetClienteAux[i].cliente;
                    cliente2 = vetClienteAux[i + 1].cliente;

                    //coloca 1 nos coeficientes da restricao de troca clientes
                    modelo->chgCoeff(variaveis->restricaoTrocaClientes[h], variaveis->X[h][cliente1][cliente2], 1);
                }
            }
        }

    }


    /* ************************************************************************************************************************************************************ */

    //Pega a solucao

    Solucao::ClienteRota *vetClienteRotaAux;
    int tamAux;


    for(int h = 0; h < NumVeiculos; ++h)
    {
        if(h == 0)
        {

            if(!vetClienteRota)
                continue;

            vetClienteRotaAux = vetClienteRota;
            tamAux = *tam;
        }
        else
        {
            if(!vetClienteRota2)
                break;

            vetClienteRotaAux = vetClienteRota2;
            tamAux = *tam2;
        }


        for (int i = 1; i < tamAux - 1; ++i)
        {
            if(vetClienteRotaAux[i].cliente == 0)
                cout<<"Erro cliente: "<<vetClienteRotaAux[i].cliente<<'\n';

            //Volta o lodo direito de X para 0
            variaveis->restricaoUmArcoJ[vetClienteRotaAux[i].cliente].set(GRB_DoubleAttr_RHS, 0);
            variaveis->restricaoUmArcoI[vetClienteRotaAux[i].cliente].set(GRB_DoubleAttr_RHS, 0);
        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        if (h == 0)
        {

            if(!vetClienteRota)
                continue;

            vetClienteRotaAux = vetClienteRota;
            tamAux = *tam;
        } else
        {
            if (!vetClienteRota2)
                break;

            vetClienteRotaAux = vetClienteRota2;
            tamAux = *tam2;
        }

        for (int i = 0; i < tamAux - 1; ++i)
        {
            cliente1 = vetClienteRotaAux[i].cliente;

            for (int j = i + 1; j < tamAux; ++j)
            {
                //Percorre todos os pares de i,j
                cliente2 = vetClienteRotaAux[j].cliente;


                if (instancia->matrizDistancias[cliente1][cliente2] != 0.0)
                {
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 0);
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);
                    variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

                    //variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 0); //variavel f

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {
                        variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                    }

                }

                if (instancia->matrizDistancias[cliente2][cliente1] != 0.0)
                {
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 0);
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);
                    variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

//                  variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 0); //variavel f

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {
                        variaveis->x[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->d[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->tao[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                    }


                }
            }
        }


    }



    bool resultado = modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL;

    if(vetClienteRota)
    {
        variaveis->C[0][tipo].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
        variaveis->restricaoUmArcoDepositoVeic0.set(GRB_DoubleAttr_RHS, 0);
    }
    if(vetClienteRota2)
    {
        variaveis->C[1][tipo2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
        variaveis->restricaoUmArcoDepositoVeic1.set(GRB_DoubleAttr_RHS, 0);
    }

    if (viavel)
    {
        for(int h = 0; h < NumVeiculos; ++h)
        {

            if(h == 0)
            {

                if(!vetClienteRota)
                    continue;

                vetClienteRotaAux = vetClienteRota;
                tamAux = *tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteRotaAux = vetClienteRota2;
                tamAux = *tam2;
            }


            for (int i = 0; i < (tamAux - 1); ++i)
            {
                cliente1 = vetClienteRotaAux[i].cliente;


                for(int j = 0; j < (tamAux - 1); ++j)
                {

                    cliente2 = vetClienteRotaAux[j].cliente;

                    if(instancia->matrizDistancias[cliente1][cliente2] == 0.0)
                        continue;

                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);
                    variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);

                    //Muda o valor inicial para indefinido

                    if ((cliente1 != 0) || h == 0)
                        variaveis->l[cliente1].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    else
                        variaveis->l[instancia->numClientes].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

                    if ((cliente2 != 0) || (h == 0))
                        variaveis->a[cliente2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    else
                        variaveis->a[instancia->numClientes].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {

                        variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                        variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                    }
                }

            }


        }
    }




    if ((modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL) || (modelo->get(GRB_IntAttr_Status) == GRB_TIME_LIMIT))
    {
        for(int h = 0; h < NumVeiculos; ++h)
        {

            if(h == 0)
            {
                if(!vetClienteRota)
                    continue;



                vetClienteRotaAux = vetClienteRota;
                tamAux = *tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteRotaAux = vetClienteRota2;
                tamAux = *tam2;
            }

            cliente1 = 0;

            for (int i = 0; i < tamAux; ++i)
            {


                //recupera tempo de chegada e saida do cliente 1
                if (i != 0 && i != tamAux - 1)
                {
                    vetClienteRotaAux[i].tempoChegada = variaveis->a[cliente1].get(GRB_DoubleAttr_X);
                }

                if((cliente1!=0) || (h ==0))
                    vetClienteRotaAux[i].tempoSaida = variaveis->l[cliente1].get(GRB_DoubleAttr_X);
                else
                    vetClienteRotaAux[i].tempoSaida = variaveis->l[instancia->numClientes].get(GRB_DoubleAttr_X);

                if (i == (tamAux - 1))
                    break;

                cliente2 = vetClienteRotaAux[i + 1].cliente;

                for (int k = 0; k < numPeriodos; ++k)
                {
                    //Recupera tempo e distancia parcial

                    vetClienteRotaAux[i + 1].tempoPorPeriodo[k] = variaveis->tao[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X);
                    vetClienteRotaAux[i + 1].distanciaPorPeriodo[k] = variaveis->d[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X);
                    double l = variaveis->d[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X);
                    vetClienteRotaAux[i + 1].percorrePeriodo[k] = bool(variaveis->x[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X));

                }

                cliente1 = cliente2;


            }


            //Verifica se ultima psicao é o deposito
            if (vetClienteRotaAux[tamAux - 1].cliente != 0)
            {
                cout<< "ERRO, \nArquivo: modelo.cpp\nFuncao: criaRota\nMotivo: Ultima possicao diferente do deposito\n";
                cout << "tipo: " << tipo << '\n';
                cout << "Rota: ";

                if(h == 0)
                {
                    for (int i = 0; i < rotaOri; ++i)
                        cout << rota[i] << ' ';
                    cout << '\n';

                    cout << "Rota gerada: ";
                    for (int i = 0; i < *tam; ++i)
                        cout << vetClienteRota[i].cliente << ' ';
                    cout << '\n';
                }
                else
                {
                    for (int i = 0; i < rotaOri2; ++i)
                        cout << rota2[i] << ' ';
                    cout << '\n';

                    cout << "Rota gerada: ";
                    for (int i = 0; i < *tam2; ++i)
                        cout << vetClienteRota2[i].cliente << ' ';
                    cout << '\n';
                }
                exit(-1);
            }

            //Altera o tempo de chegada do primeiro cliente
            if (vetClienteRotaAux[1].tempoChegada < instancia->vetorClientes[vetClienteRotaAux[1].cliente].inicioJanela)
                vetClienteRotaAux[1].tempoChegada = instancia->vetorClientes[vetClienteRotaAux[1].cliente].inicioJanela;


            int ultimoPeriodo = -1, quant = 0, primeiroPeriodo = 5;

            for (int k = 0; k < numPeriodos; ++k)
            {


                if (vetClienteRotaAux[1].percorrePeriodo[k])
                {

                    ++quant;
                    ultimoPeriodo = k;

                    if (k < primeiroPeriodo)
                        primeiroPeriodo = k;
                }
            }

            //Calcula tempo de saida do deposito
            if (quant == 1)
                vetClienteRotaAux[0].tempoSaida = vetClienteRotaAux[1].tempoChegada - vetClienteRotaAux[1].tempoPorPeriodo[primeiroPeriodo];
            else
            {
                vetClienteRotaAux[0].tempoSaida = instancia->vetorPeriodos[primeiroPeriodo].fim - vetClienteRotaAux[1].tempoPorPeriodo[primeiroPeriodo];

            }

            ultimoPeriodo = -1;
            quant = 0;

            for (int k = 0; k < numPeriodos; ++k)
            {
                if (vetClienteRotaAux[tamAux - 1].percorrePeriodo[k])
                {
                    ++quant;
                    ultimoPeriodo = k;
                }
            }

            //Calcula tempo de chegada do deposito
            if (quant == 1)
            {
                vetClienteRotaAux[tamAux - 1].tempoChegada = vetClienteRotaAux[tamAux - 2].tempoSaida + vetClienteRotaAux[tamAux - 1].tempoPorPeriodo[ultimoPeriodo];
            } else
            {
                vetClienteRotaAux[tamAux - 1].tempoChegada = instancia->vetorPeriodos[ultimoPeriodo].inicio + vetClienteRotaAux[tamAux - 1].tempoPorPeriodo[ultimoPeriodo];
            }

        }

    }


    //Zera o LB e UB do X entre os clientes dos veiculos
    if(vetClienteRota && vetClienteRota2 && trocaClientesEntreRotas)
    {

        for(int h = 0; h < NumVeiculos; ++h)
        {
            if(h == 0)
            {
                vetClienteRotaAux = vetClienteRota;
                tamAux = *tam;
            }
            else
            {
                vetClienteRotaAux = vetClienteRota2;
                tamAux = *tam2;
            }


            for(int i = 0; i < (tamAux - 1); ++i)
            {

                cliente1 = vetClienteRotaAux[i].cliente;

                for(int j = i + 1; j < (tamAux - 1); ++j)
                {
                    cliente2 = vetClienteRotaAux[j].cliente;


                    if (instancia->matrizDistancias[cliente1][cliente2] != 0.0)
                    {
                        variaveis->X[(h + 1) % 2][cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
                        variaveis->X[(h + 1) % 2][cliente1][cliente2].set(GRB_DoubleAttr_UB, 0);
                        variaveis->X[(h + 1) % 2][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[(h + 1) % 2][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);

                        for (int k = 0; k < instancia->numPeriodos; ++k)
                        {
                            variaveis->x[(h + 1) % 2][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->d[(h + 1) % 2][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->tao[(h + 1) % 2][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);

                        }


                    }

                    if (instancia->matrizDistancias[cliente2][cliente1] != 0.0)
                    {
                        variaveis->X[(h + 1) % 2][cliente2][cliente1].set(GRB_DoubleAttr_LB, 0);
                        variaveis->X[(h + 1) % 2][cliente2][cliente1].set(GRB_DoubleAttr_UB, 0);
                        variaveis->X[(h + 1) % 2][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[(h + 1) % 2][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);

                        for (int k = 0; k < instancia->numPeriodos; ++k)
                        {
                            variaveis->x[(h + 1) % 2][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->d[(h + 1) % 2][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->tao[(h + 1) % 2][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);

                        }
                    }

                }
            }


        }

        for(int i = 0; i < (*tam - 1); ++i)
        {
            cliente1 = vetClienteRota[i].cliente;

            for(int j = 0; j < (*tam2 - 1); ++j)
            {

                cliente2 = vetClienteRota2[j].cliente;

                for(int h = 0; h < NumVeiculos; ++h)
                {
                    if(instancia->matrizDistancias[cliente1][cliente2] != 0)
                    {
                        variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
                        variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 0);
                        variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);

                        for(int k = 0; k < instancia->numPeriodos; ++k)
                        {
                            variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);
                        }
                    }

                    if(instancia->matrizDistancias[cliente2][cliente1] != 0)
                    {
                        variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_LB, 0);
                        variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 0);
                        variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);

                        for(int k = 0; k < instancia->numPeriodos; ++k)
                        {
                            variaveis->x[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->d[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                            variaveis->tao[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                        }

                    }
                }
            }
        }

    }

    if(vetClienteRota)
    {
        pesoAux = 0;

        for(int i = 1; i < (*tam - 1); ++i)
            pesoAux += instancia->vetorClientes[vetClienteRota[i].cliente].demanda;

        *peso = pesoAux;
    }

    if(vetClienteRota2)
    {
        pesoAux = 0;

        for(int i = 1; i < (*tam2 - 1); ++i)
            pesoAux += instancia->vetorClientes[vetClienteRota2[i].cliente].demanda;

        *peso2 = pesoAux;
    }

    modelo->set(GRB_IntParam_SubMIPNodes, GRB_MAXINT);
    modelo->reset(0);

    /* ************************************************************************************************************************************************************ */



    auto c_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempoCpu = c_end-c_start;




    return true;

}

void Modelo::geraRotasOtimas(Solucao::Solucao *solucao, Modelo *modelo, Solucao::ClienteRota *vetClienteRota, const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas,
                             int *vetRotasAux)
{
    Solucao::ClienteRota veiClienteRotaAux[25];

    if(solucao->veiculoFicticil)
        return;


    double poluicao, combustivel;
    int resultado;

    for(auto veiculo : solucao->vetorVeiculos)
    {
        if(veiculo->tipo == 2)
        {
            cout<<"Erro, veiculo ficticio em uma solucao viavel\n";
            cout<<veiculo->getRota()<<'\n';
            delete modelo;
            exit(-1);
        }

        auto vetAux = *veiculo->listaClientes.begin();

        if((veiculo->listaClientes.size() == 2) || vetAux->rotaMip)
            continue;

        //Copia clientes para vetor
        auto itCliente = veiculo->listaClientes.begin();

        for(int i = 0; i < veiculo->listaClientes.size(); ++i)
        {
            vetClienteRota[i].swap(*itCliente);

            ++itCliente;
        }


        bool rotaEncontrada = false;

        if(hashRotas)
            rotaEncontrada = hashRotas->getVeiculo(vetClienteRota, veiculo->listaClientes.size(), veiculo->tipo, &poluicao, &combustivel);

        resultado = false;

        if(!rotaEncontrada)
        {

            auto ptrVeicAux = veiClienteRotaAux;

            for(auto cliente : veiculo->listaClientes)
            {
                ptrVeicAux->swap(cliente);

                ++ptrVeicAux;
            }

            //Cria rota
            try
            {


                combustivel = veiculo->combustivel;
                poluicao = veiculo->poluicao;
                int tam = veiculo->listaClientes.size();
                resultado = modelo->criaRota(vetClienteRota, &tam, veiculo->tipo, &veiculo->carga, instancia, &poluicao, &combustivel, NumTrocas, vetRotasAux,
                                             NULL, NULL, false, 0, NULL, NULL, NULL, false);
            } catch (GRBException e)
            {
                cout << "Erro MIP. \nVeiculo tipo: " << veiculo->tipo << "\nRota: " << veiculo->getRota() << '\n';
                cout << "Erro code: " << e.getErrorCode();
                cout << "Mensagem: " << e.getMessage() << '\n';
                delete modelo;
                exit(-1);
            }

        }
        else
            resultado = true;



        if(((poluicao - 1e-5) < veiculo->poluicao) && resultado || rotaEncontrada)
        {
            vetClienteRota[0].rotaMip = true;
            //Atualiza solucao
            auto it = veiculo->listaClientes.begin();
            int i = 0;

            for(auto it : veiculo->listaClientes)
            {
                it->swap(&vetClienteRota[i]);
                ++i;
            }

            auto cliente = *veiculo->listaClientes.begin();
            cliente->rotaMip = true;

            //Retira poluicao do veiculo e adiciona a nova poluicao
            solucao->poluicao -= veiculo->poluicao;
            solucao->poluicao += poluicao;

            //Atualiza poluicao e combustivel do veiculo
            veiculo->poluicao = poluicao;
            veiculo->combustivel = combustivel;


            if(!rotaEncontrada && hashRotas)
            {
                hashRotas->insereVeiculo(veiClienteRotaAux, vetClienteRota, poluicao, combustivel, veiculo->listaClientes.size(), veiculo->tipo, veiculo->carga);

            }
        }
        else
        {
            if(!rotaEncontrada)
            {
                if (!resultado)
                {
                    cout << "resultado : " << resultado << '\n';
                    cout << "Veiculo tipo: " << veiculo->tipo << '\n';
                    cout << "Rota: " << veiculo->getRota() << "\n";
                } else if (((poluicao - 1e-5) >= veiculo->poluicao))
                    cout << "rota mip eh maior. Mip: " << poluicao << ",  original: " << veiculo->poluicao << '\n';
                else
                    cout << "Rota errada. motivo: ???\n";
            }
        }
    }

    solucao->rotasMip = true;

}
