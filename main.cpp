#include <iostream>
#include <iomanip>
#include <fstream>
#include "Instancia.h"
#include "Solucao.h"
#include "VerificaSolucao.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "time.h"

#define saida false
// /home/igor/Documentos/HGVRSP/instanciasUK/UK_10x5_2.dat /home/igor/Documentos/HGVRSP/saidaCompleta.txt /home/igor/Documentos/HGVRSP/saidaParcial.txt
/*
 * Debug memória : g++ *.cpp -Wall -fsanitize=address -g
 *
 * date '+%d_%m_%Y_%k-%M-%S'
 *
 * Arquivo de log: /home/igor/Documentos/HGVRSP/saidaLog.txt
 * 
 * Instancia 10 clientes, parametro: 0.12
 * Instancia 15 clientes, parametro: 0.99
 * Instancia 20 clientes, parametro: 0.67
 * Instancia 25 clientes, parametro: 0.54
 * Instancia 50 clientes, parametro: 0.79
 * Instancia 75 clientes, parametro: 0.82 
 */

using namespace std;

int main(int num, char **agrs)
{
    std::map<int, float> parametro;
    parametro.insert({75, 0.82});
    parametro.insert({50, 0.79});
    parametro.insert({25, 0.54});
    parametro.insert({20, 0.67});
    parametro.insert({15, 0.99});
    parametro.insert({10, 0.12});

    string strInstancia;
    string saidaCompleta;
    string saidaParcial;
    string instanciaNome;
    string log;

    bool logAtivo = false;

    if(num != 4 && num != 1 && num != 5)
    {
        cout<<"Numero incorreto de parametros.\n";
        exit(-1);
    }

    if(num == 1)
    {
        strInstancia = "/home/igor/Documentos/HGVRSP/instanciasUK/UK_15x5_10.dat";
    }
    else
    {
        strInstancia = agrs[1];
        saidaCompleta = agrs[2];
        saidaParcial = agrs[3];

        if(num == 5)
        {
            log = agrs[4];
            logAtivo = true;

        }

    }


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

    //uint32_t semente = 1576259993;

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

    auto *solucao = Construtivo::reativo(instancia, vetAlfas, numAlfas, 1000, 100, logAtivo,
                                         &strLog, parametro[instancia->numClientes - 1]);



    clock_t c_end = clock();

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

        texto += "\n\n";
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

    bool Veificacao = VerificaSolucao::verificaSolucao(instancia, solucao, &texto, &distanciaTotal);
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


    tempo << "Tempo cpu: " << ((1000.0*c_end-c_start) / CLOCKS_PER_SEC/1000.0) << " S\n";
    tempo << "Verificacao: " <<(Veificacao) << "\n";
    tempo << "Poluicao: " <<(solucao->poluicao + solucao->poluicaoPenalidades) << '\n';
    tempo << "Ultima atualizacao: " << (solucao->ultimaAtualizacao) << '\n';
    tempo << "Numero de solucoes inviaveis: " << (solucao->numSolucoesInv) << '\n';
    tempo << "Tempo total de viagem: " << (tempoViagem*60.0) << '\n';
    tempo << "Distancia total: " << (distanciaTotal);
    texto += tempo.str();

    //if(solucao->poluicao <= 0.1)
    //    cout<<"Poluicao = 0\n";

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
                ++numVeiculosUsados;

        }


            // Poluicao (kg), tempo cpu (SEC), ultima atualizacao, numero de solucoes inviaveis, tempo total de viagem (min), distancia total (km).

        if(Veificacao)
            file << std::to_string(solucao->poluicao) << " " << ((1000.0 * c_end - c_start) / CLOCKS_PER_SEC / 1000.0) <<
            " " <<std::to_string(instancia->numVeiculos)<<" " << std::to_string(numVeiculosUsados) << " "<<std::to_string(solucao->numSolucoesInv) << '\n';

        else
            file << std::to_string(0.0) << " " <<((1000.0 * c_end - c_start) / CLOCKS_PER_SEC / 1000.0)<<
                 " " <<std::to_string(instancia->numVeiculos)<<" " << std::to_string(numVeiculosUsados) <<" "<<std::to_string(solucao->numSolucoesInv) << '\n';

        file.close();

    }

    #if saida

        cout<<"Instancia: "<<instanciaNome<<'\n';
        cout<<tempo.str()<<'\n';
        cout<<"Semente: "<<semente<<'\n';


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

    delete solucao;
    delete instancia;

    return 0;
}



/************************************************************************************************************************************************************************************
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

    //uint32_t semente = 1576259993;

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

    auto *solucao = Construtivo::reativo(instancia, vetAlfas, numAlfas, 1000, 100, logAtivo,
                                         &strLog, parametro);



    clock_t c_end = clock();


    cout<<solucao->numSolucoesInv<<'\n';

    delete solucao;
    delete instancia;

    return 0;
}
************************************************************************************************************************************************************************************************************/

