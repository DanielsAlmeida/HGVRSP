#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include "Instancia.h"
#include "Solucao.h"
#include "VerificaSolucao.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "Constantes.h"
#include "Vnd.h"
#include "Movimentos_Paradas.h"
#include "time.h"
#include "Modelo.h"
#include <string>
#include <fstream>
#include "HashRotas.h"

//UK_10x5_5
//Tempo total cpu: 90.45 S
//Poluicao: 520.58

//Tempo total cpu: 356.13
//Poluicao: 517.11

#define Saida true
#define Grasp 0
#define RotaMip 1
#define VerificaSol 2

#define Opcao RotaMip

// UK_50x5_5_0 90.8872    0 24 50 38 0   tempo: 0.07, presove: 0.05, Poluicao: 90.88, Combustivel: 34.12
//  UK_50x5_6 1593111849
// /home/igor/Documentos/HGVRSP/instanciasUK/UK_10x5_2.dat /home/igor/Documentos/HGVRSP/saidaCompleta.txt /home/igor/Documentos/HGVRSP/saidaParcial.txt
/* 1585430665
 * Debug memória : g++ *.cpp -Wall -fsanitize=address -g
 *
 * date '+%d_%m_%Y_%k-%M-%S'
 *
 * Arquivo de log: /home/igor/Documentos/HGVRSP/saidaLog.txt
 *
 ****************************************************Parametro para criterio : folga -> poluicao**************************************************************
 *
 * Instancia 10 clientes, parametro: 0.12
 * Instancia 15 clientes, parametro: 0.99
 * Instancia 20 clientes, parametro: 0.67
 * Instancia 25 clientes, parametro: 0.54
 * Instancia 50 clientes, parametro: 0.79
 * Instancia 75 clientes, parametro: 0.82
 *
    parametro.insert({75, 0.82});
    parametro.insert({50, 0.79});
    parametro.insert({25, 0.54});
    parametro.insert({20, 0.67});
    parametro.insert({15, 0.99});
    parametro.insert({10, 0.39});
 *
 * ************************************************************************************************************************************************************
 *
 *
 * ****************************************************Parametro para criterio : poluicao -> folga**************************************************************
 *
 * Instancia 10 clientes, parametro: 0.43
 * Instancia 15 clientes, parametro: 0.27
 * Instancia 20 clientes, parametro: 0.19
 * Instancia 25 clientes, parametro: 0.15
 * Instancia 50 clientes, parametro: 0.94
 * Instancia 75 clientes, parametro: 0.46
 *
    parametro.insert({10, 0.43});
    parametro.insert({15, 0.27});
    parametro.insert({20, 0.19});
    parametro.insert({25, 0.15});
    parametro.insert({50, 0.94});
    parametro.insert({75, 0.46});
 *******************************************************************************************************************************************************************

 * ****************************************************************************************************************************************************************/

using namespace std;

#if Opcao == Grasp
int main(int num, char **agrs)
{



    std::map<int, double> parametroHeur0;
    std::map<int, double> parametroHeur1;
    std::map<int, double> parametroHeur3;
    std::map<int, double> parametroHeur4;
    std::map<int, double> parametroHeur5;
    std::map<int, double> parametroHeur6;
    

    parametroHeur0.insert({10, 0.35});
    parametroHeur0.insert({15, 0.73});
    parametroHeur0.insert({20, 0.19});
    parametroHeur0.insert({25, 0.99});
    parametroHeur0.insert({50, 0.73});
    parametroHeur0.insert({75, 0.36});
    parametroHeur0.insert({100, 0.92});

    parametroHeur1.insert({10, 0.81});
    parametroHeur1.insert({15, 0.42});
    parametroHeur1.insert({20, 0.94});
    parametroHeur1.insert({25, 0.55});
    parametroHeur1.insert({50, 0.44});
    parametroHeur1.insert({75, 0.68});
    parametroHeur1.insert({100, 0.96});

    parametroHeur3.insert({10, 0.41});
    parametroHeur3.insert({15, 0.97});
    parametroHeur3.insert({20, 0.23});
    parametroHeur3.insert({25, 0.66});
    parametroHeur3.insert({50, 0.34});
    parametroHeur3.insert({75, 0.73});
    parametroHeur3.insert({100, 0.12});

    parametroHeur4.insert({10, 1.0});
    parametroHeur4.insert({15, 0.3});
    parametroHeur4.insert({20, 0.23});
    parametroHeur4.insert({25, 0.17});
    parametroHeur4.insert({50, 0.98});
    parametroHeur4.insert({75, 0.76});
    parametroHeur4.insert({100, 0.87});

    parametroHeur5.insert({10, 0.7});
    parametroHeur5.insert({15, 0.86});
    parametroHeur5.insert({20, 0.3});
    parametroHeur5.insert({25, 0.36});
    parametroHeur5.insert({50, 0.35});
    parametroHeur5.insert({75, 0.65});
    parametroHeur5.insert({100, 0.55});

    parametroHeur6.insert({10, 0.16});
    parametroHeur6.insert({15, 0.31});
    parametroHeur6.insert({20, 0.56});
    parametroHeur6.insert({25, 0.61});
    parametroHeur6.insert({50, 0.29});
    parametroHeur6.insert({75, 0.57});
    parametroHeur6.insert({100, 0.34});


    string strInstancia;
    string saidaCompleta;
    string saidaParcial;
    string instanciaNome;
    string log;

    bool logAtivo = false;

    if(num != 5)
    {
        cout<<"Numero incorreto de parametros.\n";
        exit(-1);
    }


    strInstancia = agrs[1];
    saidaCompleta = agrs[2];
    saidaParcial = agrs[3];

    ofstream file;

    for(int i = 0; i < strInstancia.length(); ++i)
    {
        if(strInstancia[i] != '/')
        {

            if(strInstancia[i] == '.')
                break;
            instanciaNome += strInstancia[i];

        }

        else
            instanciaNome = "";

    }

    auto semente  = time(NULL);

    if(std::atoll(agrs[4]) != 0)
        semente = std::atoll(agrs[4]);

    cout<<"Semente = "<<semente<<'\n';



    string texto;
    std::time_t result = std::time(nullptr);
    auto data = std::asctime(std::localtime(&result));
    texto += data;
    texto += '\n';

    texto += "Nome: " + instanciaNome + "\n\n";

    seed(semente);

    texto += "Semente: " + std::to_string(semente) + "\n\n";

    Instancia::Instancia *instancia = new Instancia::Instancia(strInstancia);

    const double vetParametro[7] = {parametroHeur0[instancia->numClientes - 1], parametroHeur1[instancia->numClientes - 1], 0.0, parametroHeur3[instancia->numClientes - 1],
                                    parametroHeur4[instancia->numClientes - 1], parametroHeur5[instancia->numClientes -1], parametroHeur6[instancia->numClientes - 1]};

    auto vet = instancia->vetorClientes;

    #define numAlfas  9
    //float vetAlfas[numAlfas] = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9};
    float vetAlfas[numAlfas] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    //{{2,1}, {1,3}, {5, 3}};

    #define TamVetH 2

    boost::tuple<int,int> vetHeuristicas[TamVetH] = {{2,1}, {2,3}};
    
    if(vetHeuristicas[0].get<1>() < 0 || vetHeuristicas[0].get<1>() > 3)
    {
        cout<<"Erro, heuristicas informadas de forma incorreta\n";
        exit(-1);
    }

    //Alaoca matrix para guardar a melhor rota de cada candidato

    Solucao::ClienteRota **matrixClienteBest = new Solucao::ClienteRota *[instancia->numClientes];

    for(int i = 0; i < instancia->numClientes; ++i)
        matrixClienteBest[i] = new Solucao::ClienteRota[MaxTamVetClientesMatrix];

    std::stringstream strLog;
    string logAux;

    Movimentos_Paradas::TempoCriaRota tempoCriaRota;
    Vnd::EstatisticaMv vetEstatisticaMv[9];
    Construtivo::GuardaCandInteracoes *vetCandInteracoes = new Construtivo::GuardaCandInteracoes[instancia->numClientes+2];
    double vetLimiteTempo[20];

    GRBEnv env;
    env.set(GRB_IntParam_OutputFlag, 0);
    GRBModel grb_modelo = GRBModel(env);

    Modelo::Modelo *modelo = new Modelo::Modelo(instancia, &grb_modelo, false);

    auto c_start = std::chrono::high_resolution_clock::now();

    auto *solucao = Construtivo::grasp(instancia, vetAlfas, numAlfas, 1000, 150, logAtivo, &strLog, vetHeuristicas, TamVetH, vetParametro, vetEstatisticaMv,
                                       matrixClienteBest, &tempoCriaRota, vetCandInteracoes, vetLimiteTempo, modelo);

    auto c_end = std::chrono::high_resolution_clock::now();

    /*
    if(!solucao->veiculoFicticil)
    {
        HashRotas::HashRotas hashRotas(instancia->numClientes);


        for(auto veiculo : solucao->vetorVeiculos)
        {
            if(!hashRotas.insereVeiculo(veiculo) && veiculo->listaClientes.size() > 2)
                cout<<"Nao inseriu veiculo\n";
            else
                cout<<"Inseriu veiculo\n";

            HashRotas::HashNo *hashNo = hashRotas.getVeiculo(veiculo);

            if(hashNo == NULL)
                cout<<"hash igual a NULL\n";
            else
            {
                cout << veiculo->getRota() << "  ";

                Solucao::ClienteRota *clienteRota = hashNo->veiculo;

                for (int i = 0; i < hashNo->tam; ++i, ++clienteRota)
                    cout << clienteRota->cliente << ' ';

                cout<<'\n';
            }
        }

        float media;
        int maior;

        hashRotas.estatisticasHash(&media, &maior);

        cout<<"media: "<<media<<"\nmaior: "<<maior<<'\n';
    }*/

    delete []vetCandInteracoes;


    //desaloca matrix
    for(int i = 0; i < instancia->numClientes; ++i)
        delete []matrixClienteBest[i];

    delete []matrixClienteBest;

    [](Solucao::Solucao *solucao)
    {



#if Saida

        if(solucao->rotasMip)
            cout<<"Rotas geradas pelo resolvedor\n";

        /*cout<<"Solucao:\n\n";

        for(auto it = solucao->vetorVeiculos[0]->listaClientes.begin(); it != solucao->vetorVeiculos[0]->listaClientes.end(); ++it)
        {
            auto prox = it;
            ++prox;

            cout<<(*it)->cliente<<" - "<<(*prox)->cliente<<" : ";

            for(int i = 0; i < 5; ++i)
            {
                if((*prox)->percorrePeriodo[i])
                    cout<<"("<<(*prox)->percorrePeriodo[i]<<" "<<(*prox)->distanciaPorPeriodo[i]<<" "<<(*prox)->tempoPorPeriodo[i]<<")  ";
                else
                    cout<<"(0 0 0) ";

            }
            cout<<"\n\n";

            if((*prox)->cliente == 0)
                break;
        }*/
#endif
    }(solucao);

    strLog<<logAux;
    //strLog<<"\n\nRota:\n";

    cout << fixed << setprecision(2);
    texto += std::to_string(solucao->vetorVeiculos.size()) + "\n\n";
    //strLog<<std::to_string(solucao->vetorVeiculos.size()) + "\n\n";

    double tempoViagem = 0.0;
    double inicio;
    bool fim = false;

    for(auto veiculo : solucao->vetorVeiculos)
    {
        fim = false;

        texto += std::to_string(veiculo->listaClientes.size()-2) + '\n';
        strLog<<std::to_string(veiculo->listaClientes.size()-2) + '\n';
        for (auto it : (*veiculo).listaClientes)
        {
            texto += std::to_string((*it).cliente) + " ";
            strLog<<std::to_string((*it).cliente) + " ";

            if(((*it).cliente==0) && (!fim))
            {
                fim = true;
                inicio = it->tempoSaida;
            }

            else if(((*it).cliente==0) && (fim))
            {
                tempoViagem += it->tempoChegada - inicio;
            }
        }

        texto += "\n";
        strLog<<"\n\n";



    }

    strLog<<"CLIENTE\t\tTEMPO CHEGADA\t\tTEMPO SAIDA\n";

    for(auto veiculo : solucao->vetorVeiculos)
    {

        for(auto cliente : (*veiculo).listaClientes)
        {

            if(cliente->cliente != 0)
            {
                texto += std::to_string(cliente->cliente) +" " + std::to_string(cliente->tempoChegada) + " " + std::to_string(cliente->tempoSaida) + "\n";
                strLog<< std::to_string(cliente->cliente) +"\t\t\t\t" + std::to_string(cliente->tempoChegada) + "\t\t\t" + std::to_string(cliente->tempoSaida) + "\n";

            }
        }

    }
    texto += '\n';
    strLog<<'\n';

    double distanciaTotal;

    bool Veificacao = false;

    /*if(!solucao->veiculoFicticil)
        Veificacao = VerificaSolucao::verificaSolucao(instancia, solucao, &texto, &distanciaTotal);*/

    if(!solucao->veiculoFicticil)
    {
        for (auto veiculo : solucao->vetorVeiculos)
        {
            if(veiculo->listaClientes.size() == 2)
                continue;

            auto cliente2 = veiculo->listaClientes.begin();

            int peso = veiculo->carga;

            for(auto cliente1 = veiculo->listaClientes.begin(); ;++cliente1)
            {
                cliente2 = cliente1;
                ++cliente2;

                texto += std::to_string((*cliente1)->cliente) + ' ' + std::to_string((*cliente2)->cliente) + ' ';

                int quant = 0, ultimo = -1;

                for (int i = 0; i < 5; ++i)
                {
                    if ((*cliente2)->percorrePeriodo[i])
                    {
                        ++quant;

                        ultimo = i;
                    }
                }

                for (int i = 0; i < ultimo; ++i)
                {
                    if ((*cliente2)->percorrePeriodo[i])
                    {
                        double poluicao = VerificaSolucao::poluicaoRota(instancia, veiculo->tipo,
                                                                        (*cliente2)->distanciaPorPeriodo[i],
                                                                        (*cliente1)->cliente, (*cliente2)->cliente, i);

                        texto += std::to_string(i) + "," + std::to_string((*cliente2)->tempoPorPeriodo[i]) + "," +
                                 std::to_string((*cliente2)->distanciaPorPeriodo[i]) + ", " + to_string(poluicao) +
                                 "," +
                                 to_string(instancia->matrizVelocidade[(*cliente1)->cliente][(*cliente2)->cliente][i]) +
                                 ",0 ";
                    }
                }

                {
                    double poluicao = VerificaSolucao::poluicaoRota(instancia, veiculo->tipo,
                                                                    (*cliente2)->distanciaPorPeriodo[ultimo],
                                                                    (*cliente1)->cliente, (*cliente2)->cliente, ultimo);
                    poluicao += VerificaSolucao::poluicaoCarga(instancia, veiculo->tipo, peso,
                                                               instancia->matrizDistancias[(*cliente1)->cliente][(*cliente2)->cliente]);

                    texto +=
                            std::to_string(ultimo) + "," + std::to_string((*cliente2)->tempoPorPeriodo[ultimo]) + "," +
                            std::to_string((*cliente2)->distanciaPorPeriodo[ultimo]) + "," + to_string(poluicao) +
                            "," + to_string(instancia->matrizVelocidade[(*cliente1)->cliente][(*cliente2)->cliente][ultimo]) + ",0"+
                            '\n';

                }

                peso -= instancia->vetorClientes[(*cliente2)->cliente].demanda;

                if ((*cliente2)->cliente == 0)
                    break;
            }
        }
    }
    string erro;

    if(!solucao->veiculoFicticil)
    {
        Veificacao = true;

        for(auto veiculo : solucao->vetorVeiculos)
        {
            bool v = VerificaSolucao::verificaVeiculoRotaMip(veiculo, instancia, NULL, &erro);

            if(!v)
            {
                cout<<"Erro, rota: "<<veiculo->getRota()<<"\nTipo: "<<veiculo->tipo<<"\n\nMotivo: "<<erro<<"\n\n";
                cout<<"Combustivel veiculo: "<<veiculo->combustivel<<'\n';
                erro = "";
            }

            Veificacao = Veificacao * v;
        }
    }

    texto += '\n';

    std::setprecision(2);

    strLog<<"CLIENTE\t\tINICIO JANELA\t\tFIMJANELA\t\tTEMPO SERVICO\n";

    for(int i = 1; i < instancia->numClientes; ++i)
    {
        texto += std::to_string(vet[i].cliente) + " " + std::to_string(vet[i].inicioJanela) + " " + std::to_string(vet[i].fimJanela) + "\n";
        strLog<< std::to_string(vet[i].cliente) + "\t\t\t\t" + std::to_string(vet[i].inicioJanela) + "\t\t" + std::to_string(vet[i].fimJanela) + "\t\t" + std::to_string(vet[i].tempoServico)+'\n';
    }

    texto += "-1\n\n\n";
    strLog<<"\n\n";
    //string tempo;

    std::stringstream tempo;
    tempo << std::fixed << std::setprecision(2); //    std::string s = stream.str();

    std::chrono::duration<double> tempoCpu = c_end-c_start;

    for(int i = 0; i < 9 ; ++i)
    {
        if(vetEstatisticaMv[i].num > 0)
        {
            vetEstatisticaMv[i].poluicao /= vetEstatisticaMv[i].num;
            vetEstatisticaMv[i].gap /= vetEstatisticaMv[i].num;
            vetEstatisticaMv[i].tempo /= vetEstatisticaMv[i].numTempo;
        }
    }

    tempo << "Tempo total cpu: " << tempoCpu.count()<< " S\n";
    tempo <<"Tempo total construtivo: "<<solucao->tempoConstrutivo<<" S\n";
    tempo <<"Tempo total viabilizador: "<<solucao->tempoViabilizador<<" S\n";
    tempo <<"Tempo total Vnd: "<<solucao->tempoVnd<<" S\n\n";

    tempo<<"Tamanho medio vetor: "<<double(tempoCriaRota.tamVet)/ tempoCriaRota.num<<'\n';
    tempo<<"Numero chamadas: "<<tempoCriaRota.num<<"\n";
    tempo<<"Maior intervalo tempo: "<<tempoCriaRota.maior<<'\n';
    tempo << "Verificacao: " <<(Veificacao) << "\n";
    tempo << "Poluicao: " <<(solucao->poluicao + solucao->poluicaoPenalidades) << '\n';
    tempo << "Ultima atualizacao: " << (solucao->ultimaAtualizacao) << '\n';
    tempo << "Numero de solucoes inviaveis: " << (solucao->numSolucoesInv) << '\n';
    tempo << "Solucoes viabilizadas: "<<solucao->solucoesViabilizadas<<'\n';
    tempo << "Tempo total de viagem: " << (tempoViagem*60.0) << '\n';
    tempo << "Distancia total: " << (distanciaTotal)<<"\n";

    int numVeiculosUsados = 0;


    for(auto veiculo : solucao->vetorVeiculos)
    {

        if(veiculo->listaClientes.size() > 2)
        {
            ++numVeiculosUsados;

        }

    }

    tempo<<"Numero de veiculo: "<<numVeiculosUsados<<"\n\n";

    tempo << std::fixed << std::setprecision(4);
    for(int i = 0; i < 8 ; ++i)
        tempo<<vetEstatisticaMv[i].poluicao<<" ("<<vetEstatisticaMv[i].tempo<<")    ";

    tempo<<"\n";
    texto += tempo.str();

    //if(solucao->poluicao <= 0.1)
    //    cout<<"Poluicao = 0\n";

    //cout<<"Tempo cpu: " << ((1000.0*c_end-c_start) / CLOCKS_PER_SEC/1000.0) << " S\n";

    if(num == 1)
    {

    }
    else
    {

        file.open(saidaCompleta, ios::out);
        file << texto;

        file.close();
        file.open(saidaParcial, ios::out | ios::app);


            // Poluicao (kg), tempo cpu (SEC), ultima atualizacao, numero de solucoes inviaveis, tempo total de viagem (min), distancia total (km).

        if(Veificacao)
        {


            file << std::to_string(solucao->poluicao) << ' ' << tempoCpu.count() <<' '<< std::to_string(instancia->numVeiculos) <<' '<< std::to_string(numVeiculosUsados) << ' '
            << std::to_string(solucao->numSolucoesInv) << ' ' << std::to_string(solucao->ultimaAtualizacao)<<' '<<solucao->solucoesViabilizadas<<' '<<solucao->tempoConstrutivo<<' '
            <<solucao->tempoViabilizador<<' '<<solucao->tempoVnd;

            for(int i = 0; i < 8; ++i)
                file<<" "<<vetEstatisticaMv[i].poluicao;

            for(int i = 0; i < 8; ++i)
                file<<" "<<vetEstatisticaMv[i].tempo;

            file<<'\n';

        }
        else
            file << std::to_string(0.0) << " " <<tempoCpu.count()<<
                 " " <<std::to_string(instancia->numVeiculos)<<" " << std::to_string(numVeiculosUsados) <<" "<<std::to_string(solucao->numSolucoesInv) << " "<< std::to_string(solucao->ultimaAtualizacao)<<
                 solucao->solucoesViabilizadas<<' '<<solucao->tempoConstrutivo<<' ' <<solucao->tempoViabilizador<<' '<<solucao->tempoVnd<<
                 " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"<<'\n';

         /*if(Veificacao)
             file << std::to_string(solucao->poluicao) << " " << tempoCpu.count() <<
                 " "<<std::to_string(solucao->ultimaAtualizacao)<<" " <<std::to_string(solucao->numSolucoesInv) <<" "<<std::to_string(tempoViagem*60.0)<<" "<<std::to_string(distanciaTotal)<<'\n';

         else
             file << std::to_string(0.0) << " " <<tempoCpu.count()<<
             " "<<std::to_string(solucao->ultimaAtualizacao)<<" " <<std::to_string(solucao->numSolucoesInv) <<" "<<std::to_string(tempoViagem*60.)<<" "<<std::to_string(distanciaTotal)<<'\n';*/

        file.close();

    }

    #if Saida

        cout<<"Instancia: "<<instanciaNome<<'\n';
        cout<<tempo.str()<<'\n';
        cout<<"Semente: "<<semente<<'\n';

        if(solucao->veiculoFicticil)
        {
            auto lista = solucao->vetorVeiculos[solucao->vetorVeiculos.size()-1];
            cout<<"Clientes sem rota: "<<lista->listaClientes.size()-2;
        }

        cout<<"\n";
        int i = 0;

        for(auto veiculo : solucao->vetorVeiculos)
        {
            auto cliente = veiculo->listaClientes.begin();
            cout<<i<<" P: "<<veiculo->poluicao<<" C: "<<veiculo->combustivel<<" MIP: "<<(*cliente)->rotaMip<<" Rota: "<<veiculo->getRota()<<'\n';

            ++i;
        }





    #endif

    if(num == 5)
    {
        file.open(log, ios::out | ios::app);

        file<<instanciaNome<<'\n';
        file << data<<"\n";
        file<<strLog.str();
        file<<tempo.str()<<"\n\n";

        file.close();

    }

    /* ********************************************************************************************************************************************************************************** */

/*    cout<<"Cria rotas\n\n";
    cout<<"Num de veiculos disponiveis: "<<instancia->numVeiculos<<"\n";

    cout<<"************************************************************************************************************\n\n";

    //Tenta criar rotas:
    Solucao::ClienteRota *vetClienteRota = new Solucao::ClienteRota[instancia->numClientes];

    double poluicao, combustivel;

    int l = 0;

    for(auto veiculo : solucao->vetorVeiculos)
    {


        int k = 0;
        for(auto cliente : veiculo->listaClientes)
        {
            vetClienteRota[k] = *cliente;
            k++;
        }

        if(Movimentos_Paradas::criaRota(instancia, vetClienteRota, k, veiculo->carga, veiculo->tipo, &combustivel, &poluicao))
            cout<<"Rota criada. Poluicao original: "<<veiculo->poluicao<<" Nova poluicao: "<<poluicao<<"\n\n";
        else
            cout<<"Falha ao criar rota\n\n";

        cout<<"************************************************************************************************************\n\n";

        l++;

        if(l==instancia->numVeiculos)
            break;
    }

    delete []vetClienteRota;*/

/* ********************************************************************************************************************************************************************************** */

    delete solucao;
    delete instancia;
    delete modelo;

    //muntrace();

    return 0;
}


# elif Opcao == RotaMip
 int main(int num, char **agrs)
{

    string strInstancia;
    string instanciaNome;
    string log;

    bool logAtivo = false;

    if(num != 2)
    {
        cout<<"Numero incorreto de parametros.\n";
        exit(-1);
    }


    strInstancia = agrs[1];


    ofstream file;

    for(int i = 0; i < strInstancia.length(); ++i)
    {
        if(strInstancia[i] != '/')
        {

            if(strInstancia[i] == '.')
                break;
            instanciaNome += strInstancia[i];

        }

        else
            instanciaNome = "";

    }



    string texto;
    std::time_t result = std::time(nullptr);
    auto data = std::asctime(std::localtime(&result));
    texto += data;
    texto += '\n';

    texto += "Nome: " + instanciaNome + "\n\n";


    Instancia::Instancia *instancia = new Instancia::Instancia(strInstancia);
    Modelo::Modelo *modelo;

    try
    {


        GRBEnv env;
        env.set(GRB_IntParam_OutputFlag, 1);
        env.set(GRB_IntParam_DisplayInterval, 1);

        GRBModel grb_modelo = GRBModel(env);
        Modelo::solucaoInteira solucaoInteira;

        grb_modelo.setCallback(&solucaoInteira);

        modelo = new Modelo::Modelo(instancia, &grb_modelo, false);



        // .mps, .rew, .lp, .rlp, or .ilp




        Solucao::ClienteRota *vetCliente = new Solucao::ClienteRota[MaxTamVetClientesMatrix];
        Solucao::ClienteRota *vetCliente2 = new Solucao::ClienteRota[MaxTamVetClientesMatrix];

        Solucao::ClienteRota *vetClienteAux = new Solucao::ClienteRota[MaxTamVetClientesMatrix];
        double *vetLimiteTempo = new double [MaxTamVetClientesMatrix];

        int vetRotas[MaxTamVetClientesMatrix];
        int vetRotas2[MaxTamVetClientesMatrix];

        vetCliente[0].cliente = 0;

        int tipo, tam, peso, cliente = 1;
        int tipo2, tam2, peso2;

        double combustivel, poluicao, combustivel2, poluicao2;
        int clientesTrocados;

        do
        {
            cout<<"Tipo: ";
            cin>>tipo;

            if(tipo != -1)
            {
                cout<<"clientes trocados: ";
                cin>>clientesTrocados;

                if(clientesTrocados < 0)
                    break;

                cout<<"Rota:  ";

                tam = cliente = 0;
                peso = 0;
                combustivel = poluicao = 0.0;

                do
                {

                    cin>>cliente;
                    ++tam;

                    vetCliente[tam-1].cliente = cliente;
                    peso += instancia->vetorClientes[cliente].demanda;


                }while(cliente || tam == 1);

                bool viavel = Movimentos_Paradas::criaRota(instancia, vetCliente, tam, peso, tipo, &combustivel, &poluicao, NULL, NULL, vetLimiteTempo, vetClienteAux);
                double poluicaoHeur = poluicao;

                if(viavel)
                {

                    cout<<"Segunda rota: \n";
                    cout<<"tipo: ";
                    cin>>tipo2;

                    cout<<"Rota:  ";

                    tam2 = cliente = 0;
                    peso2 = 0;
                    combustivel2 = poluicao2 = 0.0;

                    do
                    {

                        cin>>cliente;
                        ++tam2;

                        vetCliente2[tam2-1].cliente = cliente;
                        peso2 += instancia->vetorClientes[cliente].demanda;


                    }while(cliente || tam2 == 1);

                    bool viavel2 = true;

                    if(tam2 > 2)
                    {
                        viavel2 = Movimentos_Paradas::criaRota(instancia, vetCliente2, tam2, peso2, tipo2, &combustivel2, &poluicao2, NULL, NULL, vetLimiteTempo, vetClienteAux);

                    }
                    double poluicaoHeur2 = poluicao2;

                    if(viavel2)
                    {

                        solucaoInteira.inicializaInicio();

                        bool resultadoModelo;

                        if(tam2 > 2)
                            resultadoModelo = modelo->criaRota(vetCliente, tam, tipo, peso, instancia, &poluicao, &combustivel, clientesTrocados, vetRotas, vetCliente2, tam2, tipo2, peso2,
                                                               &poluicao2, &combustivel2, vetRotas2);
                        else
                            resultadoModelo = modelo->criaRota(vetCliente, tam, tipo, peso, instancia, &poluicao, &combustivel, clientesTrocados, vetRotas, NULL, -1, false, -1,
                                                               NULL, NULL, NULL);

                        if (resultadoModelo)
                        {
                            Solucao::ClienteRota *ptr_cliente;
                            int tamAux, tipoAux, pesoAux;

                            double poluicaoAux, combustivelAux, poluicaoHeurAux;

                            for(int h = 0; h < 2; ++h)
                            {

                                if(h == 0)
                                {
                                    ptr_cliente = vetCliente;
                                    tamAux = tam;
                                    tipoAux = tipo;

                                    pesoAux = peso;
                                    poluicaoAux = poluicao;
                                    combustivelAux = combustivel;
                                    poluicaoHeurAux = poluicaoHeur;
                                }
                                else
                                {
                                    if(tam2 <= 2)
                                        break;

                                    ptr_cliente = vetCliente2;
                                    tamAux = tam2;
                                    tipoAux = tipo2;

                                    pesoAux = peso2;
                                    poluicaoAux = poluicao2;
                                    combustivelAux = combustivel2;
                                    poluicaoHeurAux = poluicaoHeur2;
                                }

                                Solucao::Veiculo *veiculo = new Solucao::Veiculo(tipo);

                                for (auto cliente : veiculo->listaClientes)
                                {
                                    delete cliente;
                                }


                                for (int i = 0; i < 2; ++i)
                                    veiculo->listaClientes.pop_front();

                                for (int i = 0; i < tamAux; ++i)
                                {
                                    Solucao::ClienteRota *clienteRota = new Solucao::ClienteRota;
                                    clienteRota->swap(&ptr_cliente[i]);

                                    veiculo->listaClientes.push_back(clienteRota);
                                }

                                veiculo->carga = pesoAux;
                                veiculo->combustivel = combustivelAux;
                                veiculo->poluicao = poluicaoAux;

                                string erro = "";


                                bool resultado = VerificaSolucao::verificaVeiculoRotaMip(veiculo, instancia, NULL,
                                                                                         &erro);
                                if (resultado)
                                    cout << "Veiculo correto\n";
                                else
                                {
                                    cout << "Erro veiculo "<<h<<" . verificao falhou\nErro: " << erro << '\n';


                                }

                                delete veiculo;

                                if (resultado)
                                {

                                    cout << "\nRota: ";

                                    for (int i = 0; i < tamAux; ++i)
                                        cout << ptr_cliente[i].cliente << ' ';

                                    cout << '\n';

                                    cout << "Combustivel: " << combustivelAux << '\n';
                                    cout << "Polucao: " << poluicaoAux << '\n';
                                    cout << "Polucao heuristica: " << poluicaoHeurAux << '\n';
                                    cout << "Tipo: " << tipo << "\n\n\n";
                                }
                            }

                        } else
                        {
                            cout << "Modelo errada!!\n";
                        }

                    }else
                        cout<<"Rota heuristica2 eh inviavel\n";
                }
                else
                    cout<<"Rota heuristica1 eh inviavel";

                cout<<"\n\n***********************************************************************\n\n";

            }

        }while (tipo != -1);



        delete instancia;
        delete modelo;
        delete []vetCliente;
        delete []vetClienteAux;
        delete []vetLimiteTempo;
        delete []vetCliente2;


    } catch (GRBException e)
    {
        cout<<"Erro code: "<<e.getErrorCode()<<"\nmessage: "<<e.getMessage()<<'\n';

        delete instancia;
        delete modelo;

        exit(-1);
    }

    return 0;
}

#elif Opcao == VerificaSol

int main(int num, char **args)
{
    const string Diretorio = "/home/igor/Documentos/matPlotLib/Testes/08_07_20_Grasp/resultados_07_07_2020_14_45_01/resultados_07_07_2020_14_56_19/resultadosCompletos/";
    string strInstancia;
    string instanciaNome;




    if (num < 2)
    {
        cout << "Numero incorreto de parametros.\n";
        exit(-1);
    }


    GRBEnv env;
    env.set(GRB_IntParam_OutputFlag, 0);



    for(int posicao = 1; posicao < num; ++posicao)
    {


        strInstancia = args[posicao];


        ofstream file;

        for (int i = 0; i < strInstancia.length(); ++i)
        {
            if (strInstancia[i] != '/')
            {

                if (strInstancia[i] == '.')
                    break;
                instanciaNome += strInstancia[i];

            } else
                instanciaNome = "";

        }

        cout<<instanciaNome<<'\n';

        string texto;


        Instancia::Instancia *instancia = new Instancia::Instancia(strInstancia);
        Modelo::Modelo *modelo;

        try
        {



            GRBModel grb_modelo = GRBModel(env);

            modelo = new Modelo::Modelo(instancia, &grb_modelo, true);


            Solucao::ClienteRota *vetCliente = new Solucao::ClienteRota[MaxTamVetClientesMatrix];
            vetCliente[0].cliente = 0;

            int tipo, tam, peso, cliente = 1;
            long double combustivel, poluicao;

            for (int execucao = 0; execucao < 10; ++execucao)
            {
                const string execucaoStr = Diretorio + instanciaNome + '_' + std::to_string(execucao) + ".txt";
                std::ifstream file;

                file.open(execucaoStr, ios_base::out);

                if (!file.is_open())
                {
                    cout << "Erro ao abrir arquivo " << execucaoStr << '\n';
                    exit(-1);
                }

                string lixo;

                for (int i = 0; i < 6; ++i)
                {
                    getline(file, lixo);
                }

                int aux;

                file >> aux;

                const int NumRotas = instancia->numVeiculos;



                getline(file, lixo);
                //cout<<lixo<<"||\n";

                for (int rota = 0; rota < NumRotas; ++rota)
                {


                    //lixo = "";
                    //getline(file, lixo);
                    tipo = rota % 2;

                    //cout<<lixo<<"||\n";

                    file >> vetCliente[0].cliente;
                    file >> vetCliente[0].cliente;

                    tam = cliente = 1;

                    peso = 0;
                    combustivel = poluicao = 0.0;

                    do
                    {

                        file >> cliente;

                        ++tam;

                        vetCliente[tam - 1].cliente = cliente;
                        peso += instancia->vetorClientes[cliente].demanda;

                    } while (cliente);

                    if(tam <= 2)
                        continue;
                    int resultado = modelo->criaRota(vetCliente, tam, tipo, peso, instancia, &poluicao, &combustivel);

                    if (resultado == 0)
                    {

                        cout << "Rota errada!!\n";
                        cout<<"Execucao: "<<execucao<<'\n';

                        for(int i = 0; i < tam; ++i)
                            cout<<vetCliente[i].cliente<<' ';

                        cout<<'\n';

                        exit(-1);
                    }

                    if(resultado == 1)
                    {
                        Solucao::Veiculo veiculo(tipo);

                        veiculo.combustivel = combustivel;
                        veiculo.poluicao = poluicao;
                        veiculo.carga = peso;

                        auto it = veiculo.listaClientes.begin();
                        auto itF = it++;


                        delete *it;
                        veiculo.listaClientes.pop_back();

                        delete *itF;
                        veiculo.listaClientes.pop_back();


                        Solucao::ClienteRota *clienteRota = NULL;

                        for (int j = 0; j < tam; ++j)
                        {
                            clienteRota = new Solucao::ClienteRota;

                            clienteRota->cliente = vetCliente[j].cliente;

                            clienteRota->tempoChegada = vetCliente[j].tempoChegada;
                            clienteRota->tempoSaida = vetCliente[j].tempoSaida;

                            for (int k = 0; k < instancia->numPeriodos; ++k)
                            {

                                clienteRota->tempoPorPeriodo[k] = vetCliente[j].tempoPorPeriodo[k];
                                clienteRota->distanciaPorPeriodo[k] = vetCliente[j].distanciaPorPeriodo[k];
                                clienteRota->percorrePeriodo[k] = vetCliente[j].percorrePeriodo[k];

                            }

                            veiculo.listaClientes.push_back(clienteRota);
                            clienteRota = NULL;

                        }

                        string erro;

                        if (!VerificaSolucao::verificaVeiculoRotaMip(&veiculo, instancia, NULL, &erro))
                        {
                            cout << "ERRO\nInstancia: " << instanciaNome << "\nExecucao: " << execucao << "\n";
                            cout << "Rota: " << veiculo.getRota() << "\n\nMotivo: " << erro << "\n\n";

                            cout<<"tempoSaida 0: "<<vetCliente[0].tempoSaida<<'\n';
                            cout<<"tempoChegada 1: "<<vetCliente[1].tempoChegada<<'\n';
                            cout<<"tempoSaida 1: "<<vetCliente[1].tempoSaida<<'\n';
                            cout<<"tempoChegada 0: "<<vetCliente[2].tempoChegada<<'\n';

                            for(int i = 1; i < tam; ++i)
                            {
                                cout<<vetCliente[i-1].cliente<<' '<<vetCliente[i].cliente<<" : ";

                                for(int k = 0; k < 5; ++k)
                                {
                                    if(vetCliente[i].percorrePeriodo[k])
                                        cout<<k<<' ';
                                }
                                cout<<"\n";
                            }

                            delete modelo;
                            delete[]vetCliente;
                            delete instancia;

                            exit(-1);
                        }



                    }


                }

                file.close();
            }

            delete instancia;
            delete modelo;
            modelo = NULL;

        } catch (GRBException e)
        {
            cout<<"Erro Code : "<<e.getErrorCode()<<'\n';
            cout<<"Mensagem: "<<e.getMessage()<<'\n';

            exit(-1);

        }



    }


    cout<<"Passou em todos os testes!!\n";

    return 0;

}
#endif
