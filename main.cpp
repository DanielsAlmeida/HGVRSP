#include <iostream>
#include <iomanip>
#include <fstream>
#include "Instancia.h"
#include "Solucao.h"
#include "VerificaSolucao.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "time.h"

/*
 * Debug memória : g++ *.cpp -Wall -fsanitize=address -g
 */

using namespace std;

bool comparadorFimJanela(Instancia::Cliente &cliente1, Instancia::Cliente &cliente2)
{
    return cliente1.fimJanela < cliente2.fimJanela;

}

bool comparadorDistanciaDep(Instancia::Cliente &cliente1, Instancia::Cliente &cliente2)
{

    return cliente1.distanciaDeposito < cliente2.distanciaDeposito;

}

bool comparadorDistFimJanela(Instancia::Cliente &cliente1, Instancia::Cliente &cliente2)
{
    return (cliente1.distanciaDeposito * 0.001*cliente1.periodoFimJanela) < (cliente2.distanciaDeposito* 0.001*cliente2.periodoFimJanela);

}

int main(int num, char **agrs)
{
    string strInstancia;
    string saidaCompleta;
    string saidaParcial;
    string instanciaNome;


    if(num != 4 && num != 1)
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
    //uint32_t semente  = 1573480624;

    string texto;
    std::time_t result = std::time(nullptr);
    texto += std::asctime(std::localtime(&result));
    texto += '\n';

    texto += "Nome: " + instanciaNome + "\n\n";

    seed(semente);

    texto += "Semente: " + std::to_string(semente) + "\n\n";

    Instancia::Instancia *instancia = new Instancia::Instancia(strInstancia);



    auto vet = instancia->vetorClientes;

    float vetAlfas[14] = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7};

    clock_t c_start = clock();

    auto *solucao = Construtivo::reativo(instancia, comparadorFimJanela, vetAlfas, 14, 1000, 100);

    clock_t c_end = clock();

    cout << fixed << setprecision(2);

    texto += std::to_string(solucao->vetorVeiculos.size()) + "\n\n";

    for(auto veiculo : solucao->vetorVeiculos)
    {

        texto += std::to_string(veiculo->listaClientes.size()-2) + '\n';
        for (auto it : (*veiculo).listaClientes)
        {
            texto += std::to_string((*it).cliente) + " ";
        }

        texto += "\n\n";



    }


    for(auto veiculo : solucao->vetorVeiculos)
    {

        for(auto cliente : (*veiculo).listaClientes)
        {

            if(cliente->cliente != 0)
                texto += std::to_string(cliente->cliente) += " " + std::to_string(cliente->tempoSaida) + " " + std::to_string(cliente->tempoSaida) + "\n";

        }

    }
    texto += '\n';

    bool verificao = VerificaSolucao::verificaSolucao(instancia, solucao, &texto);
    texto += '\n';



    for(int i = 1; i < instancia->numClientes; ++i)
    {
        texto += std::to_string(vet[i].cliente) += " " + std::to_string(vet[i].inicioJanela) + " " + std::to_string(vet[i].fimJanela) + "\n";
    }

    texto += "-1\n\n\n";

    string tempo;

    tempo += "Tempo cpu: " + std::to_string((1000.0*c_end-c_start) / CLOCKS_PER_SEC/1000.0) + " S\n";
    tempo += "Verificacao: " + std::to_string(verificao) + "\n";
    tempo += "Poluicao: " + std::to_string(solucao->poluicao) + '\n';

    texto += tempo;

    if(solucao->poluicao <= 0.1)
        cout<<"Poluicao = 0\n";

    if(num == 1)
    {
        cout<<"\n\n"<<tempo<<'\n';
    }
    else
    {

        file.open(saidaCompleta, ios::out);
        file << texto;

        file.close();

        file.open(saidaParcial, ios::out | ios::app);
        file << std::to_string(solucao->poluicao) << " " << ((1000.0 * c_end - c_start) / CLOCKS_PER_SEC / 1000.0)
             << '\n';

        file.close();

    }

    delete solucao;
    delete instancia;

    return 0;
}
