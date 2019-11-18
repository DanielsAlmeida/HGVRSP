//
// Created by igor on 21/10/19.
//

#include "Construtivo.h"
#include <cmath>
#include <tuple>
#include "mersenne-twister.h"

using namespace Construtivo;
using namespace std;

Solucao::Solucao *Construtivo::reativo(Instancia::Instancia *instancia, bool (*comparador)(Instancia::Cliente &, Instancia::Cliente &), float *vetorAlfa, int tamAlfa, const int numInteracoes, const int numIntAtualizarProb)
{

    //Vetor guarda o resto da lista. São passados para construir a solução
    auto *vetorClienteBest = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetorClienteAux = new Solucao::ClienteRota[instancia->numClientes+2];

    //Vetores para o reativo
    double *vetorProbabilidade = new double[tamAlfa];
    int *vetorFrequencia = new int[tamAlfa];
    double *solucaoAcumulada = new double[tamAlfa];
    double *vetorMedia = new double[tamAlfa];
    double *proporcao = new double[tamAlfa];
    int somaProb;
    int valAleatorio;
    int minVeiculos = instancia->demandaTotal/instancia->vetorVeiculos.capacidade + 1;

    //Inicializa a melhor solução
    vetorFrequencia[0] = 1;
    Solucao::Solucao *best = geraSolucao(instancia, comparador, vetorAlfa[0], vetorClienteBest, vetorClienteAux);
    vetorProbabilidade[0] = 1.0/tamAlfa;
    solucaoAcumulada[0] = best->poluicao;
    Solucao::Solucao *solucaoAux;
    int posicaoAlfa;

    //Inicializa os vetores de todos os alfas
    for(int i = 1; i < tamAlfa; ++i)
    {
        vetorProbabilidade[i] = 1/tamAlfa;
        vetorFrequencia[i] = 1;

        //Inicializa a solucaoAcumulada para o alfa
        solucaoAux = geraSolucao(instancia, comparador, vetorAlfa[i], vetorClienteBest, vetorClienteAux);
        solucaoAcumulada[i] = solucaoAux->poluicao;

        if(solucaoAux->poluicao < best->poluicao)
        {
            delete best;
            best = solucaoAux;
            solucaoAux = NULL;
        }
        else
        {
            delete solucaoAux;
            solucaoAux = NULL;
        }


    }

    for(int i = 0; i < numInteracoes; ++i)
    {

        //Atualiza probabilidade
        if((i%numIntAtualizarProb) == 0)
            atualizaProbabilidade(vetorProbabilidade, vetorFrequencia, solucaoAcumulada, vetorMedia, proporcao, tamAlfa, best->poluicao);

        //Escolher alfa, gerar solução, atualizar vetores, ....
        somaProb = posicaoAlfa = 0;

        valAleatorio = rand_u32() % 100;

        for(int j=0;somaProb < valAleatorio; ++j)
        {

            somaProb+= int(100.0 * vetorProbabilidade[j]);
            posicaoAlfa = j;

        }

        //Cria solução com o alfa escolhido
        solucaoAux = geraSolucao(instancia, comparador, vetorAlfa[posicaoAlfa], vetorClienteBest, vetorClienteAux);

        solucaoAcumulada[posicaoAlfa] += solucaoAux->poluicao;
        vetorFrequencia[posicaoAlfa] += 1;

        cout << "Verificao: " << VerificaSolucao::verificaSolucao(instancia, solucaoAux, false) << "\n\n";

        //Atualiza best
        if(solucaoAux->poluicao < best->poluicao)
        {

            delete best;
            best = solucaoAux;

            std::cout<<"Atualizacao, best = "<<best->poluicao<<"\n";

            solucaoAux = NULL;
        }
        else
        {
            delete solucaoAux;
            solucaoAux = NULL;
        }


    }

    //Libera memória
    delete []vetorClienteBest;
    delete []vetorClienteAux;
    delete []vetorProbabilidade;
    delete []vetorFrequencia;
    delete []solucaoAcumulada;
    delete []vetorMedia;
    delete []proporcao;

    return best;
}

void Construtivo::atualizaProbabilidade(double *vetorProbabilidade, int *vetorFrequencia, double *solucaoAcumulada, double *vetorMedia, double *proporcao, int tam, double melhorSolucao)
{

    double somaProporcoes = 0.0;

    //Calcular média

    for(int i = 0; i < tam; ++i)
    {
        vetorMedia[i] = solucaoAcumulada[i]/vetorFrequencia[i];

    }


    //Calcula proporção.
    for(int i = 0; i < tam; ++i)
    {
        proporcao[i] = melhorSolucao/vetorMedia[i];

        somaProporcoes += proporcao[i];
    }

    //Calcula probabilidade
    for(int i = 0; i<tam; ++i)
    {
        vetorProbabilidade[i] = proporcao[i]/somaProporcoes;
    }



}

Solucao::Solucao * Construtivo::geraSolucao(Instancia::Instancia *instancia, bool (*comparador)(Instancia::Cliente &, Instancia::Cliente &), float alfa, Solucao::ClienteRota *vetorClienteBest,
                                            Solucao::ClienteRota *vetorClienteAux)
{

    //Inicializa a lista de candidatos(Clientes)
    list<Instancia::Cliente> listaCandidatos;
    list<Instancia::Cliente>::iterator iteratorLisCand;

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

    Solucao::ClienteRota *vetorClienteSwap; //Ponteiro para trocar os vetores.

    int tamVetBest, tamVetAux;

    double combustivelParcial, poluicaoParcial; //Acumula combustivel e poluicao da rota parcial

    bool viavel;
    uint32_t tamLista;
    uint32_t escolhido;

    /* Enquanto a lista de candidatos for diferente de vazio, escolha um cliente, calcule o acrescimo de poluição para cada
    * posição possível da solução, desde que a solução seja viável.
    */



    while(!listaCandidatos.empty())
    {


        melhorPoluicao = HUGE_VALF;

        //retira um cliente da lista
        tamLista = listaCandidatos.size();
        //Escolhe um valor aleatorio
        tamLista = uint32_t(alfa*listaCandidatos.size()) + 1;
        escolhido = rand_u32();

        escolhido = escolhido % tamLista;

        iteratorLisCand = listaCandidatos.begin();

        //move iterator para escolhido
        advance (iteratorLisCand, escolhido);
        clienteAux = (*iteratorLisCand);

        //Apaga escolhido da lista
        listaCandidatos.erase(iteratorLisCand);

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


            double aux = melhorVeiculo->poluicao;
            melhorVeiculo->poluicao = melhorPoluicao;
            melhorVeiculo->combustivel = melhorCombustivel;
            melhorVeiculo->carga += instancia->vetorClientes[candidato->cliente].demanda;


            candidato = new Solucao::ClienteRota;
            solucao->poluicao -= aux;
            solucao->poluicao += melhorPoluicao;







        }
    }

    delete candidato;

    return solucao;


}

/*Realiza a viagem de cliente1 para cliente2, considerando que cliente1 já esta inserido na solução.
 *
 * Verifica a viabilidade (janela de tempo, tempo de espera maximo). insere informações da rota em cliente2.
 */
bool Construtivo::determinaHorario( Solucao::ClienteRota*  cliente1, Solucao::ClienteRota *cliente2, Instancia::Instancia *instancia)
{


    double distancia = instancia->matrizDistancias[cliente1->cliente][cliente2->cliente];
    //std::cout<<"Distancia "<<distancia<<"\n";

    double horaPartida = cliente1->tempoSaida;
    double velocidade, tempoRestantePeriodo, horario, horaChegada, poluicaoAux = 0, combustivelAux = 0, poluicao;
    int periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]

    velocidade = instancia->matrizVelocidade[cliente1->cliente][cliente2->cliente][periodoSaida];


    if((distancia == 0.0) || (velocidade == 0.0))
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
                            poluicaoAux = 0;
                            combustivelAux = 0;

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

            //Verifica viabilidade
            if(!determinaHorario(&vetorClientes[i], &vetorClientes[i+1], instancia))
                return {false, -1, -1, -1};

            ++iteratorCliente;

            //Armazenar combustível e poluição
            combustivel += vetorClientes[i+1].combustivel;
            poluicao += vetorClientes[i+1].poluicao;

            if(combustivel > instancia->vetorVeiculos.combustivel)
                return {false, -1, -1, -1};


        }

        i +=1;

    }
    else
    {

        return {false, -1, -1, -1};
    }

    return {true, i, combustivel, poluicao};

}
