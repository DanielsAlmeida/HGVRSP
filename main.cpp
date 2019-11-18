#include <iostream>
#include <iomanip>
#include "Instancia.h"
#include "Solucao.h"
#include "VerificaSolucao.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "time.h"

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

int main()
{

    /*
    Instancia::Instancia *instancia = new Instancia::Instancia("/home/igor/Documentos/HGVRSP/instanciasUK/UK_10x5_2.dat");

    auto *solucao = new Solucao::Solucao(instancia->numClientes);
    float distancia;
    int periodoSaida;
    Solucao::ClienteRota *cliente;
    float horaPartida, velocidade, horario;



        cliente = new Solucao::ClienteRota{0, 0, 0, 0, 0};
        solucao->vetorVeiculos[0]->listaClientes.push_back(cliente);

        cliente = new Solucao::ClienteRota;

        cliente->cliente = 1;
        cliente->tempoChegada = 0.587857143;

        cliente->poluicao = VerificaSolucao::calculaPoluicao(70.0,0.587857143, instancia );
        cliente->combustivel = cliente->poluicao;
        cliente->tempoSaida = instancia->vetorClientes[1].tempoServico + instancia->vetorClientes[1].inicioJanela ;
        float tempoSaida = cliente->tempoSaida;
        solucao->vetorVeiculos[0]->poluicao = cliente->poluicao;
        solucao->vetorVeiculos[0]->combustivel = cliente->combustivel;

        solucao->vetorVeiculos[0]->listaClientes.push_back(cliente);

//*************************  0 -> 1 -> *******************************************************************************************

        cliente = new Solucao::ClienteRota;
        cliente->cliente = 2;
        cliente->tempoChegada = 1.634;
        cliente->tempoSaida  = 2.084;
        cliente->poluicao = VerificaSolucao::calculaPoluicao(82.0, 0.634, instancia);
        cliente->combustivel = cliente->poluicao;

        solucao->vetorVeiculos[0]->poluicao += cliente->poluicao;
        solucao->vetorVeiculos[0]->combustivel += cliente->combustivel;


        solucao->vetorVeiculos[0]->listaClientes.push_back(cliente);

        solucao->vetorVeiculos[0]->carga = instancia->vetorClientes[1].demanda + instancia->vetorClientes[2].demanda;


//*************************** 0->1->2->*******************************************************************************************

    cliente = new Solucao::ClienteRota;
    cliente->cliente = 3;
    cliente->tempoChegada = 3.163;
    cliente->tempoSaida = 3.513;
    cliente->poluicao = VerificaSolucao::calculaPoluicao(57.0, 1.079, instancia);
    cliente->combustivel = cliente->poluicao;

    solucao->vetorVeiculos[0]->poluicao += cliente->poluicao;
    solucao->vetorVeiculos[0]->combustivel += cliente->combustivel;


    solucao->vetorVeiculos[0]->listaClientes.push_back(cliente);

    solucao->vetorVeiculos[0]->carga += instancia->vetorClientes[3].demanda;

//*************************** 0->1->2->3->*******************************************************************************************
    cliente = new Solucao::ClienteRota;
    cliente->cliente = 0;
    cliente->tempoChegada = 5.425;

    cliente->poluicao = VerificaSolucao::calculaPoluicao(53.0, 0.087, instancia);
    cliente->poluicao += VerificaSolucao::calculaPoluicao(27.0, 1.8, instancia);
    cliente->poluicao += VerificaSolucao::calculaPoluicao(42.0, 0.025214286, instancia);

    cliente->combustivel = cliente->poluicao;

    solucao->vetorVeiculos[0]->poluicao += cliente->poluicao;
    solucao->vetorVeiculos[0]->combustivel += cliente->combustivel;


    solucao->vetorVeiculos[0]->listaClientes.push_back(cliente);

    bool verificao = VerificaSolucao::verificaSolucao(instancia, solucao);

    cout<<verificao<<endl;







    delete solucao;
    delete instancia;

    */

    //auto semente  = time(NULL);
    uint32_t semente  = 1573480624;
    seed(semente);

    cout<<"Semente: "<<semente<<endl;

    Instancia::Instancia *instancia = new Instancia::Instancia("/home/igor/Documentos/HGVRSP/instanciasUK/UK_10x5_10.dat");

    cout<<"velocidade 0-10 "<<instancia->matrizVelocidade[0][10][0]<<endl;


    auto vet = instancia->vetorClientes;

    cout<<"\nNo Inicio Fim\n";

    for(int i = 1; i < instancia->numClientes; ++i)
    {
        cout<<vet[i].cliente<<" "<<vet[i].inicioJanela<<" "<<vet[i].fimJanela<<"\n";
    }

    cout<<"\n\n";

    //auto *solucao = Construtivo::geraSolucao(instancia, comparadorFimJanela, 0.5, nullptr, nullptr);//18.5

    float vetAlfas[14] = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7};

    auto *solucao = Construtivo::reativo(instancia, comparadorFimJanela, vetAlfas, 14, 100, 10);

    cout << fixed << setprecision(2);

    cout<<"\t\t\t(combustivel, poluicao)\n\n";

    for(auto veiculo : solucao->vetorVeiculos)
    {


        for (auto it : (*veiculo).listaClientes)
        {
            cout << (*it).cliente << " ";
        }

        cout<<" ("<<(*veiculo).combustivel<<", "<<(*veiculo).poluicao<<")\n";



    }


    cout<<"\n("<<solucao->poluicao<<")\n";


    //int *horaSaida = new int[instancia->numClientes];
    //horaSaida[0] = 0;

    cout<<"\n\nNO TEMPO CHEGADA TEMPO SAIDA\n";

    for(auto veiculo : solucao->vetorVeiculos)
    {

        for(auto cliente : (*veiculo).listaClientes)
        {

            if((*cliente).cliente == 0)
                continue;

            //horaSaida[cliente->cliente] = instancia->retornaPeriodo(cliente->tempoSaida);

            cout<<cliente->cliente<<" "<<cliente->tempoChegada<<" "<<cliente->tempoSaida<<"\n";


        }
    }

    cout<<"\n\n\n";

    bool verificao = VerificaSolucao::verificaSolucao(instancia, solucao);

    cout<<"\nVerificacao: "<<verificao<<endl;

    delete solucao;
    delete instancia;

    //for(int i = 0; i < (instancia->numClientes); ++i)
        //cout<<horaSaida[i]<<endl;


    //delete []horaSaida;


    return 0;
}

// (13.21)