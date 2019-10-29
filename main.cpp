#include <iostream>
#include "Instancia.h"
#include "Solucao.h"
#include "VerificaSolucao.h"
#include "Construtivo.h"

using namespace std;

bool comparador(Instancia::Cliente &cliente1 , Instancia::Cliente &cliente2)
{
    return cliente1.fimJanela < cliente2.fimJanela;

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

    Instancia::Instancia *instancia = new Instancia::Instancia("/home/igor/Documentos/HGVRSP/instanciasUK/UK_10x5_5.dat");

    auto *solucao = Construtivo::geraSolucao(instancia, comparador);

    for(auto veiculo : solucao->vetorVeiculos)
    {


        for (auto it : (*veiculo).listaClientes)
        {
            cout << (*it).cliente << " ";
        }

        cout << endl;

    }

    bool verificao = VerificaSolucao::verificaSolucao(instancia, solucao);

    cout<<"\nVerificacao: "<<verificao<<endl;

    delete solucao;
    delete instancia;

    return 0;
}