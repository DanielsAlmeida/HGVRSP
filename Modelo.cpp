//
// Created by igor on 09/07/2020.
// Erro: restricao_soma_tao_k_, //Restrição 7, restringe o somatorio de tempo do périodo k


#include "Modelo.h"
#include <cstdio>
#include "Construtivo.h"
#include "Exception.h"
#include "HashRotas.h"

ExceptioViabilidadeMip exceptionViabilidadeMip;

Modelo::Modelo::Modelo(Instancia::Instancia *instancia, GRBModel *grbModel, const bool usaModeloVnd_)
        : numClientes(instancia->numClientes), numVeiculos(instancia->numVeiculos), numPeriodos(instancia->numPeriodos),
                      modelo(grbModel), usaModeloVnd(usaModeloVnd_)
{

    const int NumVeic = 2;

    //Cria o modelo
    tipoVeiculo = false;
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


    //Cria variaveis

    //*****************************************************************************************************************************************

    //Aloca variavel X
    variaveis->X = new GRBVar *[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
        variaveis->X[i] = new GRBVar[instancia->numClientes];


    //Cria veriaveis X_(i,j) no modelo
    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
                variaveis->X[i][j] = modelo->addVar(0, 0, 0, GRB_BINARY,
                                                    "X_" + std::to_string(i) + '_' + std::to_string(j));
        }
    }

    //*****************************************************************************************************************************************


    //Aloca variavel x: Variavel binária. Indica se um arco (i,j) é percorrido em um período k pelo veículo h
    variaveis->x = new GRBVar **[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
        variaveis->x[i] = new GRBVar *[instancia->numClientes];


    for (int i = 0; i < instancia->numClientes; ++i)
    {

        for (int j = 0; j < instancia->numClientes; ++j)
        {
            variaveis->x[i][j] = new GRBVar[instancia->numPeriodos];
        }

    }

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
            {

                for (int k = 0; k < instancia->numPeriodos; ++k)
                {

                    variaveis->x[i][j][k] = modelo->addVar(0, 1, 0, GRB_BINARY, "x_" + std::to_string(i) + '_' +
                                                                                std::to_string(j) + '_' +
                                                                                std::to_string(k));

                }
            }
        }

    }

    //*****************************************************************************************************************************************
    //Cria a variável T


    variaveis->T = new GRBVar[2];


    variaveis->T[0] = modelo->addVar(0, 0, 0, GRB_BINARY, "T_"+std::to_string(0));
    variaveis->T[1] = modelo->addVar(1, 1, 0, GRB_BINARY, "T_"+std::to_string(1));

    //*****************************************************************************************************************************************

    //Aloca variavel d: Variavel continua. Indica a distância percorrida do arco (i,j) no período k pelo veículo h

    variaveis->d = new GRBVar **[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        variaveis->d[i] = new GRBVar *[instancia->numClientes];
    }

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
            variaveis->d[i][j] = new GRBVar[instancia->numPeriodos];

    }

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
            {

                for (int k = 0; k < instancia->numPeriodos; ++k)
                {

                    variaveis->d[i][j][k] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS,
                                                           "d_" + std::to_string(i) + '_' +
                                                           std::to_string(j) + '_' + std::to_string(k));

                }
            }

        }
    }

    //*************************************************************************************************************************************************************

    //Aloca a variavel tal: Variavel continua. Indica o tempo gasto no arco (i,j) no período k pelo veículo h

    variaveis->tao = new GRBVar **[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        variaveis->tao[i] = new GRBVar *[instancia->numClientes];
    }

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
            variaveis->tao[i][j] = new GRBVar[instancia->numPeriodos];

    }

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
            {
                for (int k = 0; k < instancia->numPeriodos; ++k)
                {

                    variaveis->tao[i][j][k] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS,
                                                             "tao_" + std::to_string(i)
                                                             + '_' + std::to_string(j) + '_' + std::to_string(k));

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
    variaveis->f = new GRBVar *[instancia->numClientes];

    for (int i = 0; i < instancia->numClientes; ++i)
        variaveis->f[i] = new GRBVar[instancia->numClientes];


    for (int i = 0; i < instancia->numClientes; ++i)
    {
        for (int j = 0; j < instancia->numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
                variaveis->f[i][j] = modelo->addVar(0, 0, 0, GRB_CONTINUOUS,
                                                    "f_" + std::to_string(i) + '_' + std::to_string(j));
        }
    }

    //*************************************************************************************************************************************************************
    //Cria a variavel C

    variaveis->C = new GRBVar[2];

    variaveis->C[0] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "C_0");
    variaveis->C[1] = modelo->addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "C_1");

    //*************************************************************************************************************************************************************
    //Cria a funcao objetivo
    variaveis->funcaoObjetivo = new GRBLinExpr[2];

    //*************************************************************************************************************************************************************
    //*************************************************************************************************************************************************************
    //ok
    //Adiciona as restrições

    //Restrição 1. Limita a distancia da variavel d. d[i,j,k] <= D [i,j] * x [i,j,k]

    GRBLinExpr linExprDist_d = 0;



    for (int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
            {

                for (int k = 0; k < numPeriodos; ++k)
                {
                    linExprDist_d = 0;
                    linExprDist_d = variaveis->d[i][j][k] - instancia->matrizDistancias[i][j] * variaveis->x[i][j][k];

                    GRBVar var = modelo->addVar(-1e-5, 0, 0, GRB_CONTINUOUS, "var_erro_rest_restringe_d_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k));

                    modelo->addConstr(linExprDist_d + var <= 0.0, "restringe_d_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k));

                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Cria restrição 2, Zera a variavel x se (X = 0)
    //ok
    GRBLinExpr linExpr_limitaVar_x_por_X = 0;



    for (int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0.0)
            {

                for (int k = 0; k < numPeriodos; ++k)
                {
                    linExpr_limitaVar_x_por_X = 0;
                    linExpr_limitaVar_x_por_X = variaveis->X[i][j] - variaveis->x[i][j][k];

                    modelo->addConstr(linExpr_limitaVar_x_por_X >= 0.0,
                                      "restrige_x_por_X_" + std::to_string(i) + "_" + std::to_string(j) + "_" +
                                      std::to_string(k));

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
    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j < numClientes; ++j)
        {

            if(instancia->matrizDistancias[i][j] != 0.0)
            {
                linExpr = 0;

                for(int k = 0; k < numPeriodos; ++k)
                    linExpr += variaveis->d[i][j][k];

                GRBVar varAux = modelo->addVar(-1e-5, 1e-5, 0, GRB_CONTINUOUS, "var_erro_rest_restringe_soma_d_k_"+std::to_string(i)+"_"+std::to_string(j));

                linExpr += -instancia->matrizDistancias[i][j] * variaveis->X[i][j] + varAux;
                modelo->addConstr(linExpr == 0, "restringe_soma_d_k_"+std::to_string(i)+"_"+std::to_string(j));


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
                for (int l = 0; l < numClientes; ++l)
                {
                    if (l == j)
                        continue;

                    if (instancia->matrizDistancias[j][l] == 0.0)
                        continue;

                    linExpr += variaveis->x[j][l][k1];


                }

                for(int i = 0; i < numClientes; ++i)
                {
                    if(instancia->matrizDistancias[i][j] != 0.0)
                        linExpr += variaveis->x[i][j][k2];
                }

                modelo->addConstr(linExpr <= 1, "subCiclo_"+std::to_string(j) + "_k1_" + std::to_string(k1) + "_k2_" + std::to_string(k2));
            }
        }




    }


    /*for (int j = 1; j < numClientes; ++j)
    {
        for(int i = 0; i < numClientes; ++i)
        {
            if(instancia->matrizDistancias[i][j] == 0.0)
                continue;

            for (int l = 0; l < numClientes; ++l)
            {
                if((l == i) || (l == j))
                    continue;

                if(instancia->matrizDistancias[j][l] == 0.0)
                    continue;



                for (int k1 = 0; k1 < numPeriodos; ++k1)
                {
                    for (int k2 = 0; k2 < numPeriodos; ++k2)
                    {

                        if(k1 >= k2)
                            continue;

                        modelo->addConstr(variaveis->x[j][l][k1] + variaveis->x[i][j][k2]<= 1 , "subCiclo_("+std::to_string(i)+"_"+std::to_string(j) + ")_("+std::to_string(j) + "_"+std::to_string(l)+")_"+std::to_string(k1)+"_"+std::to_string(k2));

                    }


                }
            }

        }


    }*/

    //*************************************************************************************************************************************************************
    //Calcula o tempo gasto ao percorrer o arco i,j no periodo k
    //ok
    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j < numClientes; ++j)
        {
            if(instancia->matrizDistancias[i][j] != 0.0)
            {
                for(int k = 0; k < numPeriodos; ++k)
                {
                    linExpr = 0;
                    linExpr = instancia->matrizVelocidade[i][j][k] * variaveis->tao[i][j][k] -  variaveis->d[i][j][k];

                    modelo->addConstr(linExpr == 0, "tempo_"+std::to_string(i)+"_"+std::to_string(j)+"_"+std::to_string(k));
                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Restrição 7, restringe o somatorio de tempo do périodo k
    //ok
    for(int k = 0; k < numPeriodos; ++k)
    {
        linExpr = 0;

        for(int i = 0; i < numClientes; ++i)
        {
            for(int j = 0; j < numClientes; ++j)
            {
                if(instancia->matrizDistancias[i][j] != 0.0)
                    linExpr += variaveis->tao[i][j][k];
            }
        }

        double val = (instancia->vetorPeriodos[0].fim - instancia->vetorPeriodos[0].inicio);

        GRBVar erro = modelo->addVar(-5e-3, 0, 0, GRB_CONTINUOUS, "var_erro_restricao_soma_tao_k_"+std::to_string(k));

        modelo->addConstr(linExpr + erro <= val, "restricao_soma_tao_k_"+std::to_string(k));
    }

    //*************************************************************************************************************************************************************
    // restrição 8, tempo de saida do cliente i, em um arco (i,j) no périodo k
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
                    linExpr = variaveis->l[i] + variaveis->tao[i][j][k];
                    linExpr +=  instancia->vetorPeriodos[4].fim *variaveis->x[i][j][k];

                    double val = instancia->vetorPeriodos[4].fim + instancia->vetorPeriodos[k].fim;
                    modelo->addConstr(linExpr <= val, "tempoSaida_cliente_"+std::to_string(i)+"_("+std::to_string(i)+"_"+std::to_string(j)+")_P_"+std::to_string(k));

                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Tempo de chegada - 9
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
                    linExpr = variaveis->a[j]  - variaveis->tao[i][j][k];
                    linExpr +=   - instancia->vetorPeriodos[4].fim *variaveis->x[i][j][k];

                    double val = + instancia->vetorPeriodos[k].inicio - instancia->vetorPeriodos[4].fim;

                    modelo->addConstr(linExpr >= val, "tempoChegada_"+std::to_string(j)+"_("+std::to_string(i)+"_"+std::to_string(j)+")_P_"+std::to_string(k));


                }
            }
        }
    }

    //*************************************************************************************************************************************************************
    //Tempo chegada de j  - 10
    //ok
    for(int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            if (instancia->matrizDistancias[i][j] != 0)
            {
                linExpr = 0;
                linExpr = variaveis->a[j] - variaveis->l[i]  - instancia->vetorPeriodos[4].fim *variaveis->X[i][j];

                for(int k = 0; k < numPeriodos; ++k)
                {

                    linExpr += -variaveis->tao[i][j][k];
                }

                modelo->addConstr(linExpr >= -instancia->vetorPeriodos[4].fim, "tempoChegada_"+std::to_string(j)+"_Soma_k_("+std::to_string(i)+"_"+std::to_string(j)+")");
            }
        }

    }

    //*************************************************************************************************************************************************************
    //Restrige tempo de saida de i _ 11
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

    modelo->addConstr(variaveis->l[0] >= variaveis->T[0]*0.5);

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

    for(int h = inicio; h < 2; ++h)
    {

        linExpr = 0;

        for(int i = 0; i < numClientes; ++i)
        {
            for(int j = 0; j < numClientes; ++j)
            {
                if(instancia->matrizDistancias[i][j] != 0)
                {

                    for(int k = 0; k < numPeriodos; ++k)
                        linExpr += instancia->matrizCo2[i][j][k][h] * variaveis->d[i][j][k];
                }
            }
        }

        for(int i = 0; i < numClientes; ++i)
        {
            for (int j = 0; j < numClientes; ++j)
            {
                if (instancia->matrizDistancias[i][j] != 0)
                    linExpr += instancia->vetorVeiculos[h].pVeiculo * instancia->matrizDistancias[i][j] * variaveis->f[i][j];
            }

        }

        linExpr += -variaveis->C[h];

        modelo->addConstr(linExpr == 0, "combustivelVeiculoTipo_"+std::to_string(h));

        linExprAux = 0;


        linExprAux = instancia->vetorVeiculos[h].combustivel - variaveis->T[h] *instancia->vetorVeiculos[h].combustivel  + variaveis->T[h]*(2*instancia->vetorVeiculos[1].combustivel);
        GRBVar var = modelo->addVar(-1e-3, 0, 0, GRB_CONTINUOUS, "Var_erro_restr_limitaCombustivelTipo_"+std::to_string(h));


        modelo->addConstr(variaveis->C[h] + var <= linExprAux, "limitaCombustivelTipo_"+std::to_string(h));

    }

    modelo->addConstr(variaveis->T[0] + variaveis->T[1] == 1, "limitaTipoVeiculo");


    //*************************************************************************************************************************************************************

    //ok

    //-19

    for (int u = inicio; u < 1; ++u)
    {



                for (int k = 0; k < numPeriodos; ++k)
                {
                    linExpr = 0;
                    double temp = 0.0;//instancia->vetorPeriodos[4].fim;

                    for (int j = 1; j < numClientes; ++j)
                    {
                        if (instancia->matrizDistancias[0][j] != 0)
                            linExpr += variaveis->tao[0][j][k] + instancia->vetorPeriodos[4].fim * variaveis->x[0][j][k];
                    }

                    double val = instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].inicioJanela + instancia->vetorPeriodos[k].fim;



                    temp = -0.5;
                    modelo->addConstr(linExpr <= val + temp*variaveis->T[0], "r_0_limita_tempo_saida_veiculo_0_e_1_0_Periodo_" + std::to_string(k));

                }

    }


    //*************************************************************************************************************************************************************
    //-20
    //ok
    for(int u = inicio; u < 1; ++u)
    {
        for(int j = 1; j < numClientes; ++j)
        {
            if(instancia->matrizDistancias[0][j] != 0)
                for(int k = 0; k < numPeriodos; ++k)
                {
                    linExpr = 0;

                    linExpr = -variaveis->a[j] + variaveis->tao[0][j][k] + instancia->vetorPeriodos[4].fim *variaveis->x[0][j][k];
                    double val = instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].inicioJanela;

                    /*double temp = 0;//instancia->vetorPeriodos[4].fim;
                    if(u == 1)
                        temp = 0.5;

                    modelo->addConstr(linExpr <= val + temp * variaveis->T[u], "r_1_limita_tempo_saida_veiculo_"+std::to_string(u)+"_0_"+std::to_string(j)+"_"+std::to_string(k));
                     */
                    double temp = -0.5;//instancia->vetorPeriodos[4].fim;


                    modelo->addConstr(linExpr <= val + temp * variaveis->T[0], "r_1_limita_tempo_saida_veiculo_0_e_1__0_"+std::to_string(j)+"_"+std::to_string(k));
                }
        }
    }

    //*************************************************************************************************************************************************************
    //ok
    //-21
    for(int u = inicio; u < 1; ++u)
    {

                for(int k = 0; k < numPeriodos; ++k)
                {
                    linExpr = 0;
                    for(int i = 1; i < numClientes; ++i)
                    {
                        if (instancia->matrizDistancias[i][0] != 0)
                            linExpr += -variaveis->tao[i][0][k] - instancia->vetorPeriodos[4].fim * variaveis->x[i][0][k];
                    }
                    double val = -instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].fimJanela + instancia->vetorPeriodos[k].inicio;

                    modelo->addConstr(linExpr >= val, "r_2_limita_tempo_chegada_veiculo_"+std::to_string(u)+"_Periodo_"+std::to_string(k));
                }

    }

    //*************************************************************************************************************************************************************

    //-22

    for(int u = inicio; u < 1; ++u)
    {
        for(int i = 1; i < numClientes; ++i)
        {
            if(instancia->matrizDistancias[i][0] != 0)
                for(int k = 0; k < numPeriodos; ++k)
                {
                    linExpr = 0;
                    linExpr = -variaveis->l[i] - variaveis->tao[i][0][k]   - instancia->vetorPeriodos[4].fim *variaveis->x[i][0][k];

                    double val = -instancia->vetorPeriodos[4].fim - instancia->vetorVeiculos[u].fimJanela;


                    modelo->addConstr(linExpr >= val, "r_3_limita_tempo_chegada_veiculo_"+std::to_string(u)+"_"+std::to_string(i)+"_0_"+std::to_string(k));
                }
        }
    }

    //*************************************************************************************************************************************************************
    //Limita tempo de chegada, -18


    modelo->addConstr(variaveis->a[0] - instancia->vetorVeiculos[1].fimJanela*variaveis->T[0] -  instancia->vetorVeiculos[0].fimJanela * variaveis->T[1] <= 0, "Restringe_tempo_chegada_deposito_veic");


    //*************************************************************************************************************************************************************
    //Cria a função objetivo

    for(int u = 0; u < 2; ++u)
    {
        variaveis->funcaoObjetivo[u] = variaveis->C[u] * instancia->vetorVeiculos[u].cVeiculo;
    }

    modelo->setObjective(variaveis->funcaoObjetivo[0], GRB_MINIMIZE);
    tipoVeiculo = false;


}
Modelo::Modelo::~Modelo()
{


    //deleteta variaveis

    //*****************************************************************************************************************************************

    //Deleteta variavel X

    for(int i = 0; i < numClientes; ++i)
        delete []variaveis->X[i];

    delete []variaveis->X;


    //*****************************************************************************************************************************************

    //Deleta variavel x

    for (int i = 0; i < numClientes; ++i)
    {

        for (int j = 0; j < numClientes; ++j)
        {

            delete []variaveis->x[i][j];
        }

    }

    for (int i = 0; i < numClientes; ++i)
    {

        delete []variaveis->x[i];

    }

    delete []variaveis->x;



    //*****************************************************************************************************************************************

    //Deletando variavel d

    for (int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            delete []variaveis->d[i][j];

        }
    }

    for (int i = 0; i < numClientes; ++i)
    {
        delete []variaveis->d[i];

    }

    delete []variaveis->d;



    //*****************************************************************************************************************************************

    //Deleta variavel tao


    for (int i = 0; i < numClientes; ++i)
    {
        for (int j = 0; j < numClientes; ++j)
        {
            delete []variaveis->tao[i][j];
        }
    }

    for (int i = 0; i < numClientes; ++i)
    {
        delete []variaveis->tao[i];
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

    for(int i = 0; i < numClientes; ++i)
    {
        delete []variaveis->f[i];
    }

    delete [] variaveis->f;

    //*****************************************************************************************************************************************
    //Deleta variavel C

    delete []variaveis->C;

    //*****************************************************************************************************************************************
    //Deleta variavel T

    delete []variaveis->T;

    //*****************************************************************************************************************************************
    //Deleta a funçao objetivo

    delete []variaveis->funcaoObjetivo;

    //*****************************************************************************************************************************************

    modelo = NULL;
    delete variaveis;
}

int Modelo::Modelo::criaRota(Solucao::ClienteRota *vetClienteRota, const int tam, bool tipo, int peso, const Instancia::Instancia *instancia, double *poluicao, double *combustivel)
{


    if(tipo != this->tipoVeiculo)
    {

        this->tipoVeiculo = tipo;
        modelo->setObjective(variaveis->funcaoObjetivo[tipo], GRB_MINIMIZE);

        if(tipo)
        {
            variaveis->T[0].set(GRB_DoubleAttr_LB, 1);
            variaveis->T[0].set(GRB_DoubleAttr_UB, 1);

            variaveis->T[1].set(GRB_DoubleAttr_LB, 0);
            variaveis->T[1].set(GRB_DoubleAttr_UB, 0);
        }
        else
        {
            variaveis->T[0].set(GRB_DoubleAttr_LB, 0);
            variaveis->T[0].set(GRB_DoubleAttr_UB, 0);

            variaveis->T[1].set(GRB_DoubleAttr_LB, 1);
            variaveis->T[1].set(GRB_DoubleAttr_UB, 1);
        }


    }


    int cliente1, cliente2;

    long double combustivelAux = 0.0, poluicaoAux = 0.0;
    bool viavel = false;
    int pesoAux = peso;

    vetClienteRota[0].tempoSaida = (tipo ?  0.5 : 0.0);

    /*for(int i = 1; i < tam; ++i)
    {
        if(!Construtivo::determinaHorario(&vetClienteRota[i-1], &vetClienteRota[i], instancia, pesoAux, tipoVeiculo, NULL, NULL))
        {
            //cout<<"Janela de tempo\n";
            return 0;
        }


        poluicaoAux += vetClienteRota[i].poluicao;
        combustivelAux += vetClienteRota[i].combustivel;

        if(!VerificaSolucao::verificaCombustivel(combustivelAux, tipoVeiculo, instancia))
            viavel = false;

        pesoAux -= instancia->vetorClientes[vetClienteRota[i].cliente].demanda;

    }*/

    if(viavel)
    {
        variaveis->C[tipo].set(GRB_DoubleAttr_Start, combustivelAux);

        for(int i = 1; i < tam - 1; ++i)
        {
            if (vetClienteRota[i].tempoChegada < instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela)
            {

                int ultimoPeriodo = -1;

                for(int k = 4; k >= 0; --k)
                {
                    if(vetClienteRota[i].percorrePeriodo[k])
                    {
                        ultimoPeriodo = k;
                        break;
                    }
                }

                if(ultimoPeriodo != 0)
                {
                    cout<<"\n\nErro, ultimo periodo nao eh 0\n";
                    cout<<"\nArquivo: Modelo.cpp\n";
                    cout<<"Funcao: criaRotaMip\n";
                    cout<<"Linha: "<<__LINE__<<"\n\n";
                    exit(-1);
                }

                //vetClienteRota[i].tempoPorPeriodo[ultimoPeriodo] += instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela - vetClienteRota[i].tempoChegada;
                vetClienteRota[i].tempoChegada = instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela;
                vetClienteRota[i-1].tempoSaida = vetClienteRota[i-1].tempoSaida + (instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela - vetClienteRota[i].tempoChegada);
            }
        }
    }

    for(int i = 1; i < tam; ++i)
    {
        cliente1 = vetClienteRota[i-1].cliente;
        cliente2 = vetClienteRota[i].cliente;

        variaveis->X[cliente1][cliente2].set(GRB_DoubleAttr_LB, 1);
        variaveis->X[cliente1][cliente2].set(GRB_DoubleAttr_UB, 1);

        variaveis->f[cliente1][cliente2].set(GRB_DoubleAttr_UB, peso);
        variaveis->f[cliente1][cliente2].set(GRB_DoubleAttr_LB, peso);

        if(viavel)
        {
            variaveis->l[cliente1].set(GRB_DoubleAttr_Start, vetClienteRota[i-1].tempoSaida);
            variaveis->a[cliente2].set(GRB_DoubleAttr_Start, vetClienteRota[i].tempoChegada);

            for(int k = 0; k < instancia->numPeriodos; ++k)
            {

                variaveis->x[cliente1][cliente2][k].set(GRB_DoubleAttr_Start, vetClienteRota[i].percorrePeriodo[k]);
                variaveis->tao[cliente1][cliente2][k].set(GRB_DoubleAttr_Start, vetClienteRota[i].tempoPorPeriodo[k]);
                variaveis->d[cliente1][cliente2][k].set(GRB_DoubleAttr_Start, vetClienteRota[i].distanciaPorPeriodo[k]);
            }
        }

        peso -= instancia->vetorClientes[cliente2].demanda;
    }


    //modelo->update();
    modelo->reset(0);

    //modelo->feasRelax(1, true, false, true);
    //modelo->write("modelo.lp");
    modelo->optimize();


    if(modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL)
    {
        //modelo->write("modelo.sol");
        // Copia a solução

        *poluicao = modelo->get(GRB_DoubleAttr_ObjVal);
        *combustivel = variaveis->C[tipo].get(GRB_DoubleAttr_X);

        for(int i = 0; i < tam; ++i)
        {
            vetClienteRota[i].tempoChegada = variaveis->a[vetClienteRota[i].cliente].get(GRB_DoubleAttr_X);
            vetClienteRota[i].tempoSaida = variaveis->l[vetClienteRota[i].cliente].get(GRB_DoubleAttr_X);

            if(i != 0)
            {

                for (int k = 0; k < numPeriodos; ++k)
                {

                    vetClienteRota[i].tempoPorPeriodo[k] = variaveis->tao[vetClienteRota[i-1].cliente][vetClienteRota[i].cliente][k].get(GRB_DoubleAttr_X);
                    vetClienteRota[i].distanciaPorPeriodo[k] = variaveis->d[vetClienteRota[i-1].cliente][vetClienteRota[i].cliente][k].get(GRB_DoubleAttr_X);
                    vetClienteRota[i].percorrePeriodo[k] = bool(variaveis->x[vetClienteRota[i-1].cliente][vetClienteRota[i].cliente][k].get(GRB_DoubleAttr_X));

                    /*if(vetClienteRota[i].tempoPorPeriodo[k] < 1e-5)
                    {
                        vetClienteRota[i].percorrePeriodo[k] = false;
                    }*/
                }

            }
        }

        if(vetClienteRota[1].tempoChegada < instancia->vetorClientes[vetClienteRota[1].cliente].inicioJanela)
        {
            vetClienteRota[1].tempoChegada = instancia->vetorClientes[vetClienteRota[1].cliente].inicioJanela;


        }

        int ultimoPeriodo = -1, quant = 0, primeiroPeriodo = 5;

        for(int k = 0; k < numPeriodos; ++k)
        {


            if(vetClienteRota[1].percorrePeriodo[k])
            {

                ++quant;
                ultimoPeriodo = k;

                if(k < primeiroPeriodo)
                    primeiroPeriodo = k;
            }
        }


        if(quant == 1)
            vetClienteRota[0].tempoSaida = vetClienteRota[1].tempoChegada - vetClienteRota[1].tempoPorPeriodo[primeiroPeriodo];
        else
        {
            vetClienteRota[0].tempoSaida = instancia->vetorPeriodos[primeiroPeriodo].fim - vetClienteRota[1].tempoPorPeriodo[primeiroPeriodo];

        }


        ultimoPeriodo = -1;
        quant = 0;

        for(int k = 0; k < numPeriodos; ++k)
        {
            if(vetClienteRota[tam - 1].percorrePeriodo[k])
            {
                ++quant;
                ultimoPeriodo = k;
            }
        }

        if(quant == 1)
        {
            vetClienteRota[tam - 1].tempoChegada = vetClienteRota[tam - 2].tempoSaida + vetClienteRota[tam - 1].tempoPorPeriodo[ultimoPeriodo];
        }
        else
        {
            vetClienteRota[tam - 1].tempoChegada = instancia->vetorPeriodos[ultimoPeriodo].inicio + vetClienteRota[tam - 1].tempoPorPeriodo[ultimoPeriodo];
        }

    }

    if(modelo->get(GRB_IntAttr_Status) != GRB_OPTIMAL)
    {
        //modelo->computeIIS();
        //modelo->write("modelo.ilp");
    }

    bool resultado = modelo->get(GRB_IntAttr_Status) == GRB_OPTIMAL;

    if(viavel)
        variaveis->C[tipo].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

    for(int i = 1; i < tam; ++i)
    {
        cliente1 = vetClienteRota[i-1].cliente;
        cliente2 = vetClienteRota[i].cliente;

        variaveis->X[cliente1][cliente2].set(GRB_DoubleAttr_LB, 0);
        variaveis->X[cliente1][cliente2].set(GRB_DoubleAttr_UB, 0);

        variaveis->f[cliente1][cliente2].set(GRB_DoubleAttr_LB, 0.0);
        variaveis->f[cliente1][cliente2].set(GRB_DoubleAttr_UB, 0.0);

        if(viavel)
        {
            variaveis->l[cliente1].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
            variaveis->a[cliente2].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);

            for(int k = 0; k < instancia->numPeriodos; ++k)
            {

                variaveis->x[cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                variaveis->tao[cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
                variaveis->d[cliente1][cliente2][k].set(GRB_DoubleAttr_Start, GRB_UNDEFINED);
            }
        }
    }

    modelo->set(GRB_IntParam_SubMIPNodes, GRB_MAXINT);

    if(resultado)
        return 1;
    else
        return 0;


}

void Modelo::geraRotasOtimas(Solucao::Solucao *solucao, Modelo *modelo, Solucao::ClienteRota *vetClienteRota, const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas)
{

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

        for(int i = 0; i < veiculo->listaClientes.size(); ++i, ++itCliente)
            vetClienteRota[i].swap(*itCliente);

        HashRotas::HashNo *hashNo = hashRotas->getVeiculo(vetClienteRota, veiculo->listaClientes.size(), veiculo->tipo);

        if(hashNo)
        {
            auto veiculo_h = hashNo->veiculo;
            auto veiculo_ = vetClienteRota;

            for(int i = 0; i < hashNo->tam; ++i)
            {
                veiculo_->swap(veiculo_h);

                ++veiculo_;
                ++veiculo_h;
            }

            combustivel = hashNo->combustivel;
            poluicao = hashNo->poluicao;

        }
        else
        {

            //Cria rota
            try
            {


                resultado = modelo->criaRota(vetClienteRota, veiculo->listaClientes.size(), veiculo->tipo,
                                             veiculo->carga, instancia, &poluicao, &combustivel);
            } catch (GRBException e)
            {
                cout << "Erro MIP, tipo: " << veiculo->tipo << "\nRota: " << veiculo->getRota() << '\n';
                cout << "Erro code: " << e.getErrorCode();
                cout << "Mensagem: " << e.getMessage() << '\n';
                delete modelo;
                exit(-1);
            }

        }

        if(poluicao < veiculo->poluicao)
        {
            vetClienteRota[0].rotaMip = true;
            //Atualiza solucao
            auto it = veiculo->listaClientes.begin();

            for(int i = 0; i < veiculo->listaClientes.size(); ++i, ++it)
                (*it)->swap(&vetClienteRota[i]);

            //Retira poluicao do veiculo e adiciona a nova poluicao
            solucao->poluicao -= veiculo->poluicao;
            solucao->poluicao += poluicao;

            //Atualiza poluicao e combustivel do veiculo
            veiculo->poluicao = poluicao;
            veiculo->combustivel = combustivel;

            if(hashNo == NULL)
            {
                if(!hashRotas->insereVeiculo(veiculo))
                {
                    cout<<"Erro, nao foi possivel inserir a rota: "<<veiculo->getRota()<<'\n'<<"Tipo: "<<veiculo->tipo<<'\n';
                    delete modelo;
                    exit(-1);
                }
                //else
                  //  cout<<"Inseriu veiculo do tipo: "<<veiculo->tipo<<" rota: "<<veiculo->getRota()<<"\n\n";
            }
        }
/*        else
            cout<<"rota mip eh maior. Mip: "<<poluicao<<",  original: "<<veiculo->poluicao<<'\n';*/
    }

    solucao->rotasMip = true;

}