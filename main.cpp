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
//  1588722899

//1586725703
#define Saida false
#define TesteParametro false

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

#if not TesteParametro
int main(int num, char **agrs)
{
    //mtrace();


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

    #define numAlfas  18
    float vetAlfas[numAlfas] = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9};
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

    auto c_start = std::chrono::high_resolution_clock::now();

    auto *solucao = Construtivo::grasp(instancia, vetAlfas, numAlfas, 1000, 100, logAtivo, &strLog, vetHeuristicas, TamVetH, vetParametro, vetEstatisticaMv,
                                       matrixClienteBest, &tempoCriaRota, vetCandInteracoes, vetLimiteTempo);

    auto c_end = std::chrono::high_resolution_clock::now();



    delete []vetCandInteracoes;

    //desaloca matrix
    for(int i = 0; i < instancia->numClientes; ++i)
        delete []matrixClienteBest[i];

    delete []matrixClienteBest;

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

    if(!solucao->veiculoFicticil)
        Veificacao = VerificaSolucao::verificaSolucao(instancia, solucao, &texto, &distanciaTotal);

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
    tempo << "Distancia total: " << (distanciaTotal);
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

        int numVeiculosUsados = 0;


        for(auto veiculo : solucao->vetorVeiculos)
        {

            if(veiculo->listaClientes.size() > 2)
            {
                ++numVeiculosUsados;

            }

        }

        for(int i = 0; i < 9 ; ++i)
        {
            if(vetEstatisticaMv[i].num > 0)
            {
                vetEstatisticaMv[i].poluicao /= vetEstatisticaMv[i].num;
                vetEstatisticaMv[i].gap /= vetEstatisticaMv[i].num;
                vetEstatisticaMv[i].tempo /= vetEstatisticaMv[i].numTempo;
            }
        }

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

        cout<<std::fixed << std::setprecision(4);

        for(int i = 0; i < 8 ; ++i)
            cout<<vetEstatisticaMv[i].poluicao<<" ("<<vetEstatisticaMv[i].tempo<<")    ";

        cout<<"\n\n";



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

    //muntrace();

    return 0;
}


# else
 int main(int num, char **agrs)
{

    string strInstancia;
    string saidaCompleta;
    string saidaParcial;
    string instanciaNome;
    string log;

    bool logAtivo = false;

    if(num != 6)
    {
        cout<<"Numero incorreto de parametros.\n Num "<<num<<'\n';
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


    auto semente  = std::stoul(agrs[3]);


    string texto;
    std::time_t result = std::time(nullptr);
    auto data = std::asctime(std::localtime(&result));
    texto += data;
    texto += '\n';

    texto += "Nome: " + instanciaNome + "\n\n";

    seed(semente);

    texto += "Semente: " + std::to_string(semente) + "\n\n";

    Instancia::Instancia *instancia = new Instancia::Instancia(strInstancia);

    instancia->getClientes();



    auto vet = instancia->vetorClientes;

    #define numAlfas  18
    float vetAlfas[numAlfas] = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9};





    std::stringstream strLog;
    string logAux;

    clock_t c_start = clock();

    const double parametro = std::atof(agrs[5]);

     const double vetParametro[7] = {parametro, parametro, parametro, parametro, parametro, parametro, parametro};

    auto *solucao = Construtivo::grasp(instancia, vetAlfas, numAlfas, 1000, 100, logAtivo, &strLog, Heuristica_6, vetParametro);



    clock_t c_end = clock();


    cout<<solucao->numSolucoesInv<<'\n';

    delete solucao;
    delete instancia;

    return 0;
}

#endif
