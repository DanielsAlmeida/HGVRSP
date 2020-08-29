//
// Created by igor on 09/07/2020.
// Erro: restricao_soma_tao_k_, //Restrição 7, restringe o somatorio de tempo do périodo k


#include "Modelo.h"
#include <cstdio>
#include <chrono>
#include "Construtivo.h"
#include "Exception.h"
#include "HashRotas.h"
#include "time.h"

#define NumTrocas 3
#define NumVeiculos 2

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
    modelo->set(GRB_IntParam_CrossoverBasis, 0);
    modelo->set(GRB_IntParam_GomoryPasses, 3);
    modelo->set(GRB_IntParam_Cuts, 3);
    modelo->set(GRB_IntParam_Presolve, 2);
    modelo->set(GRB_DoubleParam_IntFeasTol, 1e-4);
    modelo->set(GRB_DoubleParam_FeasibilityTol, 1e-4);
    modelo->set(GRB_DoubleParam_MIPGap, 0.05);
    modelo->set(GRB_IntParam_MIPFocus, GRB_MIPFOCUS_FEASIBILITY);


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

                        variaveis->d[h][i][j][k] = modelo->addVar(1e-8, GRB_INFINITY, 0, GRB_SEMICONT, "d_" + std::to_string(h) + '_' + std::to_string(i) +
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
    variaveis->l = new GRBVar[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        variaveis->l[i] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "l_" + std::to_string(i));
    }

    //*************************************************************************************************************************************************************

    //Aloca a variavel a: Variavel continua. Indica o tempo de chegada do cliente i
    variaveis->a = new GRBVar[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        variaveis->a[i] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "a_" + std::to_string(i));
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
                    variaveis->f[h][i][j] = modelo->addVar(0, 0, 0, GRB_CONTINUOUS,
                                                        "f_" + std::to_string(h) + '_' + std::to_string(i) + '_' + std::to_string(j));
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

                        GRBVar var = modelo->addVar(-1e-5, 0, 0, GRB_CONTINUOUS,"var_erro_rest_restringe_d_" + std::to_string(h) + '_'  +
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

                    GRBVar varAux = modelo->addVar(-1e-5, 1e-5, 0, GRB_CONTINUOUS, "var_erro_rest_restringe_soma_d_k_" +
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


        for (int k2 = 1; k2 < numPeriodos; ++k2)
        {
            for (int k1 = 0; k1 < k2; ++k1)
            {
                linExpr = 0;

                for(int h = 0; h < NumVeiculos; ++h)
                {
                    for (int l = 0; l < numClientes; ++l)
                    {
                        if (l == j)
                            continue;

                        if (instancia->matrizDistancias[j][l] == 0.0)
                            continue;

                        linExpr += variaveis->x[h][j][l][k1];


                    }
                }

                for(int h = 0; h < NumVeiculos; ++h)
                {
                    for (int i = 0; i < numClientes; ++i)
                    {
                        if (instancia->matrizDistancias[i][j] != 0.0)
                            linExpr += variaveis->x[h][i][j][k2];
                    }
                }

                modelo->addConstr(linExpr <= 1, "subCiclo_"+std::to_string(j) + "_k1_" + std::to_string(k1) + "_k2_" + std::to_string(k2));
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

            modelo->addConstr(linExpr + erro <= val, "restricao_soma_tao_k_" + std::to_string(k));
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
                for (int k = 0; k < instancia->numPeriodos; ++k)
                {

                    linExpr = 0;
                    linExpr = variaveis->l[i];

                    for(int h = 0; h < NumVeiculos; ++h)
                        linExpr += variaveis->tao[h][i][j][k];

                    for(int h = 0; h < NumVeiculos; ++h)
                        linExpr +=  instancia->vetorPeriodos[4].fim *variaveis->x[h][i][j][k];

                    double val = instancia->vetorPeriodos[4].fim + instancia->vetorPeriodos[k].fim;
                    modelo->addConstr(linExpr <= val, "tempoSaida_cliente_"+std::to_string(i)+"_("+std::to_string(i)+"_"+std::to_string(j)+")_P_"+std::to_string(k));

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
                for (int k = 0; k < instancia->numPeriodos; ++k)
                {

                    linExpr = 0;
                    linExpr = variaveis->a[j];

                    for(int h = 0; h < NumVeiculos; ++h)
                        linExpr += -variaveis->tao[h][i][j][k];

                    for(int h = 0; h < NumVeiculos; ++h)
                        linExpr +=   - instancia->vetorPeriodos[4].fim *variaveis->x[h][i][j][k];

                    double val = + instancia->vetorPeriodos[k].inicio - instancia->vetorPeriodos[4].fim;

                    modelo->addConstr(linExpr >= val, "tempoChegada_"+std::to_string(j)+"_("+std::to_string(i)+"_"+std::to_string(j)+")_P_"+std::to_string(k));


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
                linExpr = 0;
                linExpr = variaveis->a[j] - variaveis->l[i];

                for(int h = 0; h < NumVeiculos; ++h)
                    linExpr += -instancia->vetorPeriodos[4].fim *variaveis->X[h][i][j];

                for(int h = 0; h < NumVeiculos; ++h)
                {
                    for (int k = 0; k < numPeriodos; ++k)
                    {

                        linExpr += -variaveis->tao[h][i][j][k];
                    }
                }

                modelo->addConstr(linExpr >= -instancia->vetorPeriodos[4].fim, "tempoChegada_"+std::to_string(j)+"_Soma_k_("+std::to_string(i)+"_"+std::to_string(j)+")");
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
        GRBVar varAux = modelo->addVar(-5e-3, 0.0, 0, GRB_CONTINUOUS,"var_erro_rest_fim_janela_"+std::to_string(i));

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
            GRBVar var = modelo->addVar(-1e-3, 0, 0, GRB_CONTINUOUS,"Var_erro_restr_limitaCombustivelTipo_" + std::to_string(t));


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
                modelo->addConstr(linExpr <= val + temp * variaveis->T[h][0], "r_0_limita_tempo_saida_veiculo_0_e_1_0_Periodo_" + std::to_string(k));

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

                        linExpr = -variaveis->a[j] + variaveis->tao[h][0][j][k] + instancia->vetorPeriodos[4].fim * variaveis->x[h][0][j][k];
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
                        linExpr = -variaveis->l[i] - variaveis->tao[h][i][0][k] - instancia->vetorPeriodos[4].fim * variaveis->x[h][i][0][k];

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

    for(int j = 0; j < numClientes; ++j)
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

    variaveis->restricaoUmArcoJ[0].set(GRB_DoubleAttr_RHS, 1);

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

    for(int i = 0; i < numClientes; ++i)
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

    variaveis->restricaoUmArcoI[0].set(GRB_DoubleAttr_RHS, 1);
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

int Modelo::Modelo::criaRota(Solucao::ClienteRota *vetClienteRota, const int tam, bool tipo, int peso,
                             const Instancia::Instancia *instancia, double *poluicao, double *combustivel,
                             const int numArcos, int *vetRotaAux, Solucao::ClienteRota *vetClienteRota2, const int tam2,
                             const bool tipo2, const int peso2, double *poluicao2, double *combustivel2,
                             int *vetRotaAux2)
{


    auto c_start = std::chrono::high_resolution_clock::now();

    static int rota[20];

    for (int i = 0; i < tam; ++i)
    {
        rota[i] = vetRotaAux[i] = vetClienteRota[i].cliente;

    }

    for(int i = 0; i < tam2; ++i)
        vetRotaAux2[i] = vetClienteRota2[i].cliente;

    if ((tipo != this->tipoVeiculo0) || (tipo2 != this->tipoVeiculo1))
    {

        this->tipoVeiculo0 = tipo;
        this->tipoVeiculo1 = tipo2;

        modelo->setObjective(variaveis->funcaoObjetivo[tipo] + variaveis->funcaoObjetivo[tipo2], GRB_MINIMIZE);


        if (tipo)
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
            vetClienteAux = vetClienteRota;
            tamAux = tam;

            combustivelAux = *combustivel;
            poluicaoAux = *poluicao;
            pesoAux = peso;
        }
        else
        {
            if(!vetClienteRota2)
                break;

            vetClienteAux = vetClienteRota2;
            tamAux = tam2;

            combustivelAux = *combustivel2;
            poluicaoAux = * poluicao2;
            pesoAux = peso2;
        }

        //Inicio
        if (viavel)
        {


            variaveis->C[h][tipo].set(GRB_DoubleAttr_Start, combustivelAux);

            for (int i = 1; i < tam - 1; ++i)
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
                    vetClienteAux[i - 1].tempoSaida = vetClienteAux[i - 1].tempoSaida +
                                                       (instancia->vetorClientes[vetClienteAux[i].cliente].inicioJanela -
                                                               vetClienteAux[i].tempoChegada);
                }
            }
        }

        for (int i = 0; i < tam - 1; ++i)
        {
            cliente1 = vetClienteAux[i].cliente;

            for (int j = i + 1; j < tam; ++j)
            {
                //Percorre todos os pares i,j da rota

                cliente2 = vetClienteAux[j].cliente;

                //Verifica se existe o arco i,j
                if (instancia->matrizDistancias[cliente1][cliente2] != 0.0)
                {
                    //Muda os limites superiores de X e f. Destrava x e f
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 1);
                    variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, GRB_INFINITY);

                    if (viavel)
                    {
                        //Velor inicial é 0
                        variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 0);

                        //Inicializa x

                        for (int k = 0; k < instancia->numPeriodos; ++k)
                            variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, 0);

                    }
                    //modelo->chgCoeff(variaveis->restricaoTrocaClientes, variaveis->X[cliente1][cliente2], 1);
                }

                //Verifica se existe o arco j,i
                if (instancia->matrizDistancias[cliente2][cliente1] != 0.0)
                {
                    //Muda os limites superiores de X e f. Destrava x e f
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 1);
                    variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, GRB_INFINITY);

                    if (viavel)
                    {
                        //Velor inicial é 0
                        variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);
                        variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, 0);

                        for (int k = 0; k < instancia->numPeriodos; ++k)
                            variaveis->x[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, 0);
                    }

                    //modelo->chgCoeff(variaveis->restricaoTrocaClientes, variaveis->X[cliente2][cliente1], 1);
                }
            }
        }

        for (int i = 1; i < tam; ++i)
        {
            //Percorre as arestas da rota

            cliente1 = vetClienteAux[i - 1].cliente;
            cliente2 = vetClienteAux[i].cliente;

            //Inicializa x
            if (viavel)
                variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, 1);

            //inicializa peso
            if (viavel)
                variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, peso);

            modelo->chgCoeff(variaveis->restricaoTrocaClientes[h], variaveis->X[h][cliente1][cliente2], 1);


            if (viavel)
            {
                //Inicializa tempo de saida do cliente1 e tempo de chegada do cliente2
                variaveis->l[cliente1].set(GRB_DoubleAttr_Start, vetClienteAux[i - 1].tempoSaida);
                variaveis->a[cliente2].set(GRB_DoubleAttr_Start, vetClienteAux[i].tempoChegada);

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

            peso -= instancia->vetorClientes[cliente2].demanda;

            //Muda a restricao de peso do cliente2 para a demanda do cliente2 vetClienteAux

            if (cliente2 != 0)
            {
                if(h == 0)
                    variaveis->restricaoPeso_veic0[cliente2].set(GRB_DoubleAttr_RHS, instancia->vetorClientes[cliente2].demanda);
                else
                    variaveis->restricaoPeso_veic1[cliente2].set(GRB_DoubleAttr_RHS, instancia->vetorClientes[cliente2].demanda);
            }

        }

        for (int i = 1; i < tam - 1; ++i)
        {
            //Muda a restricao do somatorio de X para 1
            variaveis->restricaoUmArcoJ[vetClienteAux[i].cliente].set(GRB_DoubleAttr_RHS, 1);
            variaveis->restricaoUmArcoI[vetClienteAux[i].cliente].set(GRB_DoubleAttr_RHS, 1);
        }

        //tam - 1 => numero de arcos da rota. numArcos => numero de arcos que podem ser trocados
        int num = (tam - 1) - numArcos;

        num = ( num < 0 ? 0 : num);


        //Muda o lado direito da restricao de trocar clientes para num. num -> numero de arcos que  nao podem ser trocados
        variaveis->restricaoTrocaClientes[h].set(GRB_DoubleAttr_RHS, num);
    }

    /* **************************************************************************************************************************************/

    double ultimaPoluicao = *poluicao;
    double ultimaPoluicao2;

    if(vetClienteRota2)
        ultimaPoluicao2 = *poluicao2;

    int numInteracoes = 0;

    int aux;

    if(numArcos == 0)
        aux = 1;
    else
        aux = 3;

    const int MaxNumInteracoes = 3;

    for(int p = 0; p < MaxNumInteracoes; ++p)
    {
        ++numInteracoes;

        modelo->update();
        modelo->optimize();

        bool resultado = (modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL);

        if (!resultado)
            return 0;


        *combustivel = variaveis->C[0][tipo].get(GRB_DoubleAttr_X);
        *poluicao = *combustivel * instancia->vetorVeiculos[tipo].cVeiculo;

        if(vetClienteRota2)
        {
            *combustivel2 = variaveis->C[1][tipo2].get(GRB_DoubleAttr_X);
            *poluicao2 = *combustivel2 * instancia->vetorVeiculos[tipo2].cVeiculo;
        }


        //Calcula o gap
        double gap1 = (*poluicao - ultimaPoluicao)/ultimaPoluicao;

        double gap2;

        if(vetClienteRota2)
        {
            gap2 = (*poluicao2 - ultimaPoluicao2) / ultimaPoluicao2;
            ultimaPoluicao2 = *poluicao2;
        }

        ultimaPoluicao = *poluicao;

        //Zera os coeficientes da restricao de troca clientes

        Solucao::ClienteRota *vetClienteAux;
        int tamAux;

        for(int h = 0; h < NumVeiculos; ++h)
        {
            if(h == 0)
            {
                vetClienteAux = vetClienteRota;
                tamAux = tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteAux = vetClienteRota2;
                tamAux = tam2;
            }

            for (int i = 0; i < tamAux - 1; ++i)
            {
                cliente1 = vetClienteAux[i].cliente;
                cliente2 = vetClienteAux[i + 1].cliente;

                modelo->chgCoeff(variaveis->restricaoTrocaClientes[h], variaveis->X[h][cliente1][cliente2], 0.0);
            }
        }
        //Recuperar a sequencia de clientes

        for(int h = 0; h < NumVeiculos; ++h)
        {

            if(h == 0)
            {
                vetClienteAux = vetClienteRota;
                tamAux = tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteAux = vetClienteRota2;
                tamAux = tam2;
            }

            cliente1 = 0;

            for (int i = 0; i < (tamAux - 1); ++i)
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

                    for (int i = 0; i < tam; ++i)
                        cout << rota[i] << ' ';
                    cout << '\n';

                    exit(-1);
                }

                //Atualiza vetor e cliente1
                vetClienteAux[i + 1].cliente = cliente2;
                cliente1 = cliente2;

            }
        }


        if((gap1 > -.04) && (gap2 > -.04))
            break;

        //Verifica se existe uma proxima interaca
        if((p + 1) < MaxNumInteracoes)
        {
            bool iguais = true;
            bool iguais2 = true;

            //Verifica se a seguencia é igual a da interacao anterior
            for(int i = 0; i < tam; ++i)
            {
                if(vetClienteRota[i].cliente != vetRotaAux[i])
                {
                    iguais = false;
                    break;
                }
            }

            for(int i = 0; i < tam2; ++i)
            {
                if(vetClienteRota2[i].cliente != vetRotaAux2[i])
                {
                    iguais2 = false;
                    break;
                }

            }

            if(iguais && iguais2)
                break;

            for(int i = 0; i < tam; ++i)
            {
                vetRotaAux[i] = vetClienteRota[i].cliente;
            }

            for(int i = 0; i < tam2; ++i)
                vetRotaAux2[i] = vetClienteRota2[i].cliente;

            for(int h = 0; h < NumVeiculos; ++h)
            {
                if(h==0)
                {
                    vetClienteAux = vetClienteRota;
                    tamAux = tam;
                }
                else
                {
                    if(!vetClienteRota2)
                        break;

                    vetClienteAux = vetClienteRota2;
                    tamAux = tam2;
                }

                for (int i = 0; i < tam2 - 1; ++i)
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
            vetClienteRotaAux = vetClienteRota;
            tamAux = tam;
        }
        else
        {
            if(!vetClienteRota2)
                break;

            vetClienteRotaAux = vetClienteRota2;
            tamAux = tam2;
        }

        for (int i = 1; i < tamAux - 1; ++i)
        {
            //Volta o lodo direito de X para 0
            variaveis->restricaoUmArcoJ[vetClienteRotaAux[i].cliente].set(GRB_DoubleAttr_RHS, 0);
            variaveis->restricaoUmArcoI[vetClienteRotaAux[i].cliente].set(GRB_DoubleAttr_RHS, 0);
        }
    }

    for(int h = 0; h < NumVeiculos; ++h)
    {
        if(h == 0)
        {
            vetClienteRotaAux = vetClienteRota;
            tamAux = tam;
        }
        else
        {
            if(!vetClienteRota2)
                break;

            vetClienteRotaAux = vetClienteRota2;
            tamAux = tam2;
        }

        for (int i = 0; i < tamAux - 1; ++i)
        {
            cliente1 = vetClienteRotaAux[i].cliente;

            for (int j = i + 1; j < tam; ++j)
            {
                //Percorre todos os pares de i,j
                cliente2 = vetClienteRotaAux[j].cliente;


                if (instancia->matrizDistancias[cliente1][cliente2] != 0.0)
                {
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 0);
                    variaveis->X[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

                    variaveis->f[h][cliente1][cliente2].set(GRB_DoubleAttr_UB, 0);

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {
                        variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                        variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                        variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    }

                }

                if (instancia->matrizDistancias[cliente2][cliente1] != 0.0)
                {
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_LB, 0);
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 0);
                    variaveis->X[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

                    variaveis->f[h][cliente2][cliente1].set(GRB_DoubleAttr_UB, 0);

                    for (int k = 0; k < instancia->numPeriodos; ++k)
                    {
                        variaveis->x[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                        variaveis->d[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                        variaveis->tao[h][cliente2][cliente1][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    }


                }
            }
        }

    }

    //Muda o lado direito da restricao de peso para 0
    for (int i = 1; i < tam - 1; ++i)
        variaveis->restricaoPeso_veic0[vetClienteRota[i].cliente].set(GRB_DoubleAttr_RHS, 0);

    for (int i = 1; i < tam2 - 1; ++i)
        variaveis->restricaoPeso_veic1[vetClienteRota2[i].cliente].set(GRB_DoubleAttr_RHS, 0);



    bool resultado = modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL;


    variaveis->C[0][tipo].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

    if(vetClienteRota2)
        variaveis->C[1][tipo2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

    if (viavel)
    {
        for(int h = 0; h < NumVeiculos; ++h)
        {

            if(h == 0)
            {
                vetClienteRotaAux = vetClienteRota;
                tamAux = tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteRotaAux = vetClienteRota2;
                tamAux = tam2;
            }

            for (int i = 0; i < tamAux - 1; ++i)
            {
                cliente1 = vetClienteRotaAux[i].cliente;
                cliente2 = vetClienteRotaAux[i + 1].cliente;

                //Muda o valor inicial para indefinido

                variaveis->l[cliente1].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                variaveis->a[cliente2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

                for (int k = 0; k < instancia->numPeriodos; ++k)
                {

                    variaveis->x[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    variaveis->tao[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                    variaveis->d[h][cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                }


            }
        }
    }




    if (modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
    {
        for(int h = 0; h < NumVeiculos; ++h)
        {

            if(h == 0)
            {
                vetClienteRotaAux = vetClienteRota;
                tamAux = tam;
            }
            else
            {
                if(!vetClienteRota2)
                    break;

                vetClienteRotaAux = vetClienteRota2;
                tamAux = tam2;
            }

            cliente1 = 0;

            for (int i = 0; i < tamAux; ++i)
            {


                //recupera tempo de chegada e saida do cliente 1
                if (i != 0 && i != tamAux - 1)
                    vetClienteRotaAux[i].tempoChegada = variaveis->a[cliente1].get(GRB_DoubleAttr_X);

                vetClienteRotaAux[i].tempoSaida = variaveis->l[cliente1].get(GRB_DoubleAttr_X);

                if (i == (tamAux - 1))
                    break;

                cliente2 = vetClienteRotaAux[i + 1].cliente;

                for (int k = 0; k < numPeriodos; ++k)
                {
                    //Recupera tempo e distancia parcial

                    vetClienteRotaAux[i + 1].tempoPorPeriodo[k] = variaveis->tao[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X);
                    vetClienteRotaAux[i + 1].distanciaPorPeriodo[k] = variaveis->d[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X);
                    double l = variaveis->d[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X);

                    bool x;
                    /*if(l < 1e-8)
                        x = false;
                    else*/

                    x = bool(variaveis->x[h][cliente1][cliente2][k].get(GRB_DoubleAttr_X));

                    vetClienteRotaAux[i + 1].percorrePeriodo[k] = x;

                }

                cliente1 = cliente2;


            }

            //Verifica se ultima psicao é o deposito
            if (vetClienteRotaAux[tam - 1].cliente != 0)
            {
                cout
                        << "ERRO, \nArquivo: modelo.cpp\nFuncao: criaRota\nMotivo: Ultima possicao diferente do deposito\n";
                cout << "tipo: " << tipo << '\n';
                cout << "Rota: ";

                for (int i = 0; i < tam; ++i)
                    cout << rota[i] << ' ';
                cout << '\n';

                cout << "Rota gerada: ";
                for (int i = 0; i < tam; ++i)
                    cout << vetClienteRota[i].cliente << ' ';
                cout << '\n';

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
                vetClienteRotaAux[tam - 1].tempoChegada = instancia->vetorPeriodos[ultimoPeriodo].inicio + vetClienteRotaAux[tamAux - 1].tempoPorPeriodo[ultimoPeriodo];
            }

        }

    }


    modelo->set(GRB_IntParam_SubMIPNodes, GRB_MAXINT);

    /* ************************************************************************************************************************************************************ */


    auto c_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempoCpu = c_end-c_start;

    cout<<"Tempo cpu: "<<tempoCpu.count()<<'\n';


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

                resultado = modelo->criaRota(vetClienteRota, veiculo->listaClientes.size(), veiculo->tipo,
                                             veiculo->carga, instancia, &poluicao, &combustivel, NumTrocas, vetRotasAux,
                                             nullptr, 0, 0, 0, nullptr, nullptr, nullptr);
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