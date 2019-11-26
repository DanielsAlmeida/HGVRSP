//
// Created by igor on 17/10/19.
//Corrigido 26/11/19

#include "VerificaSolucao.h"

using namespace VerificaSolucao;

bool VerificaSolucao::verificaSolucao(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, string *texto)
{

    std::string saida;
    int *vetorClientes = new int[instancia->numClientes]; //Vetor para checar se cada cliente foi visitado uma unica vez.

    for(int i = 0; i < instancia->numClientes; ++i)
        vetorClientes[i] = 0;



    int carga = 0, cargaTotal = 0;
    double combustivel;

    int periodoSaida, periodo;
    double distancia, velocidade, tempoRestantePeriodo, horario, horaChegada, horaPartida;
    std::list<Solucao::ClienteRota*>::iterator iterator;

    double poluicaoAux, poluicao, aux;
    double combustivelAux;

    //No No PERIODO,TEMPO,DISTANCIA,POLUICAO, Velocidade

    for(auto it : solucao->vetorVeiculos)//Percorre os veiculos da solução
    {

        carga = 0;
        poluicao = 0.0;
        combustivel = 0.0;
        cargaTotal = it->carga;




        for(auto itCliente = it->listaClientes.begin(); itCliente != it->listaClientes.end(); )//Percorre os clientes do veículo
        {

            iterator = itCliente;
            ++itCliente;

            poluicaoAux =  0.0;
            combustivelAux = 0.0;

            if(itCliente == it->listaClientes.end())//verifica se itCliente é igual a NULL
                break;


            distancia = instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente];

            if(distancia == 0.0)
            {
                delete []vetorClientes;
                return false;
            }

            horaPartida = (*iterator)->tempoSaida;

            carga += instancia->vetorClientes[(*itCliente)->cliente].demanda;

            saida+= std::to_string((*iterator)->cliente) + " " + std::to_string((*itCliente)->cliente) + " ";

            periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]

            do
            {

                velocidade = instancia->matrizVelocidade[(*iterator)->cliente][(*itCliente)->cliente][periodoSaida];//velocidade -> km/h
                horario = horaPartida + (distancia / velocidade); //Horario de chegada considerando somente uma velocidade. Horario em horas


                if(!(*itCliente)->percorrePeriodo[periodoSaida])
                {
                    horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;
                    periodoSaida += 1;

                    if(periodoSaida >= 5 )
                    {
                        delete []vetorClientes;
                        return false;
                    }
                }
//Aqui.
                else if((instancia->retornaPeriodo(horario) != periodoSaida))//Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
                {
                    // percorreu todo o periodoSaida e não chegou ao destino.

                    tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;
                    distancia -= tempoRestantePeriodo * velocidade;
                    horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;
                    aux = poluicaoRota(instancia, it->tipo, tempoRestantePeriodo*velocidade, (*iterator)->cliente, (*iterator)->cliente, periodoSaida);
                    poluicaoAux += aux;

                    poluicaoAux += combustivelRota(instancia, it->tipo, tempoRestantePeriodo*velocidade, (*iterator)->cliente, (*iterator)->cliente, periodoSaida);

                    saida += std::to_string(periodoSaida) + "," + std::to_string(tempoRestantePeriodo) + "," + std::to_string(tempoRestantePeriodo * velocidade) + "," + std::to_string(aux) + "," + std::to_string(velocidade) +",";
                    saida += std::to_string(0) + ' ';

                    periodoSaida += 1;

                }
                else//horario é do mesmo periodo de periodoSaida, então o veículo chegou ao destino
                {

                    float tempoAux =distancia/velocidade;

                    aux = poluicaoRota(instancia, it->tipo, distancia, (*iterator)->cliente, (*iterator)->cliente, periodoSaida);
                    aux += poluicaoCarga(instancia, it->tipo, cargaTotal, instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente]);
                    poluicaoAux += aux;
                    combustivelAux += combustivelRota(instancia, it->tipo, distancia, (*iterator)->cliente, (*iterator)->cliente, periodoSaida);
                    combustivelAux += combustivelCarga(instancia, it->tipo, cargaTotal, instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente]);

                    combustivel += combustivelAux;

                    saida += std::to_string(periodoSaida) + "," + std::to_string(tempoAux) + "," + std::to_string(distancia) + "," + std::to_string(aux) + "," +std::to_string(velocidade) + ",";
                    saida += std::to_string(0) + '\n';

                    if(fabs(poluicaoAux - ((*itCliente)->poluicao ) > 0.001))
                    {
                        delete []vetorClientes;
                        return false;
                    }

                    if(fabs(combustivelAux - (*itCliente)->combustivel) > 0.001)
                    {
                        delete []vetorClientes;

                        return false;
                    }

                    distancia = 0;
                    horaChegada = horario;

                }



            }while(distancia != 0.0);

            combustivel += (*itCliente)->combustivel;
            poluicao += (*itCliente)->poluicao;
            vetorClientes[(*itCliente)->cliente] = 1;
            cargaTotal -= instancia->vetorClientes[(*itCliente)->cliente].demanda;


            //Verificar se horaChegada é igual a itCliente.tempoChegada.
            if(fabs((*itCliente)->tempoChegada - horaChegada) <= 0.001)
            {

                /* **********************************************************
                 * tempoChegada está correto.
                 * Verificar janela de tempo, tempo de espera, combustível ...
                 *
                ***********************************************************/



                if((*itCliente)->tempoChegada >= instancia->vetorClientes[(*itCliente)->cliente].inicioJanela) //Chegou após o inicio da janela
                {

                    // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

                    float tempoSaida = (*itCliente)->tempoChegada + instancia->vetorClientes[(*itCliente)->cliente].tempoServico;

                    if(!((tempoSaida <= instancia->vetorClientes[(*itCliente)->cliente].fimJanela) && (fabs(tempoSaida - ((*itCliente)->tempoChegada + instancia->vetorClientes[(*itCliente)->cliente].tempoServico )) <= 0.0001)))
                    {
                        //Solução está ERRADA.

                        delete []vetorClientes;
                        return false;
                    }


                }
                else //Chegou antes do inicio da janela
                {

                    //Verificar tempo de saida

                    if(instancia->vetorClientes[(*itCliente)->cliente].inicioJanela + instancia->vetorClientes[(*itCliente)->cliente].tempoServico != (*itCliente)->tempoSaida)
                    {
                        //Solução está ERRADA.
                        std::cout<<"Solucao Errada.\n";
                        std::cout <<(*itCliente)->cliente<<"\n";

                        delete []vetorClientes;
                        return false;

                    }
                }

            }
            else
            {
                //Solução está ERRADA.

                delete []vetorClientes;
                return false;
            }

        }

        //if((it->carga != carga) || (carga > instancia->vetorVeiculos.capacidade) || ((fabs(it->combustivel - combustivel) > 0.001)) || ((fabs(it->poluicao - poluicao) > 0.001)))
        if((it->carga != carga) || (carga > instancia->vetorVeiculos.capacidade) || ((fabs(it->poluicao - poluicao) > 0.001)) || ((fabs(it->combustivel - combustivel) > 0.001)))
        {
            //Solução está ERRADA.

            delete []vetorClientes;
            return false;
        }


    }

    saida += "-1\n";

    bool clientesVisitados = true;

    for(int i = 0; i < instancia->numClientes; ++i)
    {
        if(!vetorClientes[i])
        {
            //std::cout<<std::endl<<i<<std::endl;
            clientesVisitados = false;


            break;
        }
    }

    double sumPoluicao = 0.0;
    for(auto it : solucao->vetorVeiculos)
    {

        sumPoluicao += (*it).poluicao;
    }

    if(fabs(sumPoluicao - solucao->poluicao) <= 0.001)
    {
        if(texto)
            *texto += saida;

        delete[]vetorClientes;
        return clientesVisitados;

    }
    else
    {

        delete []vetorClientes;
        return false;
    }




}

double VerificaSolucao::calculaPoluicao(double velocidade, double tempoViagem, const Instancia::Instancia *const instancia)
{
    double poluicao = 0.0;

    for(int i = 0; i < 7; ++i)
    {
        poluicao += (instancia->CO[i] * pow(velocidade, i))/velocidade;
        poluicao += (instancia->HC[i] * pow(velocidade, i))/velocidade;
        poluicao += (instancia->NOX[i] * pow(velocidade, i))/velocidade;
        poluicao += (instancia->CO[i] * pow(velocidade, i))/velocidade;
        poluicao += (instancia->PM[i] * pow(velocidade, i))/velocidade;
        poluicao += (instancia->CO2[i] * pow(velocidade, i))/velocidade;

    }


    poluicao = (poluicao*tempoViagem)/1000.0;
    return poluicao;


}

double VerificaSolucao::poluicaoRota(const Instancia::Instancia *const instancia, int tipoVeiculo, double distanciaParcial, int i, int j, int k)
{

    double c = instancia->vetorVeiculos[tipoVeiculo].cVeiculo;

    return c * instancia->matrizCo2[i][j][k][tipoVeiculo] * distanciaParcial;

}

double VerificaSolucao::poluicaoCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, double carga, double distanciaTotal)
{

    double c = instancia->vetorVeiculos[tipoVeiculo].cVeiculo;
    double p = instancia->vetorVeiculos[tipoVeiculo].pVeiculo;

    return c * p * carga * distanciaTotal;

}

double VerificaSolucao::combustivelRota(const Instancia::Instancia *const instancia, int tipoVeiculo, double distanciaParcial, int i, int j, int k)
{
    return instancia->matrizCo2[i][j][k][tipoVeiculo] * distanciaParcial;
}

double VerificaSolucao::combustivelCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, double carga, double distanciaTotal)
{

    double p = instancia->vetorVeiculos[tipoVeiculo].pVeiculo;

    return p * carga * distanciaTotal;
}