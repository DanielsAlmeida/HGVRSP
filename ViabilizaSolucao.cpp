//
// Created by igor on 11/04/2020.
//

#include "ViabilizaSolucao.h"
#include "mersenne-twister.h"
#include "Movimentos.h"

using namespace ViabilizaSolucao;
using namespace Construtivo;

bool ViabilizaSolucao::geraSolucao(Solucao::Solucao *solucao, const Instancia::Instancia *const instancia, float alfa, Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux,
                                  string *sequencia, bool log, Construtivo::Candidato *vetorCandidatos, const double parametroHeur1, const double parametroHeur2, const bool heurist1)
{

    if(!solucao->veiculoFicticil)
        return true;

    string texto;

    //Inicializa a lista de candidatos(Clientes)
    list<Instancia::Cliente> listaCandidatos;
    int numClientesSol = 0;
    int numClientesFora = 0;

    Solucao::Veiculo *veiculoFicticil = solucao->vetorVeiculos[solucao->vetorVeiculos.size()-1];

    while(veiculoFicticil->listaClientes.size() > 0)
    {

        auto cliente = *veiculoFicticil->listaClientes.begin();

        if(cliente->cliente != 0)
        {
            listaCandidatos.push_back(instancia->vetorClientes[cliente->cliente]);
            numClientesFora++;
        }

        delete cliente;
        veiculoFicticil->listaClientes.pop_front();

    }

    numClientesSol = (instancia->numClientes - 1) - numClientesFora;

    solucao->vetorVeiculos.pop_back();
    delete veiculoFicticil;
    solucao->veiculoFicticil = false;



    //Cria uma solução com o mínimo de veiculos
    double beta, teta, gama;

    Solucao::Veiculo *melhorVeiculo;
    bool nullMelhorVeiculo = true;
    std::list<Solucao::ClienteRota *>::iterator melhorPosicao, posicaoAux;
    double melhorPoluicao, melhorCombustivel, auxPoluicao, auxCombustivel;
    double folgaRota, folgaRotaAux;
    double distRotaBest;

    auto *candidato = new Solucao::ClienteRota;
    Instancia::Cliente clienteAux;

    Solucao::ClienteRota *vetorClienteSwap; //Ponteiro para trocar os vetores.

    int tamVetBest, tamVetAux, peso, posicaoVetor;

    double combustivelParcial, poluicaoParcial, combAux, polAux; //Acumula combustivel e poluicao da rota parcial
    auto clienteItAux = solucao->vetorVeiculos[0]->listaClientes.begin();
    bool viavel;
    uint32_t tamLista2Crit;
    uint32_t escolhido;
    int b;
    /* *******************************************************************************************************************************
     *
     * Enquanto a lista de candidatos for diferente de vazio, escolha um cliente, calcule o acrescimo de poluição para cada
     * posição possível da solução, desde que a solução seja viável.
     *
     **********************************************************************************************************************************/

    //vetorCandidatos marca inicio

    Candidato *ultimo = NULL;

    bool incrementarIt = true;

    solucao->poluicaoPenalidades = 0;

    while (!listaCandidatos.empty())
    {

        atualizaPesos(&beta, &teta, instancia->numClientes, parametroHeur1, numClientesSol, &gama, parametroHeur2, heurist1);
        numClientesSol += 1;

        //Pega a melhor solucao de cada candidato.
        for (auto iteratorLisCand = listaCandidatos.begin(); iteratorLisCand != listaCandidatos.end(); )
        {
            incrementarIt = true;


            melhorPoluicao = HUGE_VALF;
            folgaRota = HUGE_VALF;
            nullMelhorVeiculo = true;

            clienteAux = (*iteratorLisCand);

            candidato->cliente = clienteAux.cliente;

            if (log)
                (*sequencia) += to_string(candidato->cliente) + ' ';

            texto += std::to_string(candidato->cliente) + " ";

            //Percorre os veículos
            for (auto veiculo = solucao->vetorVeiculos.begin(); veiculo != solucao->vetorVeiculos.end(); ++veiculo)
            {

                if ((*veiculo)->tipo == 2)
                    break;

                //Verifica se a capacidade max será atingida
                if ((*veiculo)->carga + clienteAux.demanda > instancia->vetorVeiculos->capacidade)
                    continue;

                //Variaveis para armazenar a soma do combustivel e da poluicao do veiculo até clienteIt
                poluicaoParcial = 0.0;
                combustivelParcial = 0.0;
                posicaoVetor = 0;
                peso = (*veiculo)->carga + clienteAux.demanda;
                vetorClienteAux[0] = **((*veiculo)->listaClientes.begin());


                //Percorrer os clientes.
                //Candidato tentara ser inserido após clienteIt

                for (auto clienteIt = (*veiculo)->listaClientes.begin(); clienteIt != (*veiculo)->listaClientes.end();)
                {

                    double aux = folgaRotaAux;
                    double disTemp = -HUGE_VAL;
                    tie(viavel, tamVetAux, auxPoluicao, auxCombustivel) = viabilidadeInserirCandidato(vetorClienteAux,
                                                                                                      clienteIt,
                                                                                                      instancia,
                                                                                                      candidato,
                                                                                                      combustivelParcial,
                                                                                                      poluicaoParcial,
                                                                                                      *veiculo, peso,
                                                                                                      posicaoVetor,
                                                                                                      &aux);

                    if (viavel)
                    {

                        if(!heurist1)
                        {
                            for (int i = 0; i + 2 < tamVetAux; ++i)
                            {
                                double dist = instancia->matrizDistancias[vetorClienteAux[i].cliente][vetorClienteAux[i + 1].cliente];
                                dist += instancia->matrizDistancias[vetorClienteAux[i + 1].cliente][vetorClienteAux[i+2].cliente];

                                if (dist > disTemp)
                                    disTemp = dist;
                            }
                        }

                        if (nullMelhorVeiculo)
                        {

                            nullMelhorVeiculo = false;
                            melhorVeiculo = (*veiculo);
                            melhorPoluicao = auxPoluicao;
                            melhorCombustivel = auxCombustivel;
                            tamVetBest = tamVetAux;
                            melhorPosicao = clienteIt;
                            if(aux < folgaRotaAux)
                                folgaRota = aux;
                            else
                                folgaRota = folgaRotaAux;

                            if(!heurist1)
                                distRotaBest = disTemp;

                            //trocar vetores
                            vetorClienteSwap = vetorClienteBest;
                            vetorClienteBest = vetorClienteAux;
                            vetorClienteAux = vetorClienteSwap;

                            for (int i = 0; i <= posicaoVetor; ++i)
                                vetorClienteAux[i] = vetorClienteBest[i];

                        }
                        else
                        {
                            //Variaveis auxiliares de folga e poluicao

                            double folgaSolParcial;    //Armazena a folga do veiculo da solucao parcial.
                            double folgaBest = folgaRota;

                            double distParcial;
                            double distBest;

                            if(!heurist1)
                            {
                                distParcial = disTemp;
                                distBest = distRotaBest;
                            }

                            if(aux < folgaRotaAux)
                                folgaSolParcial = aux;
                            else
                                folgaSolParcial = folgaRotaAux;

                            //Valor que eh incrementado na funcao objetivo
                            double incAtual = (auxPoluicao - (*veiculo)->poluicao);
                            double incBest = (melhorPoluicao - (*melhorVeiculo).poluicao);
                            double normaAtual;
                            double normaBest;

                            if(heurist1)
                            {
                                //Normaliza os valores
                                normaAtual = pow(folgaSolParcial, 2) + pow(incAtual, 2);
                                normaBest = pow(folgaBest, 2) + pow(incBest, 2);

                            }
                            else
                            {
                                normaAtual = pow(folgaSolParcial,2) + pow(incAtual, 2) + pow(distParcial, 2);
                                normaBest =  pow(folgaBest,2) + pow(incBest, 2) + pow(distBest, 2);
                            }

                            folgaSolParcial /=normaAtual;
                            incAtual /= normaAtual;

                            folgaBest /= normaBest;
                            incBest /= normaBest;

                            if(!heurist1)
                            {
                                distParcial /= normaAtual;
                                distBest /= normaBest;
                            }

                            bool condicao;

                            if(heurist1)
                                condicao = ((beta*incAtual + teta*(1.0/folgaSolParcial)) < (beta*incBest + teta*(1.0/folgaBest) ));
                            else
                                condicao = ((beta*incAtual + teta*(1.0/folgaSolParcial) + gama*distParcial) < (beta*incBest + teta*(1.0/folgaBest) + gama*distBest));

                            if(condicao)
                            {

                                melhorVeiculo = (*veiculo);
                                melhorPoluicao = auxPoluicao;
                                melhorCombustivel = auxCombustivel;
                                tamVetBest = tamVetAux;
                                melhorPosicao = clienteIt;

                                if(!heurist1)
                                    distRotaBest = distParcial;

                                if (aux < folgaRotaAux)
                                    folgaRota = aux;
                                else

                                    folgaRota = folgaRotaAux;


                                //trocar vetores
                                vetorClienteSwap = vetorClienteBest;
                                vetorClienteBest = vetorClienteAux;
                                vetorClienteAux = vetorClienteSwap;

                                for (int i = 0; i <= posicaoVetor; ++i)
                                    vetorClienteAux[i] = vetorClienteBest[i];
                            }

                        }
                    }

                    //cout<<(*clienteIt)->cliente<<'\n';

                    ++clienteIt;

                    if ((*clienteIt)->cliente == 0)
                        break;


                    posicaoVetor += 1;

                    //Calcula poluicao e combustivel entre --clienteIt --> clienteIt
                    if ((*clienteIt)->cliente != 0)
                    {

                        clienteItAux = clienteIt;
                        --clienteItAux;

                        //Calcula folga:

                        double folga =  (instancia->vetorClientes[(*clienteIt)->cliente].fimJanela - instancia->vetorClientes[(*clienteIt)->cliente].tempoServico) - (*clienteIt)->tempoChegada ;

                        if(folga < folgaRotaAux)
                        {
                            folgaRotaAux = folga;
                        }

                        //Poluicao da rota não muda
                        poluicaoParcial += (*clienteIt)->poluicaoRota;

                        //Poluicao das cargas muda
                        polAux = VerificaSolucao::poluicaoCarga(instancia, (*veiculo)->tipo, peso, instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);
                        poluicaoParcial += polAux;

                        combAux = VerificaSolucao::combustivelCarga(instancia, (*veiculo)->tipo, peso, instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);
                        combustivelParcial += combAux;

                        vetorClienteAux[posicaoVetor] = **(clienteIt);
                        vetorClienteAux[posicaoVetor].combustivel = (*clienteIt)->combustivelRota + combAux;
                        vetorClienteAux[posicaoVetor].poluicao = (*clienteIt)->poluicaoRota + polAux;

                        vetorClienteAux[posicaoVetor].combustivelRota = (*clienteIt)->combustivelRota;
                        vetorClienteAux[posicaoVetor].poluicaoRota = (*clienteIt)->poluicaoRota;


                        combustivelParcial += (*clienteIt)->combustivelRota;

                    }
                    if ((clienteIt) != (*veiculo)->listaClientes.end())
                        peso -= instancia->vetorClientes[(*clienteIt)->cliente].demanda;



                }

            }

            /* *********************************************************************************
             * Se encontrou uma solução, incluir candidato no veículo de posicao melhorVeiculo,
             * Caso não encontrou uma solução, criar um novo veículo com o candidato.
             *
            ***********************************************************************************/

            if (melhorPoluicao == HUGE_VALF)
            {

                /* *********************************************************************************************************************
                 *
                 * Não conseguiu inserir o cliente em nenhuma posição. Cria um novo veiculo.
                 *corrigir horário de saida do deposito, tempo de espera. Solucao passa a ser inviavel, insere um veiculo ficticil.
                 *
                 ***********************************************************************************************************************/


                Solucao::Veiculo *veiculo;

                if (!solucao->veiculoFicticil)
                {
                    solucao->veiculoFicticil = true;
                    veiculo = new Solucao::Veiculo(2);
                    solucao->vetorVeiculos.push_back(veiculo);

                } else
                {
                    veiculo = solucao->vetorVeiculos[instancia->numVeiculos];
                }

                auto iterador = veiculo->listaClientes.begin();
                ++iterador;

                veiculo->listaClientes.insert(iterador, candidato);


                //delete candidato;

                candidato = new Solucao::ClienteRota;

                const int valPenalizacao = instancia->penalizacao.at(instancia->numClientes - 1);

                solucao->poluicaoPenalidades += valPenalizacao;




                iteratorLisCand = listaCandidatos.erase(iteratorLisCand);                 //Apaga escolhido da lista


                incrementarIt = false;


            }else
            {

                //inserir a melhor posicao do cliente na lista de candidatos.

                if(ultimo==NULL)
                    ultimo = &vetorCandidatos[0];
                else
                    ultimo++;

                ultimo->distanciaDeposito = instancia->distanciaDeposito[candidato->cliente];
                ultimo->incrementoPoluicao = melhorPoluicao - (*melhorVeiculo).poluicao;
                ultimo->veiculo = melhorVeiculo;
                ultimo->candidato = candidato;
                ultimo->posicao = melhorPosicao;
                ultimo->folgaRota = folgaRota;

                if(!heurist1)
                    ultimo->distanciaArcos = distRotaBest;

                candidato = new Solucao::ClienteRota;


            }

            if(incrementarIt)
                ++iteratorLisCand;

            auto vetAux = ultimo;
            vetAux++;


        }

        if(listaCandidatos.size() != 0)
        {

            //Ordenar vetCandidatos
            auto vetAux = ultimo;
            vetAux++;

            unsigned long tam = vetAux - vetorCandidatos;




            int tamLista1Crit = tam * alfa + 1;

            double *vetorProb = new double[instancia->numClientes];
            auto ptrEscolhido = vetorCandidatos;
            int tamLista2Crit, tamLista3Crit;


            auto escolhaProporcional = [](double *vetorProb, Construtivo::Candidato *vetorCandidatos, int tamLista1Crit, int valAleatorio, int *tamRestante, Construtivo::Candidato *ptrEscolhido, int atributo)
            {

                double soma = 0.0;
                for(int i = 0; i < tamLista1Crit; ++i)
                {
                    if(atributo == 0)
                        vetorProb[i] = vetorCandidatos[i].folgaRota;

                    else if(atributo == 1)
                        vetorProb[i] = .1/vetorCandidatos[i].incrementoPoluicao;
                    else
                        vetorProb[i] = .1/vetorCandidatos[i].distanciaArcos;

                    soma += vetorProb[i];
                }
                int aux = 0;
                for(int j = 0; j < tamLista1Crit; ++j)
                {
                    aux += int(100.0*vetorProb[j]/soma);

                    if(tamRestante)
                        *tamRestante = j + 1;

                    if(aux >= valAleatorio || (j +1) == tamLista1Crit)
                        break;

                    if(ptrEscolhido)
                        ptrEscolhido++;

                }

            };

            /*  0: compCandidatoFolga, compCandidatoDist, compCandidato
             *  1: compCandidatoFolga, compCandidato, compCandidatoDist
             *  2: compCandidatoDist, compCandidatoFolga, compCandidato
             */
            if(heurist1)
            {
                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoFolga);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista1Crit, rand_u32() % 100, &tamLista2Crit, NULL,0);

                std::qsort(vetorCandidatos, tamLista2Crit, sizeof(Candidato), compCandidato);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista2Crit, rand_u32() % 100, NULL, ptrEscolhido, 1);
            }
            else
            {
                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoFolga);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista1Crit, rand_u32() % 100, &tamLista2Crit, NULL,0);

                std::qsort(vetorCandidatos, tamLista2Crit, sizeof(Candidato), compCandidato);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista2Crit, rand_u32() % 100, &tamLista3Crit, NULL,1);

                std::qsort(vetorCandidatos, tamLista3Crit, sizeof(Candidato), compCandidatoDist);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista3Crit, rand_u32() % 100, NULL, ptrEscolhido, 2);

            }

            listaCandidatos.remove(instancia->vetorClientes[ptrEscolhido->candidato->cliente]);


            double polAntes = ptrEscolhido->veiculo->poluicao;

            //Insere candidato escolhido
            insereCandidato(ptrEscolhido, instancia, vetorClienteAux);

            solucao->poluicao -= polAntes;
            solucao->poluicao += ptrEscolhido->veiculo->poluicao;

            for (auto ptr = vetorCandidatos; ptr != &vetorCandidatos[instancia->numClientes]; ptr++)
            {
                if (!ptr->candidato)
                    delete ptr->candidato;

            }


        }

        ultimo = NULL;


    }

    delete candidato;

    return !solucao->veiculoFicticil;

}

#define NUM 6

bool ViabilizaSolucao::viabilizaSolucao(Solucao::Solucao *solucao, const Instancia::Instancia *const instancia, float alfa, Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorClienteAux,
                                   string *sequencia, bool log, Construtivo::Candidato *vetorCandidatos, const double parametroHeur1, const double parametroHeur2, const bool heurist1,
                                   const int interacoes, const int interacoesPorMv, Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteSecondAux)
{

    int i = 0;
    int j = 0;
    int k = 0;
    bool pertubacao = false;
    Movimentos::ResultadosRota resultadosRota;

    int vetMovimentos[NUM] = {0, 1, 6, 7, 2, 3};  //{0, 1, 6, 7, 2, 3};

    while(i < interacoes)
    {
        pertubacao = false;

        for(j = 0; j < NUM; ++j)
        {
            for (k = 0; k < interacoesPorMv; ++k)
            {
                //cout<<"Antes\n";
                resultadosRota = Movimentos::aplicaMovimento(vetMovimentos[j], instancia, solucao, vetorClienteBest, vetorClienteAux, true, vetClienteRotaSecundBest, vetClienteSecondAux);
                //cout<<"Depois\n";

                if(resultadosRota.viavel)
                {
                    pertubacao = true;
                    break;
                }

                if(vetMovimentos[j] == 6)
                    break;

            }

            if(pertubacao)
                break;
        }

        if(pertubacao)
        {
            Movimentos::atualizaSolucao(resultadosRota, solucao, vetorClienteBest, vetClienteRotaSecundBest);

            int numClientes = solucao->vetorVeiculos[solucao->vetorVeiculos.size() - 1]->listaClientes.size() - 2;

            bool resultado = geraSolucao(solucao, instancia, alfa, vetorClienteBest, vetorClienteAux, sequencia, log, vetorCandidatos, parametroHeur1, parametroHeur2, true);

            if(resultado)
                return true;

            int numClientesD = solucao->vetorVeiculos[solucao->vetorVeiculos.size() - 1]->listaClientes.size() - 2;

            if(numClientesD < numClientes)
                i = 0;
            else
                i++;

        }
        else
            i++;


    }

    return false;

}
