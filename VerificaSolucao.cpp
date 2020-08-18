// Erro: verificaVeiculo
// Created by igor on 17/10/19.
//Corrigido 26/11/19

#include "VerificaSolucao.h"
#include "Construtivo.h"

using namespace VerificaSolucao;
using namespace std;

bool
VerificaSolucao::verificaSolucao(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, string *texto,
                                 double *distanciaTotal)
{

    if (solucao->veiculoFicticil)
        return false;

    std::string saida;
    int *vetorClientes = new int[instancia->numClientes]; //Vetor para checar se cada cliente foi visitado uma unica vez.

    for (int i = 0; i < instancia->numClientes; ++i)
        vetorClientes[i] = 0;


    int carga = 0, cargaTotal = 0;
    double combustivel;

    int periodoSaida, periodo;
    double distancia, velocidade, tempoRestantePeriodo, horario, horaChegada, horaPartida;
    std::list<Solucao::ClienteRota *>::iterator iterator;

    double poluicaoAux, poluicao, aux;
    double combustivelAux;

    if (distanciaTotal)
        *distanciaTotal = 0.0;

    //No No PERIODO,TEMPO,DISTANCIA,POLUICAO, Velocidade

    for (auto it : solucao->vetorVeiculos)//Percorre os veiculos da solução
    {

        if (it->tipo == 2)
        {
            saida += "-1\n\n";
            if (texto)
                *texto += saida;
            delete[]vetorClientes;
            return false;

        }

        if (it->listaClientes.size() <= 2)
            continue;

        carga = 0;
        poluicao = 0.0;
        combustivel = 0.0;
        cargaTotal = it->carga;

        auto cliente = it->listaClientes.begin();

        if (it->tipo == 1)
        {
            if ((*cliente)->tempoSaida < 0.5)
            {
                delete[]vetorClientes;
                cout << "Erro, tempoSaida incompativel com veiculo\n";
                return false;

            }

        }

        for (auto itCliente = it->listaClientes.begin();
             itCliente != it->listaClientes.end();)//Percorre os clientes do veículo
        {

            iterator = itCliente;
            ++itCliente;

            poluicaoAux = 0.0;
            combustivelAux = 0.0;

            if (itCliente == it->listaClientes.end())//verifica se itCliente é igual a NULL
                break;


            distancia = instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente];

            if (distanciaTotal)
                *distanciaTotal += distancia;

            if (distancia == 0.0)
            {
                cout << "Erro, distancia igual a 0 entre: " << (*iterator)->cliente << " " << (*itCliente)->cliente
                     << '\n';
                cout << "Tamanho lista: " << it->listaClientes.size() << '\n';
                delete[]vetorClientes;
                return false;
            }

            horaPartida = (*iterator)->tempoSaida;

            carga += instancia->vetorClientes[(*itCliente)->cliente].demanda;

            saida += std::to_string((*iterator)->cliente) + " " + std::to_string((*itCliente)->cliente) + " ";

            periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]

            do
            {

                velocidade = instancia->matrizVelocidade[(*iterator)->cliente][(*itCliente)->cliente][periodoSaida];//velocidade -> km/h
                horario = horaPartida + (distancia /
                                         velocidade); //Horario de chegada considerando somente uma velocidade. Horario em horas


                if (!(*itCliente)->percorrePeriodo[periodoSaida])
                {
                    horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;
                    periodoSaida += 1;

                    if (periodoSaida >= 5)
                    {
                        delete[]vetorClientes;
                        cout << "Erro periodo.\n";
                        return false;
                    }
                }
//Aqui.
                else if ((instancia->retornaPeriodo(horario) !=
                          periodoSaida))//Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
                {
                    // percorreu todo o periodoSaida e não chegou ao destino.

                    tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;
                    distancia -= tempoRestantePeriodo * velocidade;
                    horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;
                    aux = poluicaoRota(instancia, it->tipo, tempoRestantePeriodo * velocidade, (*iterator)->cliente, (*itCliente)->cliente, periodoSaida);
                    poluicaoAux += aux;

                    combustivelAux += combustivelRota(instancia, it->tipo, tempoRestantePeriodo * velocidade,
                                                      (*iterator)->cliente, (*itCliente)->cliente, periodoSaida);

                    saida += std::to_string(periodoSaida) + "," + std::to_string(tempoRestantePeriodo) + "," +
                             std::to_string(tempoRestantePeriodo * velocidade) + "," + std::to_string(aux) + "," +
                             std::to_string(velocidade) + ",";
                    saida += std::to_string(0) + ' ';

                    periodoSaida += 1;

                } else//horario é do mesmo periodo de periodoSaida, então o veículo chegou ao destino
                {

                    double tempoAux = distancia / velocidade;

                    aux = poluicaoRota(instancia, it->tipo, distancia, (*iterator)->cliente, (*itCliente)->cliente,
                                       periodoSaida);
                    aux += poluicaoCarga(instancia, it->tipo, cargaTotal,
                                         instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente]);
                    poluicaoAux += aux;
                    combustivelAux += combustivelRota(instancia, it->tipo, distancia, (*iterator)->cliente,
                                                      (*itCliente)->cliente, periodoSaida);
                    combustivelAux += combustivelCarga(instancia, it->tipo, cargaTotal,
                                                       instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente]);

                    combustivel += combustivelAux;
                    poluicao += poluicaoAux;

                    saida += std::to_string(periodoSaida) + "," + std::to_string(tempoAux) + "," +
                             std::to_string(distancia) + "," + std::to_string(aux) + "," + std::to_string(velocidade) +
                             ",";
                    saida += std::to_string(0) + '\n';

                    if (fabs(poluicaoAux - ((*itCliente)->poluicao) > 0.001))
                    {
                        cout << "Poluicao diferente.\n";
                        cout << "Diferenca = " << std::to_string(fabs(poluicaoAux - ((*itCliente)->poluicao))) << '\n';
                        cout << (*iterator)->cliente << ' ' << (*itCliente)->cliente << '\n';
                        delete[]vetorClientes;

                        cout << "Erro.\n";
                        return false;
                    }

                    if (fabs(combustivelAux - (*itCliente)->combustivel) > 0.001)
                    {
                        cout << "Combustivel diferente.\n";
                        delete[]vetorClientes;
                        cout << "Erro.\n";
                        return false;
                    }

                    distancia = 0;
                    horaChegada = horario;

                }


            } while (distancia != 0.0);

            vetorClientes[(*itCliente)->cliente] = 1;
            cargaTotal -= instancia->vetorClientes[(*itCliente)->cliente].demanda;


            //Verificar se horaChegada é igual a itCliente.tempoChegada.
            if (fabs((*itCliente)->tempoChegada - horaChegada) <= 0.001)
            {

                /* **********************************************************
                 * tempoChegada está correto.
                 * Verificar janela de tempo, tempo de espera, combustível ...
                 *
                ***********************************************************/



                if (((*itCliente)->tempoChegada >=
                     instancia->vetorClientes[(*itCliente)->cliente].inicioJanela)) //Chegou após o inicio da janela
                {

                    // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

                    double tempoSaida =
                            (*itCliente)->tempoChegada + instancia->vetorClientes[(*itCliente)->cliente].tempoServico;
                    //((*itCliente)->tempoChegada + instancia->vetorClientes[(*itCliente)->cliente].tempoServico ))

                    if (!((((*itCliente)->tempoChegada <= instancia->vetorClientes[(*itCliente)->cliente].fimJanela) ||
                           (((*itCliente)->tempoChegada -
                             instancia->vetorClientes[((*itCliente))->cliente].fimJanela) <= 1.0 / 60)) &&
                          ((fabs(tempoSaida - (*itCliente)->tempoSaida) <= 0.001) ||
                           ((*itCliente)->tempoSaida > tempoSaida) || ((*itCliente)->cliente == 0))))
                    {
                        //Solução está ERRADA.
                        cout << "Erro, Tempo\n";
                        delete[]vetorClientes;
                        return false;
                    }


                } else //Chegou antes do inicio da janela
                {

                    //Verificar tempo de saida

                    if ((*itCliente)->tempoSaida < instancia->vetorClientes[(*itCliente)->cliente].inicioJanela +
                                                   instancia->vetorClientes[(*itCliente)->cliente].tempoServico)
                    {
                        //Solução está ERRADA.
                        std::cout << "Erro, Tempo de saida\n";
                        std::cout << (*itCliente)->cliente << "\n";
                        cout << "ERRO.!\n";
                        delete[]vetorClientes;
                        return false;

                    }
                }

            } else
            {
                //Solução está ERRADA.
                cout << "ERRO.! Tempo\n";
                delete[]vetorClientes;
                return false;
            }

        }

        //if((it->carga != carga) || (carga > instancia->vetorVeiculos.capacidade) || ((fabs(it->combustivel - combustivel) > 0.001)) || ((fabs(it->poluicao - poluicao) > 0.001)))
        if ((it->carga != carga) || (carga > instancia->vetorVeiculos[it->tipo].capacidade) ||
            ((fabs(it->poluicao - poluicao) > 0.001)) || ((fabs(it->combustivel - combustivel) > 0.001)) ||
            (!verificaCombustivel(combustivel, it->tipo, instancia)))
        {
            //Solução está ERRADA.
            //cout<<"Outros.!\n";

            if ((it->carga != carga))
                cout << "Verificacao final. Carga diferente\n";

            if (carga > instancia->vetorVeiculos[it->tipo].capacidade)
                cout << "Verificacao final. capacidade\n";

            if ((fabs(it->poluicao - poluicao) > 0.001))
                cout << "Verificacao final. poluiao diferente\n";

            if (fabs(it->combustivel - combustivel))
                cout << "Verificacao final. Combustivel diferente\n";

            if (!verificaCombustivel(combustivel, it->tipo, instancia))
            {
                cout << "Verificacao final. consumo Combustivel a mais do que a capacidade\n";
                cout << "combustivel: " << combustivel << '\n';
                cout << "Capacidade combustivel: " << instancia->vetorVeiculos[it->tipo].combustivel << '\n';
                cout << "Tipo " << it->tipo << '\n';
                cout << "Poluicao: " << it->poluicao << '\n';
                cout << "Rota: " << it->getRota() << '\n';

            }

            delete[]vetorClientes;
            return false;
        }


    }

    saida += "-1\n";

    bool clientesVisitados = true;

    for (int i = 0; i < instancia->numClientes; ++i)
    {
        if (!vetorClientes[i])
        {
            //std::cout<<std::endl<<i<<std::endl;
            clientesVisitados = false;
            cout << "ERRO.! clientes\n";


            break;
        }
    }

    double sumPoluicao = 0.0;
    for (auto it : solucao->vetorVeiculos)
    {

        sumPoluicao += (*it).poluicao;
    }

    if (fabs(sumPoluicao - solucao->poluicao) <= 0.001)
    {
        if (texto)
            *texto += saida;

        delete[]vetorClientes;

        if (!clientesVisitados)
            cout << "Erro, cliente nao foi visitado.\n";

        return clientesVisitados;

    } else
    {
        cout << "ERRO.! soma poluicao\n";
        delete[]vetorClientes;
        return false;
    }


}

long double VerificaSolucao::poluicaoRota(const Instancia::Instancia *const instancia, int tipoVeiculo, long double distanciaParcial, int i, int j, int k)
{

    if (!(tipoVeiculo < 2))
    {
        cout << "Erro tipo de veiculo\n";
        cout << tipoVeiculo << '\n';
        exit(-1);
    }

    long double c = instancia->vetorVeiculos[tipoVeiculo].cVeiculo;

    if (!(i >= 0 && i < instancia->numClientes && j >= 0 && j < instancia->numClientes && k < 5 && k >= 0))
    {
        cout << "Arquivo: VerificaSolucao.cpp Linha: " << __LINE__ << " Erro poluicao rota\n";

        if (!(i >= 0 && i < instancia->numClientes && j >= 0 && j < instancia->numClientes))
            cout << "Indice i ou j: " << i << " " << j << "\n";

        if (k < 0)
            cout << "Erro indice k: " << k << '\n';

        exit(-1);
    }

    long double temp = c * instancia->matrizCo2[i][j][k][tipoVeiculo] * distanciaParcial;

    return temp;

}

long double VerificaSolucao::poluicaoCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, int carga, double distanciaTotal)
{

    long double c = instancia->vetorVeiculos[tipoVeiculo].cVeiculo;
    long double p = instancia->vetorVeiculos[tipoVeiculo].pVeiculo;

    return c * p * carga * distanciaTotal;

}

long double VerificaSolucao::combustivelRota(const Instancia::Instancia *const instancia, int tipoVeiculo, long double distanciaParcial,
                                 int i, int j, int k)
{
    return instancia->matrizCo2[i][j][k][tipoVeiculo] * distanciaParcial;
}

long double VerificaSolucao::combustivelCarga(const Instancia::Instancia *const instancia, int tipoVeiculo, int carga, double distanciaTotal)
{

    long double p = instancia->vetorVeiculos[tipoVeiculo].pVeiculo;

    return p * carga * distanciaTotal;
}

bool VerificaSolucao::verificaVeiculo(Solucao::Veiculo *veiculo, const Instancia::Instancia *const instancia)
{


    Solucao::ClienteRota *vetClienteRotaAux = new Solucao::ClienteRota[veiculo->listaClientes.size()];
    vetClienteRotaAux[0].cliente = 0;
    vetClienteRotaAux[0].tempoSaida = (*veiculo->listaClientes.begin())->tempoSaida;

    if (vetClienteRotaAux[0].tempoSaida < instancia->vetorVeiculos[veiculo->tipo].inicioJanela)
        return false;

    int peso = 0;

    double combustivel = 0;
    double poluicao = 0;

    for (auto it:veiculo->listaClientes)
        peso += instancia->vetorClientes[it->cliente].demanda;

    if (peso != veiculo->carga)
    {
        cout << "Carga esta errada\n";
        return false;
    }

    int posicao = 0;
    auto clienteIt = std::next(veiculo->listaClientes.begin(), 0);
    vetClienteRotaAux[0].tempoSaida = (*clienteIt)->tempoSaida;

    clienteIt++;


    while (1)
    {
        vetClienteRotaAux[posicao + 1].cliente = (*clienteIt)->cliente;
        peso -= instancia->vetorClientes[vetClienteRotaAux[posicao].cliente].demanda;

        if (peso < 0)
        {
            cout << "Peso negativo\n";
            delete[]vetClienteRotaAux;
            return false;
        }


        if (!Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao + 1], instancia,
                                           peso, veiculo->tipo, NULL, NULL))
        {
            cout << "ERRO\n" << vetClienteRotaAux[posicao].cliente << " " << vetClienteRotaAux[posicao + 1].cliente
                 << '\n';


            delete[]vetClienteRotaAux;
            return false;
        } else
        {
            if (fabs(vetClienteRotaAux[posicao + 1].tempoChegada - (*clienteIt)->tempoChegada) > 0.001)
            {
                cout << "Cliente: " << (*clienteIt)->cliente << '\n';
                cout << "Tempo diferente\n";
                cout << "Tempo chegada: " << vetClienteRotaAux[posicao + 1].tempoChegada << '\n';
                cout << "Tempo :" << (*clienteIt)->tempoChegada << '\n';
                cout << "Tempo saida: " << vetClienteRotaAux[posicao + 1].tempoSaida << '\n';
                cout << "Tempo: " << (*clienteIt)->tempoSaida << '\n';
                delete[]vetClienteRotaAux;
                return false;
            }

            if (vetClienteRotaAux[posicao + 1].cliente != 0)
            {
                if (!((((*clienteIt)->tempoSaida > vetClienteRotaAux[posicao + 1].tempoSaida) ||
                       (fabs((*clienteIt)->tempoSaida - vetClienteRotaAux[posicao + 1].tempoSaida) <= 0.001))))
                {
                    cout << "Erro, veiculo deveria ser viavel.\nArquivo: VerificaSolucao.cpp; Linha: " << __LINE__
                         << '\n';
                    cout << "Tempo de saida errado\n";
                    cout << "Cliente: " << (*clienteIt)->cliente << '\n';
                    cout << "Tempo rota " << vetClienteRotaAux[posicao + 1].tempoSaida << '\n';
                    cout << (*clienteIt)->tempoSaida << '\n';
                    delete[]vetClienteRotaAux;
                    return false;
                }
            }

            if (fabs(vetClienteRotaAux[posicao + 1].combustivel - (*clienteIt)->combustivel) > 0.001)
            {

                cout << "Erro, veiculo deveria ser viavel.\nArquivo: VerificaSolucao.cpp; Linha: " << __LINE__ << '\n';
                cout << "Cliente: " << (*clienteIt)->cliente << '\n';
                cout << "Combustivel\n";
                cout << "Diferenca: " << fabs(vetClienteRotaAux[posicao + 1].combustivel - (*clienteIt)->combustivel)
                     << '\n';
                cout << "combustivel real " << vetClienteRotaAux[posicao + 1].combustivel << '\n';
                cout << "Combustivel: " << (*clienteIt)->combustivel << '\n';

                cout << '\n';
                cout << "Rota gerada: \n";
                for (int i = 0; i <= posicao + 1; ++i)
                {
                    cout << vetClienteRotaAux[i].cliente << " (" << vetClienteRotaAux[i].tempoChegada << ", "
                         << vetClienteRotaAux[i].tempoSaida << ") " <<
                         "(c " << vetClienteRotaAux[i].combustivel << ", p" << vetClienteRotaAux[i].poluicao << ")\n";
                }

                cout << '\n';

                cout << "\nRota original\n";
                for (auto it:veiculo->listaClientes)
                {
                    cout << it->cliente << " (" << it->tempoChegada << ", " << it->tempoSaida << ") " <<
                         "(c " << it->combustivel << ", p" << it->poluicao << ")\n";

                    if (it->cliente == (*clienteIt)->cliente)
                        break;
                }

                cout << '\n';

                delete[]vetClienteRotaAux;
                return false;
            }
            if (fabs(vetClienteRotaAux[posicao + 1].poluicao - (*clienteIt)->poluicao) > 0.001)
            {
                cout << "Erro, veiculo deveria ser viavel.\nArquivo: VerificaSolucao.cpp; Linha: " << __LINE__ << '\n';
                cout << "Cliente: " << (*clienteIt)->cliente << '\n';
                cout << "poluicao\n";
                delete[]vetClienteRotaAux;
                return false;
            }

            combustivel += vetClienteRotaAux[posicao + 1].combustivel;
            poluicao += vetClienteRotaAux[posicao + 1].poluicao;

            vetClienteRotaAux[posicao + 1].tempoSaida = (*clienteIt)->tempoSaida;


        }

        if ((*clienteIt)->cliente == 0)
            break;

        posicao += 1;
        clienteIt++;
    }


    if (fabs(combustivel - veiculo->combustivel) > 0.001)
    {
        cout << "Erro, veiculo deveria ser viavel.\nArquivo: VerificaSolucao.cpp; Linha: " << __LINE__ << '\n';
        cout << "Combustivel\n";
        cout << "Diferenca: " << fabs(combustivel - veiculo->combustivel) << '\n';
        delete[]vetClienteRotaAux;
        return false;
    }


    if (fabs(poluicao - veiculo->poluicao) > 0.001)
    {
        cout << "Erro, veiculo deveria ser viavel.\nArquivo: VerificaSolucao.cpp; Linha: " << __LINE__ << '\n';
        cout << "Poluicao\n";
        cout << "Diferenca: " << fabs(poluicao - veiculo->poluicao) << '\n';
        delete[]vetClienteRotaAux;
        return false;
    }

    if (peso < 0)
    {
        cout << "Erro, veiculo deveria ser viavel.\nArquivo: VerificaSolucao.cpp; Linha: " << __LINE__ << '\n';
        cout << "Peso negativo\n";
        delete[]vetClienteRotaAux;
        return false;
    }

    delete[]vetClienteRotaAux;
    return true;
    /*
    int carga = 0;
    double poluicao = 0.0;
    double combustivel = 0.0;
    int cargaTotal = veiculo->carga;
    double poluicaoAux = 0.0;
    double combustivelAux = 0.0;
    double distancia;
    IteratorCliente iterator;
    double hora, horaPartida, saida, horario, aux, tempoRestantePeriodo, horaChegada;
    int periodoSaida;
    double velocidade;

    double divisao, horaPartidaOriginal;

    string saidaStr = "";

    for(auto itCliente = veiculo->listaClientes.begin(); itCliente != veiculo->listaClientes.end(); )//Percorre os clientes do veículo
    {

        iterator = itCliente;
        ++itCliente;

        poluicaoAux =  0.0;
        combustivelAux = 0.0;

        if(itCliente == veiculo->listaClientes.end())//verifica se itCliente é igual a NULL
            break;


        distancia = instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente];

        if(distancia == 0.0)
        {
            cout<<"Dis = 0 entre "<<(*iterator)->cliente<<" e "<<(*itCliente)->cliente<<" \n";
            return false;
        }

        horaPartida = (*iterator)->tempoSaida;

        carga += instancia->vetorClientes[(*itCliente)->cliente].demanda;



        periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]
        saidaStr = "";
        do
        {

            velocidade = instancia->matrizVelocidade[(*iterator)->cliente][(*itCliente)->cliente][periodoSaida];//velocidade -> km/h
            divisao = (distancia / velocidade);
            horaPartidaOriginal = horaPartida;
            saidaStr+="hora de partida "+ std::to_string(horaPartida)+'\n';
            saidaStr += "Periodo hora de partida: "+std::to_string(instancia->retornaPeriodo(horaPartida))+'\n';
            saidaStr += "Hora chegada: "+std::to_string(horaPartida + (distancia / velocidade))+'\n';
            saidaStr += "Periodo hora chegada: "+std::to_string(instancia->retornaPeriodo(horaPartida + (distancia / velocidade)))+'\n';
            saidaStr+="Distancia: "+std::to_string(distancia)+'\n';

            horario = horaPartida + (distancia / velocidade); //Horario de chegada considerando somente uma velocidade. Horario em horas



            if(!(*itCliente)->percorrePeriodo[periodoSaida])
            {
                periodoSaida += 1;

                if(periodoSaida >= 5 )
                {
                    cout<<"tempoSaida: "<<horaPartida<<'\n';
                    cout<<"Periodo: "<<periodoSaida-1<<'\n';
                    cout<<(*iterator)->cliente<<" ** "<<(*itCliente)->cliente<<'\n';
                    cout<<"Periodo.\n";
                    return false;
                }

                horaPartida = instancia->vetorPeriodos[periodoSaida].inicio;
            }

            else if((instancia->retornaPeriodo(horario) != periodoSaida))//Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
            {
                // percorreu todo o periodoSaida e não chegou ao destino.
                tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;
                distancia -= tempoRestantePeriodo * velocidade;
                horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;
                aux = poluicaoRota(instancia, veiculo->tipo, tempoRestantePeriodo*velocidade, (*iterator)->cliente, (*itCliente)->cliente, periodoSaida);
                poluicaoAux += aux;

                combustivelAux += combustivelRota(instancia, veiculo->tipo, tempoRestantePeriodo*velocidade, (*iterator)->cliente, (*itCliente)->cliente, periodoSaida);

                periodoSaida += 1;

                saidaStr += "Distancia: "+std::to_string(distancia)+'\n';

                if(distancia == 0.0)
                    saidaStr+="distancia eh 0\n";

            }
            else//horario é do mesmo periodo de periodoSaida, então o veículo chegou ao destino
            {

                double tempoAux =distancia/velocidade;

                aux = poluicaoRota(instancia, veiculo->tipo, distancia, (*iterator)->cliente, (*itCliente)->cliente, periodoSaida);
                aux += poluicaoCarga(instancia, veiculo->tipo, cargaTotal, instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente]);
                poluicaoAux += aux;
                combustivelAux += combustivelRota(instancia, veiculo->tipo, distancia, (*iterator)->cliente, (*itCliente)->cliente, periodoSaida);
                combustivelAux += combustivelCarga(instancia, veiculo->tipo, cargaTotal, instancia->matrizDistancias[(*iterator)->cliente][(*itCliente)->cliente]);

                combustivel += combustivelAux;
                poluicao += poluicaoAux;

                if(fabs(poluicaoAux - ((*itCliente)->poluicao ) > 0.001))
                {
                    cout<<"Poluicao diferente.\n";
                    cout<<"Diferenca = "<<std::to_string(fabs(poluicaoAux - ((*itCliente)->poluicao )))<<'\n';
                    cout<<"Valor real: "<<poluicaoAux<<'\n';
                    cout<<(*iterator)->cliente << ' ' <<(*itCliente)->cliente<<'\n';
                    return false;
                }

                if(fabs(combustivelAux - (*itCliente)->combustivel) > 0.001)
                {
                    cout<<"Combustivel diferente.\n";
                    cout<<"Diferenca = "<<std::to_string(fabs(combustivelAux - ((*itCliente)->combustivel )))<<'\n';
                    cout<<"Valor real: "<<combustivelAux<<'\n';
                    cout<<(*iterator)->cliente << ' ' <<(*itCliente)->cliente<<'\n';

                    return false;
                }

                distancia = 0;
                saidaStr +="horario: "+std::to_string(horario)+"\n";
                horaChegada = horario;

            }



        }while(distancia != 0.0);


        cargaTotal -= instancia->vetorClientes[(*itCliente)->cliente].demanda;


        //Verificar se horaChegada é igual a itCliente.tempoChegada.
        if(fabs((*itCliente)->tempoChegada - horaChegada) <= 0.001)
        {

            *//* **********************************************************
             * tempoChegada está correto.
             * Verificar janela de tempo, tempo de espera, combustível ...
             *
            ***********************************************************//*



            if((*itCliente)->tempoChegada >= instancia->vetorClientes[(*itCliente)->cliente].inicioJanela) //Chegou após o inicio da janela
            {

                // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

                double tempoSaida = (*itCliente)->tempoChegada + instancia->vetorClientes[(*itCliente)->cliente].tempoServico;

                if(!((tempoSaida <= instancia->vetorClientes[(*itCliente)->cliente].fimJanela) && ((fabs(tempoSaida - (*itCliente)->tempoSaida) <= 0.001) || ((*itCliente)->tempoSaida > tempoSaida) || ((*itCliente)->cliente==0))))
                {
                    //Solução está ERRADA.
                    std::cout<<"Erro tempo.\n";
                    std::cout<<"cliente: "<<(*itCliente)->cliente<<'\n';

                    if(((*itCliente)->tempoSaida < tempoSaida))
                    {
                        cout << "tempo saida\n";

                    }

                    return false;
                }

//2972.62 2954.81
            }
            else //Chegou antes do inicio da janela
            {

                //Verificar tempo de saida

                if((*itCliente)->tempoSaida < instancia->vetorClientes[(*itCliente)->cliente].inicioJanela + instancia->vetorClientes[(*itCliente)->cliente].tempoServico)
                {
                    //Solução está ERRADA.
                    std::cout<<"Erro, Tempo de saida\n";
                    std::cout <<(*itCliente)->cliente<<"\n";

                    return false;

                }
            }

        }
        else
        {
            //Solução está ERRADA.
            cout<<saidaStr<<'\n';

            std::cout<<"Hora de chegada eh diferente de horaChegada.\n";
            std::cout<<"Cliente: "<<(*itCliente)->cliente<<'\n';
            std::cout<<"Divisao: "<<divisao<<'\n';
            std::cout<<"Hora de partida: "<<horaPartidaOriginal<<'\n';
            cout<<"Veiculo: " <<(*itCliente)->tempoChegada<<'\n';
            cout<<"Correto: "<<horaChegada<<'\n';
            cout<<"Tipo veiculo: "<<veiculo->tipo<<'\n';

            return false;
        }

    }

    //if((it->carga != carga) || (carga > instancia->vetorVeiculos.capacidade) || ((fabs(it->combustivel - combustivel) > 0.001)) || ((fabs(it->poluicao - poluicao) > 0.001)))
    if((veiculo->carga != carga) || (carga > instancia->vetorVeiculos[veiculo->tipo].capacidade) || ((fabs(veiculo->poluicao - poluicao) > 0.001)) || ((fabs(veiculo->combustivel - combustivel) > 0.001)))
    {
        //Solução está ERRADA.

        if(veiculo->carga != carga)
            cout<<"Carga.\n";

        if(carga > instancia->vetorVeiculos[veiculo->tipo].capacidade)
            cout<<"Capacidade.\n";

        if((fabs(veiculo->combustivel - combustivel) >= 0.001))
        {
            std::cout << "Combustivel.\n";
            cout<<"Diferenca: "<<fabs(veiculo->combustivel - combustivel)<<'\n';
            cout<<"Veiculo: "<<veiculo->combustivel<<'\n'<<"Real: "<<combustivel<<'\n';
        }

        if(fabs(veiculo->poluicao - poluicao) > 0.001)
        {
            std::cout << "Poluicao.\n";
            cout<<"Diferenca: "<<(fabs(veiculo->poluicao - poluicao) > 0.001)<<'\n';
            cout<<"Veiculo: "<<veiculo->poluicao<<'\n'<<"Real: "<<poluicao<<'\n';
        }


        return false;

    }
    else
        return true;*/


}

bool VerificaSolucao::verificaCombustivel(const long double combustivel, const Solucao::Veiculo *veiculo,
                                          const Instancia::Instancia *const instancia)
{
    return ((instancia->vetorVeiculos[veiculo->tipo].combustivel - combustivel) >= -0.001);
}

bool VerificaSolucao::verificaCombustivel(const long double combustivel, const int tipoVeiculo,
                                          const Instancia::Instancia *const instancia)
{
    return ((instancia->vetorVeiculos[tipoVeiculo].combustivel - combustivel) >= -0.001);
}

bool VerificaSolucao::verificaVeiculoRotaMip(Solucao::Veiculo *veiculo, const Instancia::Instancia *const instancia, double *distanciaTotal, string *erro)
{

    int carga = 0, cargaTotal = 0;
    long double combustivel;

    int periodoSaida, periodo;
    long double distancia, velocidade, tempoRestantePeriodo, horario, horaChegada, horaPartida;
    std::list<Solucao::ClienteRota *>::iterator clienteI;

    long double poluicaoAux, poluicao, aux;
    long double combustivelAux;

    if (distanciaTotal)
        *distanciaTotal = 0.0;

    //No No PERIODO,TEMPO,DISTANCIA,POLUICAO, Velocidade



    if (veiculo->tipo == 2)
    {
        if(erro)
        {
            *erro = "Erro, veiculo do tipo 2\n";
        }
        return false;
    }

    if (veiculo->listaClientes.size() <= 2)
    {
        if(veiculo->poluicao != 0.0)
            veiculo->poluicao = 0.0;

        return true;
    }

    carga = 0;
    poluicao = 0.0;
    combustivel = 0.0;
    cargaTotal = veiculo->carga;

    auto cliente = veiculo->listaClientes.begin();

    if ((((*cliente)->tempoSaida + 1e-5) < 0.5) && (veiculo->tipo == 1))
    {
        if(erro)
            *erro = "Erro, tempoSaida incompativel com veiculo\ntempoSaida: " + std::to_string((*cliente)->tempoSaida) + '\n'+
                    "tipo veiculo: " + std::to_string(veiculo->tipo) + '\n';

        return false;

    }


    for (auto clienteJ = veiculo->listaClientes.begin(); clienteJ != veiculo->listaClientes.end();)     //Percorre os clientes do veículo
    {

        clienteI = clienteJ;
        ++clienteJ;

        if (clienteJ == veiculo->listaClientes.end())//verifica se itCliente é igual a NULL
            break;

        //cout << "Arco: " << (*clienteI)->cliente << " " << (*clienteJ)->cliente << "\n";

        poluicaoAux = 0.0;
        combustivelAux = 0.0;




        distancia = instancia->matrizDistancias[(*clienteI)->cliente][(*clienteJ)->cliente];

        if (distanciaTotal)
            *distanciaTotal += distancia;

        if (distancia == 0.0)
        {
            if(erro)
            {
                *erro = "Erro, distancia igual a 0 entre: " + std::to_string((*clienteI)->cliente) + " " + std::to_string((*clienteJ)->cliente) + '\n';

            }

            return false;
        }

        horaPartida = (*clienteI)->tempoSaida;

        carga += instancia->vetorClientes[(*clienteJ)->cliente].demanda;


        periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]

        /*for(int i = 0; i < periodoSaida; ++i)
        {
            if((*clienteJ)->percorrePeriodo[i])
            {
                if(erro)
                    *erro = "Periodo de saida e percorrePeriodo incompativeis. Arco: " + std::to_string((*clienteI)->cliente) + ' ' + std::to_string((*clienteJ)->cliente) +
                            "\nperiodoSaida: "+std::to_string(periodoSaida) + " horaSaida clienteI: " + std::to_string((*clienteI)->tempoSaida) + " horaSaida: " + std::to_string(horaPartida);

                return false;
            }
        }*/

        int primeiroIndice = -1, ultimoIndice = -1, numPeriodos = 0;

        for(int i = periodoSaida; i < instancia->numPeriodos; ++i)
        {


            if((*clienteJ)->percorrePeriodo[i])
            {

                numPeriodos += 1;

                if(primeiroIndice == -1)
                    primeiroIndice = i;

                if(i > ultimoIndice)
                    ultimoIndice = i;


                long double aux = (*clienteJ)->distanciaPorPeriodo[i] / instancia->matrizVelocidade[(*clienteI)->cliente][(*clienteJ)->cliente][i];

                if(fabs(aux - (*clienteJ)->tempoPorPeriodo[i]) > 1e-5)
                {
                    if(erro)
                       *erro = "erro, Diferenca de tempo gasto no arco (" + std::to_string((*clienteI)->cliente) + " , " + std::to_string((*clienteJ)->cliente) + ") k = " + std::to_string(i) + '\n'+
                               "Diferenca: " + std::to_string(fabs(aux - (*clienteJ)->tempoPorPeriodo[i]) )+ '\n';

                    return false;
                }

                poluicao += poluicaoRota(instancia, veiculo->tipo, (*clienteJ)->distanciaPorPeriodo[i], (*clienteI)->cliente, (*clienteJ)->cliente, i);
                combustivel += combustivelRota(instancia, veiculo->tipo, (*clienteJ)->distanciaPorPeriodo[i], (*clienteI)->cliente, (*clienteJ)->cliente, i);

            }
        }

        poluicao += poluicaoCarga(instancia, veiculo->tipo, cargaTotal, instancia->matrizDistancias[(*clienteI)->cliente][(*clienteJ)->cliente]);
        combustivel += combustivelCarga(instancia, veiculo->tipo, cargaTotal, instancia->matrizDistancias[(*clienteI)->cliente][(*clienteJ)->cliente]);

        /*if(numPeriodos == 1)
        {
            horaChegada = (*clienteI)->tempoSaida + (*clienteJ)->tempoPorPeriodo[primeiroIndice];
            horaPartida = (*clienteI)->tempoChegada + instancia->vetorClientes[(*clienteI)->cliente].tempoServico;

            if(horaChegada < instancia->vetorClientes[(*clienteJ)->cliente].inicioJanela)
            {
                horaChegada = instancia->vetorClientes[(*clienteJ)->cliente].inicioJanela;
                horaPartida = horaChegada - (*clienteJ)->tempoPorPeriodo[primeiroIndice];
            }

            if((*clienteI)->cliente == 0)
            {
                horaPartida = (*clienteJ)->tempoChegada - (*clienteJ)->tempoPorPeriodo[primeiroIndice];
                horaChegada = horaPartida +
            }

        }
        else
        {
            horaChegada = instancia->vetorPeriodos[ultimoIndice].inicio + (*clienteJ)->tempoPorPeriodo[ultimoIndice];
            horaPartida = (*clienteI)->tempoChegada + instancia->vetorClientes[(*clienteI)->cliente].tempoServico;
        }

        if(!((fabs(horaChegada-(*clienteJ)->tempoChegada) <= 0.001) || ((*clienteJ)->tempoChegada > horaChegada)))
        {
            if(erro)
                *erro = "erro tempo Chegada, arco: " + std::to_string((*clienteI)->cliente) + ' ' + std::to_string((*clienteJ)->cliente) + '\n' + "hora calculada: " + std::to_string(horaChegada) +
                        " hora da rota: " + std::to_string((*clienteJ)->tempoChegada) + '\n' + "numPeriodos "+ std::to_string(numPeriodos)+"\ntempoSaida " + std::to_string((*clienteI)->tempoSaida);

            return false;
        }

        if(!((fabs(horaPartida - (*clienteI)->tempoSaida) < 0.001) || ((*clienteI)->tempoSaida) > horaPartida))
        {
            if(erro)
                *erro = "erro tempo saida, arco: " + std::to_string((*clienteI)->cliente) + ' ' + std::to_string((*clienteJ)->cliente) + '\n' + "hora calculada: " + std::to_string(horaPartida) +
                        " hora da rota: " + std::to_string((*clienteI)->tempoSaida)  + " numPeriodos "+ std::to_string(numPeriodos)+"\n";

            return false;
        }*/

        cargaTotal -= instancia->vetorClientes[(*clienteJ)->cliente].demanda;
        if(cargaTotal < 0)
        {
            *erro = "peso negativo.\nArquivo:\nVerificaSolucao\n Funcao verificaVeiculoRotaMip\n Linha: "+std::to_string(__LINE__) + '\n';
            return false;
        }


        //Verificar se horaChegada é igual a itCliente.tempoChegada.



            /* **********************************************************
             * tempoChegada está correto.
             * Verificar janela de tempo, tempo de espera, combustível ...
             *
            ***********************************************************/



        if ((((*clienteJ)->tempoChegada + 1e-6) >= instancia->vetorClientes[(*clienteJ)->cliente].inicioJanela)) //Chegou após o inicio da janela
        {

            // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

            long double tempoSaida = (*clienteJ)->tempoChegada + instancia->vetorClientes[(*clienteJ)->cliente].tempoServico;


            if (!((((*clienteJ)->tempoChegada <= instancia->vetorClientes[(*clienteJ)->cliente].fimJanela) ||
                   (((*clienteJ)->tempoChegada - instancia->vetorClientes[((*clienteJ))->cliente].fimJanela) <=
                    1.0 / 60)) && ((fabs(tempoSaida - (*clienteJ)->tempoSaida) <= 0.001) ||
                                   ((*clienteJ)->tempoSaida > tempoSaida) || ((*clienteJ)->cliente == 0))))
            {
                //Solução está ERRADA.

                if (erro)
                    *erro = "Erro, Tempo\n";

                return false;
            }


        } else //Chegou antes do inicio da janela
        {

            //Verificar tempo de saida

            if (((*clienteJ)->tempoSaida + 1e-6) < instancia->vetorClientes[(*clienteJ)->cliente].inicioJanela + instancia->vetorClientes[(*clienteJ)->cliente].tempoServico)
            {
                if (erro)
                {
                    //Solução está ERRADA.
                    *erro = "Erro, Tempo de saida " + std::to_string((*clienteJ)->cliente) + '\n' +
                            "Rota: " + to_string((*clienteJ)->tempoSaida) + "\ncalculado: " +
                            to_string(instancia->vetorClientes[(*clienteJ)->cliente].inicioJanela + instancia->vetorClientes[(*clienteJ)->cliente].tempoServico) + '\n';

                }

                return false;

            }
        }


        if((*clienteJ)->cliente == 0)
            break;

    }


    if ((veiculo->carga != carga) || (carga > instancia->vetorVeiculos[veiculo->tipo].capacidade) ||
        ((fabs(veiculo->poluicao - poluicao) > 0.001)) || ((fabs(veiculo->combustivel - combustivel) > 0.001)) ||
        (!verificaCombustivel(combustivel, veiculo->tipo, instancia)))
    {
        //Solução está ERRADA.
        //cout<<"Outros.!\n";

        if(erro)
        {

            if ((veiculo->carga != carga))
                *erro = "Carga diferente\n";

            if (carga > instancia->vetorVeiculos[veiculo->tipo].capacidade)
                *erro = "Verificacao final. capacidade\n";

            if ((fabs(veiculo->poluicao - poluicao) > 0.001))
                *erro = "Verificacao final. poluiao diferente\n";

            if (fabs(veiculo->combustivel - combustivel) > 0.001)
            {
                *erro = "Verificacao final. Combustivel diferente\n";
                *erro += "veiculo: "+std::to_string(veiculo->combustivel)+'\n';
                *erro += "calculado " + std::to_string(combustivel) + '\n';
                *erro += "tipo veiculo: " + std::to_string(veiculo->tipo)+'\n';
            }

            if (!verificaCombustivel(combustivel, veiculo->tipo, instancia))
            {
                *erro = "consumo Combustivel a mais do que a capacidade\n";

            }

            *erro += "-------ERRO-------\n";
        }

        return false;
    }

    cout<<"Poluicao "<<poluicao<<'\n';
    return true;


}
