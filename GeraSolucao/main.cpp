#include<iostream>
#include "Instancia.h"
#include "Solucao.h"
#include "time.h"
#include <iomanip>
#include <fstream>
#include "VerificaSolucao.h"

using namespace std;

void criaSolucao(string rota, Solucao::Solucao *solucao, Instancia::Instancia *instancia);

int main( int tam, char **agrs)
{
    if(tam != 4)
    {
        cout<<"Numero incorreto de parametros.\n";
        cout<<"Formato: \n\n./GERA_SOLUCAO \"instancia.txt\" \"rota.txt\" \"saida.txt\"\n\n";
    }

    string arquivo = agrs[1];
    string rota = agrs[2];
    string nomeSaida = agrs[3];

    Instancia::Instancia instancia(arquivo);

    fstream saida;
    saida.open(nomeSaida, std::ios::out);

    if(!saida.is_open())
    {
        cout<<"Nao foi possivel abri o arquivo: "<<nomeSaida<<'\n';
        exit(-1);
    }

    string tempoRota;

    auto *solucao = new Solucao::Solucao(instancia.numVeiculos);

    criaSolucao(rota, solucao, &instancia);
    
    saida << std::fixed << setprecision(2);
    string texto;

    
    VerificaSolucao::geraSolucao(&instancia, solucao, &texto);
   
    double tempoViagem = 0.0;
    double inicio;
    bool fim = false;

    string tempoChegadaSaida = "";
    
    if(solucao->veiculoFicticil)
        saida<<"-1\n";
    else
        saida<<"1\n";
        

    saida<<"\n\n\n\n\n\n"<<solucao->vetorVeiculos.size()<<"\n\n";
    for(auto veiculo : solucao->vetorVeiculos)
    {   
        saida<<veiculo->listaClientes.size()-2<<'\n';
        for (auto it : (*veiculo).listaClientes)
        {
            saida<<(*it).cliente<<' ';

            if(it->cliente != 0)
                tempoChegadaSaida += std::to_string(it->cliente) + ' ' + std::to_string(it->tempoChegada) + ' ' + std::to_string(it->tempoSaida) + '\n';
        }
        saida<<"\n\n";

    }

    saida<<tempoChegadaSaida;

    saida<<'\n'<<texto<<"-1\n\n";

    for(int i = 1; i < instancia.numClientes; ++i)
        saida<<i<<' '<<instancia.vetorClientes[i].inicioJanela<<' '<<instancia.vetorClientes[i].fimJanela<<'\n';
    
    saida<<"-1\n";
    
    saida<<"Demanda:\n\n";
    
    for(int i = 1; i < instancia.numClientes; ++i)
        saida<<i<<' '<<instancia.vetorClientes[i].demanda<<"\n";
    
    cout<<"\n\n";
    
    int i = 0;
    cout<<"Veiculo  poluicao  combustivel\n";
    for(auto it : solucao->vetorVeiculos)
    {
    
        cout<<i<<'\t'<<it->poluicao<<'\t'<<it->combustivel<<'\n';
        i += 1;
        
           
    }
    
    saida.close();

    cout<<"Poluicao: "<<solucao->poluicao<<'\n';

    for(auto veiculo : solucao->vetorVeiculos)
    {

        if(veiculo->tipo == 2)
            continue;

        int clienteInt;
        double gap = HUGE_VALF;
        double aux;
        double ultimo;

        for(auto cliente : veiculo->listaClientes)
        {
            cout<<cliente->cliente<<' ';

            aux = instancia.vetorClientes[cliente->cliente].fimJanela - cliente->tempoSaida;

            if(aux < gap)
            {
                gap = aux;
                clienteInt = cliente->cliente;
            }

            if(cliente->cliente != 0)
                ultimo = cliente->tempoSaida;
        }

        cout<<"    "<<"Cliente: "<<clienteInt<<" menor gap: "<<gap<<";  Tempo saida ultimo cliente: "<<ultimo<<'\n';

    }

    delete solucao;
}

void criaSolucao(string rota, Solucao::Solucao *solucao, Instancia::Instancia *instancia)
{

    std::ifstream file;
    file.open(rota, std::ios::out);

    if(file.is_open())
    {
        int ficticil;
        int numRotas;
        
        file>>ficticil;
        file>>numRotas;

       

        solucao->veiculoFicticil = ficticil;

        if(ficticil == 1)
            solucao->vetorVeiculos.push_back(new Solucao::Veiculo(2));

        
        

        for(auto it : solucao->vetorVeiculos)
        {
            list<Solucao::ClienteRota*> *lista = &it->listaClientes;
            lista->pop_back();

            int tamLista, cliente;

            file>>tamLista;
            file>>cliente;


            for(int i = 0; i < tamLista+1; ++i)
            {
                file>>cliente;
  
                Solucao::ClienteRota *clienteRota = new Solucao::ClienteRota;
                clienteRota->cliente = cliente;

                lista->push_back(clienteRota);
                it->carga += instancia->vetorClientes[cliente].demanda;

            }

          
        }

    }
    else
    {
        std::cout<<"Nao foi possivel abrir o arquivo: "<<rota<<'\n';
        exit(-1);
    }
    
}




