//
// Created by igor on 17/10/19.
//

#include "VerificaSolucao.h"

using namespace VerificaSolucao;

bool verificaSolucao(Instancia::Instancia *instancia, Solucao::Solucao *solucao)
{

    int *vetorClientes = new int[instancia->numClientes]; //Vetor para checar se cada cliente foi visitado uma unica vez.

    int carga = 0;
    float combustivel;

    for(auto it : solucao->vetorVeiculos)
    {
        carga = 0;
        combustivel = 0.0;
        std::list<Solucao::ClienteRota*>::iterator iterator;

        for(auto itCliente = it->listaClientes.begin(); itCliente != it->listaClientes.end(); )
        {

            iterator = itCliente;
            ++itCliente;

            if(itCliente == it->listaClientes.end())
                break;





        }
    }

}

float horaChegada(int clientePartida, int clienteDestino, float horaPartida, Solucao::Solucao *solucao, Instancia::Instancia *instancia)
{

    int periodoSaida;
    float distancia, velocidade, tempoRestantePeriodo, horario;
    distancia = instancia->matrizDistancias[clientePartida][clienteDestino];
    float horaChegada;

    do
    {

        periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]
        velocidade = instancia->matrizVelocidade[clientePartida][clienteDestino][periodoSaida];//velocidade -> km/h
        tempoRestantePeriodo;
        horario = horaPartida + (distancia / velocidade); //Horario de chegada considerando somente uma velocidade.//Horario em horas


        if(instancia->retornaPeriodo(horario) != periodoSaida)//Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
        {
            //Percorreu todo o periodoSaida e não chegou ao destino.

            tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;

            distancia -= tempoRestantePeriodo * velocidade;

            horaPartida = instancia->vetorPeriodos[periodoSaida].fim;

        }
        else//horario é do mesmo periodo de periodoSaida
        {
            distancia = 0;
            horaChegada = horario;

        }

    }while(distancia != 0.0);

    return horaChegada;


}
