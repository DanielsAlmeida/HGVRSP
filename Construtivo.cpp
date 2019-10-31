//
// Created by igor on 21/10/19.
//

#include "Construtivo.h"
#include <cmath>
#include <tuple>

using namespace Construtivo;
using namespace std;

Solucao::Solucao* Construtivo::geraSolucao(Instancia::Instancia *instancia, bool (*comparador)(Instancia::Cliente &, Instancia::Cliente & ))
{

    //Inicializa a lista de candidatos(Clientes)
    list<Instancia::Cliente> listaCandidatos;

    for(int i = 0; i < instancia->numClientes; ++i)
    {
        if(instancia->vetorClientes[i].cliente != 0)
            listaCandidatos.push_back(instancia->vetorClientes[i]);
    }

    //Ordena de acordo com o critério
    listaCandidatos.sort(comparador);

    //Cria uma solução com o mínimo de veiculos
    int minVeiculos = instancia->demandaTotal/instancia->vetorVeiculos.capacidade + 1;
    Solucao::Solucao *solucao = new Solucao::Solucao(minVeiculos);


    Solucao::Veiculo *melhorVeiculo;
    std::list<Solucao::ClienteRota *>::iterator melhorPosicao, posicaoAux;
    double melhorPoluicao, melhorCombustivel, auxPoluicao, auxCombustivel;

    auto *candidato = new Solucao::ClienteRota;
    Instancia::Cliente clienteAux;

    //Vetor guarda o resto da lista
    auto *vetorClienteBest = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetorClienteAux = new Solucao::ClienteRota[instancia->numClientes+2];

    Solucao::ClienteRota *vetorClienteSwap; //Ponteiro para trocar os vetores.

    int tamVetBest, tamVetAux;

    double combustivelParcial, poluicaoParcial; //Acumula combustivel e poluicao da rota parcial

    bool viavel;

    /* Enquanto a lista de candidatos for diferente de vazio, escolha um cliente, calcule o acrescimo de poluição para cada
    * posição possível da solução, desde que a solução seja viável.
    */



    while(!listaCandidatos.empty())
    {


        melhorPoluicao = HUGE_VALF;

        //retira um cliente da lista

        clienteAux = listaCandidatos.front();
        listaCandidatos.pop_front();

        candidato->cliente = clienteAux.cliente;



        //Percorre os veículos
        for(auto veiculo = solucao->vetorVeiculos.begin(); veiculo != solucao->vetorVeiculos.end(); ++veiculo)
        {


            //Verifica se a capacidade max será atingida
            if((*veiculo)->carga + clienteAux.demanda > instancia->vetorVeiculos.capacidade)
                continue;

            //Variaveis para armazenar a somo do combustivel e da poluicao do veiculo até clienteIt
            combustivelParcial = poluicaoParcial = 0;

            //Percorrer os clientes.
            for(auto clienteIt = (*veiculo)->listaClientes.begin(); clienteIt != (*veiculo)->listaClientes.end(); )
            {
                //Candidato tentara ser inserido após clienteIt



                combustivelParcial += (*clienteIt)->combustivel;
                poluicaoParcial += (*clienteIt)->poluicao;




                tie(viavel, tamVetAux, auxCombustivel, auxPoluicao) = viabilidadeInserirCandidato(vetorClienteAux,
                                                                                                  clienteIt,
                                                                                                  instancia, candidato,
                                                                                                  combustivelParcial,
                                                                                                  poluicaoParcial,
                                                                                                  *veiculo);

                if(viavel)
                {

                    if(auxPoluicao < melhorPoluicao)
                    {
                        melhorVeiculo = (*veiculo);
                        melhorPoluicao = auxPoluicao;
                        melhorCombustivel = auxCombustivel;
                        tamVetBest = tamVetAux;
                        melhorPosicao = clienteIt;


                        //trocar vetores

                        vetorClienteSwap = vetorClienteBest;
                        vetorClienteBest = vetorClienteAux;
                        vetorClienteAux = vetorClienteSwap;
                    }
                }

                ++clienteIt;

                if((*clienteIt)->cliente == 0)
                    break;


            }

        }

        /* Se encontrou uma solução, incluir candidato no veículo de posicao melhorVeiculo,
         *    Caso não encontrou uma solução, criar um novo veículo com o candidato.
         */


        if(melhorPoluicao == HUGE_VALF)
        {

/**********************************************************************************************************************
*           Não conseguiu inserir o cliente em nenhuma posição. Cria um novo veiculo.
*           corrigir horário de saida do deposito, tempo de espera.
**********************************************************************************************************************/


            auto *veiculo = new Solucao::Veiculo;
            determinaHorario((*veiculo->listaClientes.begin()), candidato, instancia);
            determinaHorario(candidato, (veiculo->listaClientes.back()), instancia);

            auto iterador = veiculo->listaClientes.begin();
            ++iterador;

            veiculo->listaClientes.insert(iterador, candidato);

   /*         cout<<"Veiculo novo:\n";

            for(auto itV : veiculo->listaClientes)
            {
                cout<<(itV->cliente)<<" ";
            }

            cout<<"\n";*/

            veiculo->combustivel = candidato->combustivel + (veiculo->listaClientes.back())->combustivel;
            veiculo->poluicao = candidato->poluicao + (veiculo->listaClientes.back())->poluicao;

            veiculo->carga = instancia->vetorClientes[candidato->cliente].demanda;

            candidato = new Solucao::ClienteRota;

            solucao->vetorVeiculos.push_back(veiculo);
            solucao->poluicao += veiculo->poluicao;

        }
        else
        {

            //Inserir candidato no veiculo
            melhorPosicao++;
            melhorVeiculo->listaClientes.insert(melhorPosicao, candidato);


            int j = 0;
            --melhorPosicao;
            --melhorPosicao;

            //MelhorPossicao é igual a posicao anterior de candidato.

            //Corrigi o resto do veiculo
            for(;melhorPosicao != melhorVeiculo->listaClientes.end(); ++melhorPosicao, ++j)
            {
                (**melhorPosicao) = vetorClienteBest[j];

            }

            melhorVeiculo->poluicao = melhorPoluicao;
            melhorVeiculo->combustivel = melhorCombustivel;
            melhorVeiculo->carga += instancia->vetorClientes[candidato->cliente].demanda;


            candidato = new Solucao::ClienteRota;
            solucao->poluicao += melhorPoluicao;







        }
    }

    delete candidato;
    delete []vetorClienteAux;
    delete []vetorClienteBest;

    return solucao;


}

/*Realiza a viagem de cliente1 para cliente2, considerando que cliente1 já esta inserido na solução.
 *
 * Verifica a viabilidade (janela de tempo, tempo de espera maximo). insere informações da rota em cliente2.
 */
bool Construtivo::determinaHorario( Solucao::ClienteRota*  cliente1, Solucao::ClienteRota *cliente2, Instancia::Instancia *instancia)
{


    double distancia = instancia->matrizDistancias[cliente1->cliente][cliente2->cliente];
    double horaPartida = cliente1->tempoSaida;
    double velocidade, tempoRestantePeriodo, horario, horaChegada, poluicaoAux, combustivelAux, poluicao;
    int periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]

    if(distancia == 0.0)
        return false;


        do
        {

            velocidade = instancia->matrizVelocidade[cliente1->cliente][cliente2->cliente][periodoSaida];//velocidade -> km/h

            horario = horaPartida + (distancia / velocidade); //Horario de chegada considerando somente uma velocidade. Horario em horas


            if ((instancia->retornaPeriodo(horario) != periodoSaida))//Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
            {

                //Percorreu todo o periodoSaida e não chegou ao destino.

                tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;

                distancia -= tempoRestantePeriodo * velocidade;

                horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;

                poluicaoAux += VerificaSolucao::calculaPoluicao(velocidade, tempoRestantePeriodo, instancia);
                periodoSaida += 1;


                combustivelAux += VerificaSolucao::calculaConsumo(velocidade, tempoRestantePeriodo, instancia);

            } else//horario é do mesmo periodo de periodoSaida, então o veículo chegou ao destino
            {

                if(cliente1->cliente == 0)
                {

                    if(horario < instancia->vetorClientes[cliente2->cliente].inicioJanela)
                    {
                        //Se o tempo de espera excede o tempo máximo, cliente1 tem a sua saida atrasada.
                        if ((instancia->vetorClientes[(cliente2)->cliente].inicioJanela - horario) > instancia->EsperaMax)
                        {
                            cliente1->tempoSaida = (instancia->vetorClientes[(cliente2)->cliente].inicioJanela ) + instancia->EsperaMax;
                            horaPartida = cliente1->tempoSaida;
                            periodoSaida = instancia->retornaPeriodo(horaPartida);
                            distancia = instancia->matrizDistancias[cliente1->cliente][cliente2->cliente];

                            continue;

                        }
                    }
                }

                float tempoAux = distancia / velocidade;
                poluicaoAux += VerificaSolucao::calculaPoluicao(velocidade, tempoAux, instancia);


                combustivelAux += VerificaSolucao::calculaConsumo(velocidade, tempoAux, instancia);

                cliente2->poluicao = poluicaoAux;
                cliente2->combustivel = combustivelAux;

                distancia = 0;
                horaChegada = horario;

            }


        } while (distancia != 0.0);

        cliente2->tempoChegada = horaChegada;

        if ((cliente2)->tempoChegada >= instancia->vetorClientes[(cliente2)->cliente].inicioJanela) //Chegou após o inicio da janela
        {

            // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

            cliente2->tempoSaida = (cliente2)->tempoChegada + instancia->vetorClientes[(cliente2)->cliente].tempoServico;

            if (cliente2->tempoSaida <= instancia->vetorClientes[(cliente2)->cliente].fimJanela)
                return true;
            else
                return false;


        } else //Chegou antes do inicio da janela
        {

            cliente2->tempoSaida = instancia->vetorClientes[(cliente2)->cliente].inicioJanela + instancia->vetorClientes[(cliente2)->cliente].tempoServico;

            //Verifica o tempo de espera
            if ((instancia->vetorClientes[(cliente2)->cliente].inicioJanela - (cliente2)->tempoChegada) <= instancia->EsperaMax)
                return true;

            else
                return false;

        }



}

/*Tenta inserir candidato após iteradorCliente. Ajusta e verifica a viabilidade dos próximos clientes
 * combustivel e poluicao do veiculo até iteratorCliente (inclusive).
 * 1° parametro de retorno: viabilidade da solução.
 * 2° parametro de retorno: tamanho do vetor vetorClientes.
 * 3° parametro de retorno: combustivel.
 * 4° parametro de retorno: poluicao.
 */
std::tuple<bool, int, float, float> Construtivo::viabilidadeInserirCandidato(Solucao::ClienteRota *vetorClientes,
                                                                             std::list<Solucao::ClienteRota *, std::allocator<Solucao::ClienteRota *>>::iterator iteratorCliente,
                                                                             Instancia::Instancia *instancia,
                                                                             Solucao::ClienteRota *candidato,
                                                                             double combustivelParcial, double poluicaoParcial,
                                                                             Solucao::Veiculo *veiculo)
{

    //inicialisa vetor de clientes
    vetorClientes[0] = **iteratorCliente;
    vetorClientes[1] = *candidato;

    int i;

    float combustivel = poluicaoParcial, poluicao = poluicaoParcial;



    //Verifica viabilidade com candidato
    if(determinaHorario(&vetorClientes[0], &vetorClientes[1], instancia))
    {
        ++iteratorCliente;

        combustivel += vetorClientes[1].combustivel;
        poluicao += vetorClientes[1].poluicao;



        //Percorre o resto dos clientes
        for(i = 1; (iteratorCliente) !=  veiculo->listaClientes.end(); ++i)
        {


            //Adiciona o próximo cliente da lista na nova solução

            vetorClientes[i+1] = (**iteratorCliente);

/*            vetorClientes[i+1].cliente = (*iteratorCliente)->cliente;
            vetorClientes[i+1].poluicao = (*iteratorCliente)->poluicao;
            vetorClientes[i+1].combustivel = (*iteratorCliente)->combustivel;
            vetorClientes[i+1].tempoSaida = (*iteratorCliente)->tempoSaida;
            vetorClientes[i+1].tempoChegada = (*iteratorCliente)->tempoChegada;*/



            //Verifica viabilidade
            if(!determinaHorario(&vetorClientes[i], &vetorClientes[i+1], instancia))
                return {false, -1, -1, -1};



            ++iteratorCliente;

            //Armazenar combustível e poluição

            combustivel += vetorClientes[i+1].combustivel;
            poluicao += vetorClientes[i+1].poluicao;

            if(combustivel > instancia->vetorVeiculos.combustivel)
            {
                return {false, -1, -1, -1};
            }

        }

        i +=1;

    }
    else
    {

        return {false, -1, -1, -1};
    }

    return {true, i, combustivel, poluicao};

}
