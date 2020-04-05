#include "Construtivo.h"
#include <cmath>
#include <tuple>
#include "mersenne-twister.h"
#include <unordered_map>
#include "Movimentos.h"

using namespace Construtivo;
using namespace std;

void breakPoint()
{

}

class ExceptioMvShifit: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro. \nMotivo: Movimento Shifit esta erado.\n";
    }
} exceptioMvShifit;

Solucao::Solucao * Construtivo::grasp(const Instancia::Instancia *const instancia, float *vetorAlfa, int tamAlfa,
                                      const int numInteracoes, const int numIntAtualizarProb, bool log,
                                      stringstream *strLog, const double parametro)
{

    unordered_map<int, int> hash;

    //Vetor guarda o resto da lista. São passados para construir a solução
    auto *vetorClienteBest = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetorClienteAux = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetClienteBestSecund = new Solucao::ClienteRota[instancia->numClientes+2];

    //Vetores para o reativo
    double *vetorProbabilidade = new double[tamAlfa];
    int *vetorFrequencia = new int[tamAlfa];
    double *solucaoAcumulada = new double[tamAlfa];
    double *vetorMedia = new double[tamAlfa];
    double *proporcao = new double[tamAlfa];
    Candidato *vetorCandidatos = new Candidato[instancia->numClientes];
    double somaProb, valAleatorio;
    int numSolInviaveis = 0;

    //Inicializa a melhor solução
    vetorFrequencia[0] = 1;
    Solucao::Solucao *best = geraSolucao(instancia, vetorAlfa[0], vetorClienteBest, vetorClienteAux,
                                         nullptr, false, vetorCandidatos, parametro);
    vetorProbabilidade[0] = 1.0/tamAlfa;
    solucaoAcumulada[0] = best->poluicao;
    Solucao::Solucao *solucaoAux;
    int posicaoAlfa;
    int ultimaAtualizacao = -1;
    double poluicaoUltima = -1.0;


    //Inicializa os vetores de todos os alfas
    for(int i = 1; i < tamAlfa; ++i)
    {
        vetorProbabilidade[i] = 1.0/tamAlfa;
        vetorFrequencia[i] = 1;

        //Inicializa a solucaoAcumulada para o alfa
        solucaoAux = geraSolucao(instancia, vetorAlfa[i], vetorClienteBest, vetorClienteAux, nullptr, false, vetorCandidatos, parametro);
        solucaoAcumulada[i] = solucaoAux->poluicao + solucaoAux->poluicaoPenalidades;

        if(best->veiculoFicticil)
        {
            if(!solucaoAux->veiculoFicticil)
            {
                delete best;
                best = solucaoAux;
                solucaoAux = NULL;

                int num = int (ceil(best->poluicao));

                hash[num]++;

            }
            else
            {
                if((solucaoAux->poluicao + solucaoAux->poluicaoPenalidades) < (best->poluicao + best->poluicaoPenalidades))
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
        }
        else
        {
            if(solucaoAux->veiculoFicticil)
            {
                delete solucaoAux;
                solucaoAux = NULL;
            }
            else
            {
                int num = int (ceil(solucaoAux->poluicao));

                hash[num]++;

                if(solucaoAux->poluicao  < best->poluicao)
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
        }


    }
    std::string texto;
    std::string sequencia;
    for(int i = 0; i < numInteracoes; ++i)
    {
        //cout<<"Interacao: "<<i<<"\n\n";

        //Atualiza probabilidade
        if((i%numIntAtualizarProb) == 0)
            atualizaProbabilidade(vetorProbabilidade, vetorFrequencia, solucaoAcumulada, vetorMedia, proporcao, tamAlfa, best->poluicao);

        //Escolher alfa, gerar solução, atualizar vetores, ....
        somaProb = posicaoAlfa = 0;

        valAleatorio = rand_u32() % 100;

        for(int j=0;somaProb < valAleatorio; ++j)
        {

            if(j >= tamAlfa)
            {

                j = 0;


            }

            somaProb+= (100.0 * vetorProbabilidade[j]);
            posicaoAlfa = j;

        }


        //Cria solução com o alfa escolhido

        sequencia = "";

        solucaoAux = geraSolucao(instancia, vetorAlfa[posicaoAlfa], vetorClienteBest, vetorClienteAux, &sequencia, log, vetorCandidatos, parametro);

        solucaoAcumulada[posicaoAlfa] += solucaoAux->poluicao + solucaoAux->poluicaoPenalidades;
        vetorFrequencia[posicaoAlfa] += 1;


        if(solucaoAux->veiculoFicticil)
            numSolInviaveis += 1;
        else
        {
            int num = int(ceil(solucaoAux->poluicao));
            hash[num]++;

/*
            auto resultado = Movimentos::mvInterRotasShift(instancia, solucaoAux, vetorClienteBest, vetorClienteAux, vetClienteBestSecund, false);


            //cout<<"Solucao.\n\n";

            while (resultado.viavel && (resultado.poluicao < solucaoAux->poluicao))
            {
                //cout<<"Viavel.\n";
                int l = 0;
                auto clienteRota = new Solucao::ClienteRota;
                resultado.veiculo->listaClientes.push_back(clienteRota);
                clienteRota = NULL;
                int peso = 0;
                for (auto clienteIt : resultado.veiculo->listaClientes)
                {

                    *clienteIt = vetorClienteBest[l];
                    peso += instancia->vetorClientes[(*clienteIt).cliente].demanda;

                    l += 1;
                }

                solucaoAux->poluicao -= resultado.veiculo->poluicao;


                resultado.veiculo->combustivel = resultado.combustivel;
                resultado.veiculo->poluicao = resultado.poluicao;
                resultado.veiculo->carga = resultado.peso;

                solucaoAux->poluicao += resultado.veiculo->poluicao;


                clienteRota = *resultado.veiculoSecundario->listaClientes.begin();
                resultado.veiculoSecundario->listaClientes.pop_front();

                delete clienteRota;
                l = 0;
                peso = 0;
                for (auto clienteIt : resultado.veiculoSecundario->listaClientes)
                {

                    *clienteIt = vetClienteBestSecund[l];
                    peso += instancia->vetorClientes[(*clienteIt).cliente].demanda;

                    l += 1;
                }


                //Atualiza veiculo secundario

                solucaoAux->poluicao -= resultado.veiculoSecundario->poluicao;
                resultado.veiculoSecundario->poluicao = resultado.poluicaoSecundario;
                solucaoAux->poluicao += resultado.veiculoSecundario->poluicao;

                resultado.veiculoSecundario->combustivel = resultado.combustivelSecundario;
                resultado.veiculoSecundario->carga = resultado.pesoSecundario;


                //VerificaSolucao::verificaVeiculo(resultado.veiculo, instancia);

                resultado = Movimentos::mvInterRotasShift(instancia, solucaoAux, vetorClienteBest, vetorClienteAux, vetClienteBestSecund, false);


            }*/



        }
            if(log)
            {

                texto+="***************************************************************************\n\n";

                texto += "Poluicao: " + to_string(solucaoAux->poluicao) + "\n";
                texto += "Interacao: " + to_string(i) + '\n';

                texto += "Sequencia: " + sequencia + "\nRota:\n\n";

                for(auto veiculo:solucaoAux->vetorVeiculos)
                {

                    for(auto it:(*veiculo).listaClientes)
                    {

                        texto+= to_string((*it).cliente) + " ";

                    }
                    texto+= "\n";

                }

                texto+="\n***************************************************************************\n";

            }


        //Atualiza best
        if(best->veiculoFicticil)
        {
            if(!solucaoAux->veiculoFicticil)
            {
                delete best;
                best = solucaoAux;
                solucaoAux = NULL;
                ultimaAtualizacao = i;
                poluicaoUltima = best->poluicao;



            }
            else
            {
                if((solucaoAux->poluicao + solucaoAux->poluicaoPenalidades) < (best->poluicao + best->poluicaoPenalidades))
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
        }
        else
        {
            if(solucaoAux->veiculoFicticil)
            {
                delete solucaoAux;
                solucaoAux = NULL;
            }
            else
            {
                double diferenca = (solucaoAux->poluicao - best->poluicao);
                if( diferenca < -0.001)
                {

                    delete best;
                    best = solucaoAux;
                    solucaoAux = NULL;
                    ultimaAtualizacao = i;
                    poluicaoUltima = best->poluicao;



                }
                else
                {
                    delete solucaoAux;
                    solucaoAux = NULL;
                }
            }
        }


    }

    //std::cout<<"Numero de solucoes inviaveis: "<<numSolInviaveis<<'\n';

    //Libera memória
    delete []vetorClienteBest;
    delete []vetorClienteAux;
    delete []vetClienteBestSecund;
    delete []vetorProbabilidade;
    delete []vetorFrequencia;
    delete []solucaoAcumulada;
    delete []vetorMedia;
    delete []proporcao;

    proporcao = NULL;
    vetorMedia = NULL;
    solucaoAcumulada = NULL;
    vetorFrequencia = NULL;
    vetorProbabilidade = NULL;
    vetorClienteAux = NULL;
    vetorClienteBest = NULL;
    vetClienteBestSecund = NULL;
    /*
    cout<<"Ultima Atualizacao: "<<ultimaAtualizacao<<'\n';
    cout<<"Poluicao: "<<poluicaoUltima<<'\n';
    cout<<"Veiculo Ficticio: "<<best->veiculoFicticil<<'\n';*/

    best->numSolucoesInv = numSolInviaveis;
    best->ultimaAtualizacao = ultimaAtualizacao;

    if(log)
    {
        (*strLog) << texto;


        (*strLog) << "\n\nPoluicao\tFrequencia\n";

        for (auto it : hash)
        {
            (*strLog) << it.first << "\t\t\t" << it.second << '\n';
        }
        (*strLog) << "\n\n";

    }
    return best;
}

void Construtivo::atualizaProbabilidade(double *vetorProbabilidade, int *vetorFrequencia, double *solucaoAcumulada, double *vetorMedia, double *proporcao, int tam, double melhorSolucao)
{

    double somaProporcoes = 0.0;

    //Calcular média

    for(int i = 0; i < tam; ++i)
    {
        vetorMedia[i] = solucaoAcumulada[i]/double(vetorFrequencia[i]);


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

void
Construtivo::atualizaPesos(double *beta, double *teta, int numClientes, const double parametro, const int k,
                           double *gama)
{

    *teta = abs(sin(2*M_PI*(k)/(parametro* numClientes)));
    *beta = 1 - *teta;

/*
    static int i = 0, j = 0;
    static const double F = numClientes * parametro;
    static const int max = int(numClientes);

    *beta = abs(sin(2.0 * M_PI * i) / F);
    *teta = (1-*beta) * abs(sin(2.0*M_PI*j)/F);
    *gama = 1.0 - *beta - *teta;

    if(j+1 < max)
        j += 1;
    else
    {   j = 0;
        if(i+1 < max)
            i += 1;
        else
            i = 0;
    }*/
}

Solucao::Solucao * Construtivo::geraSolucao(const Instancia::Instancia *const instancia, float alfa,
                                            Solucao::ClienteRota *vetorClienteBest,
                                            Solucao::ClienteRota *vetorClienteAux, string *sequencia, bool log,
                                            Construtivo::Candidato *vetorCandidatos, const double parametro)
{

    string texto;

    //Inicializa a lista de candidatos(Clientes)
    list<Instancia::Cliente> listaCandidatos;



    for (int i = 0; i < instancia->numClientes; ++i)
    {

        if (instancia->vetorClientes[i].cliente != 0)
            listaCandidatos.push_back(instancia->vetorClientes[i]);
    }




    //Cria uma solução com o mínimo de veiculos
    int minVeiculos = instancia->numVeiculos;
    Solucao::Solucao *solucao = new Solucao::Solucao(minVeiculos);
    double beta, teta, gama;
    int numClientesSol = 0;

    Solucao::Veiculo *melhorVeiculo;
    bool nullMelhorVeiculo = true;
    std::list<Solucao::ClienteRota *>::iterator melhorPosicao, posicaoAux;
    double melhorPoluicao, melhorCombustivel, auxPoluicao, auxCombustivel;
    double folgaRota, folgaRotaAux;
    double distRotaBest;

    // *************************************************************INSERE CLIENTE*************************************************************************
/*    solucao->vetorVeiculos[0]->listaClientes;

    Solucao::ClienteRota *cliente = new Solucao::ClienteRota;

    cliente->cliente = 7;

    auto aux = solucao->vetorVeiculos[0]->listaClientes.begin();
    ++aux;
    auto clieteAux10 = *aux;


    determinaHorario(*solucao->vetorVeiculos[0]->listaClientes.begin(), cliente, instancia, instancia->vetorClientes[7].demanda, 0);
    solucao->vetorVeiculos[0]->listaClientes.insert(aux, cliente);
    determinaHorario(cliente, clieteAux10, instancia, 0.0, 0);

    solucao->poluicao = cliente->poluicao;
    solucao->poluicao += (clieteAux10)->poluicao;
    solucao->vetorVeiculos[0]->combustivel = cliente->combustivel;
    solucao->vetorVeiculos[0]->poluicao = (clieteAux10)->poluicao;

    cliente = *aux;
    solucao->poluicao += cliente->poluicao;
    solucao->vetorVeiculos[0]->combustivel += cliente->combustivel;
    solucao->vetorVeiculos[0]->carga = instancia->vetorClientes[7].demanda;*/

    // *************************************************************INSERE CLIENTE*************************************************************************

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

    while (!listaCandidatos.empty())
    {

        atualizaPesos(&beta, &teta, instancia->numClientes, parametro, numClientesSol, &gama);
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

                        #if HEURIST_DIST
                            for(int i = 0; i+2 < tamVetAux; ++i)
                            {
                                double dist = instancia->matrizDistancias[vetorClienteAux[i].cliente][vetorClienteAux[i+1].cliente];
                                dist += instancia->matrizDistancias[vetorClienteAux[i+1].cliente][vetorClienteAux[i+2].cliente];

                                if(dist > disTemp)
                                    disTemp = dist;
                            }
                        #endif

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

                            #if HEURIST_DIST
                                distRotaBest = disTemp;
                            #endif

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

                            #if HEURIST_DIST
                                double distParcial = disTemp;
                                double distBest = distRotaBest;
                            #endif

                            if(aux < folgaRotaAux)
                                folgaSolParcial = aux;
                            else
                                folgaSolParcial = folgaRotaAux;

                            //Valor que eh incrementado na funcao objetivo
                            double incAtual = (auxPoluicao - (*veiculo)->poluicao);
                            double incBest = (melhorPoluicao - (*melhorVeiculo).poluicao);

                            #if not HEURIST_DIST
                                //Normaliza os valores
                                double normaAtual = pow(folgaSolParcial,2) + pow(incAtual, 2);
                                double normaBest =  pow(folgaBest,2) + pow(incBest, 2);

                            #else
                                double normaAtual = pow(folgaSolParcial,2) + pow(incAtual, 2) + pow(distParcial, 2);
                                double normaBest =  pow(folgaBest,2) + pow(incBest, 2) + pow(distBest, 2);
                            #endif

                            folgaSolParcial /=normaAtual;
                            incAtual /= normaAtual;

                            folgaBest /= normaBest;
                            incBest /= normaBest;

                            #if HEURIST_DIST

                                distParcial /= normaAtual;
                                distBest /= normaBest;

                            #endif

                            #if HEURIST_DIST
                                if ((beta*incAtual + teta*(1.0/folgaSolParcial) + gama*distParcial) < (beta*incBest + teta*(1.0/folgaBest) + gama*distBest))
                            #else
                                if ((beta*incAtual + teta*(1.0/folgaSolParcial)) < (beta*incBest + teta*(1.0/folgaBest) ))
                            #endif
                            {

                                melhorVeiculo = (*veiculo);
                                melhorPoluicao = auxPoluicao;
                                melhorCombustivel = auxCombustivel;
                                tamVetBest = tamVetAux;
                                melhorPosicao = clienteIt;

                                #if HEURIST_DIST
                                    distRotaBest = distParcial;
                                #endif

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

                #if HEURIST_DIST
                    ultimo->distanciaArcos = distRotaBest;
                #endif
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
            #if not HEURIST_DIST
                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoFolga);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista1Crit, rand_u32()%100, &tamLista2Crit, NULL, 0);

                std::qsort(vetorCandidatos, tamLista2Crit, sizeof(Candidato), compCandidato);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista2Crit, rand_u32()%100, NULL, ptrEscolhido, 1);

/* ************************************************************************************************************************************************************* */
            #else
                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoFolga);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista1Crit, rand_u32()%100, &tamLista2Crit, NULL, 0);

                std::qsort(vetorCandidatos, tamLista2Crit, sizeof(Candidato), compCandidato);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista2Crit, rand_u32()%100, &tamLista3Crit, NULL, 1);

                std::qsort(vetorCandidatos, tamLista3Crit, sizeof(Candidato), compCandidatoDist);
                escolhaProporcional(vetorProb, vetorCandidatos, tamLista3Crit, rand_u32()%100, NULL, ptrEscolhido, 2);

            #endif

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
    return solucao;

}
int Construtivo::compCandidato(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->incrementoPoluicao > ((const Candidato *)cand2)->incrementoPoluicao;
}

int Construtivo::compCandidatoFolga(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->folgaRota < ((const Candidato *)cand2)->folgaRota;
}

int Construtivo::compCandidatoDist(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->distanciaDeposito < ((const Candidato *)cand2)->distanciaDeposito;
}

void Construtivo::insereCandidato(Candidato *candidato, const Instancia::Instancia *instancia, Solucao::ClienteRota *vetCliente)
{



    double poluicaoParcial = 0.0;
    double combustivelParcial = 0.0;
    double aux, polAux, combAux;
    int posicaoVetor = 0;
    int cliente = candidato->candidato->cliente;
    int peso = candidato->veiculo->carga + instancia->vetorClientes[cliente].demanda;
    ItClienteRota clienteItAux;

    vetCliente[0] = **((*candidato->veiculo).listaClientes.begin());

    //Atualiza poluicao ate posicao

    for (auto clienteIt = (*candidato->veiculo).listaClientes.begin(); clienteIt != (*candidato->veiculo).listaClientes.end();)
    {


        //Calcula poluicao e combustivel entre --clienteIt --> clienteIt
        if ((*clienteIt)->cliente != 0)
        {

            clienteItAux = clienteIt;
            --clienteItAux;

            //Poluicao da rota não muda
            poluicaoParcial += (*clienteIt)->poluicaoRota;

            //Poluicao das cargas muda
            polAux = VerificaSolucao::poluicaoCarga(instancia, (*candidato->veiculo).tipo, peso,
                                                    instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);
            poluicaoParcial += polAux;

            combAux = VerificaSolucao::combustivelCarga(instancia, (*candidato->veiculo).tipo, peso,
                                                        instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);
            combustivelParcial += combAux;

            vetCliente[posicaoVetor] = **(clienteIt);
            vetCliente[posicaoVetor].combustivel = (*clienteIt)->combustivelRota + combAux;
            vetCliente[posicaoVetor].poluicao = (*clienteIt)->poluicaoRota + polAux;

            vetCliente[posicaoVetor].combustivelRota = (*clienteIt)->combustivelRota;
            vetCliente[posicaoVetor].poluicaoRota = (*clienteIt)->poluicaoRota;


            combustivelParcial += (*clienteIt)->combustivelRota;

        }
        if ((clienteIt) != (*candidato->veiculo).listaClientes.end())
        {


            peso -= instancia->vetorClientes[(*clienteIt)->cliente].demanda;


        }

        if(*clienteIt == *(candidato->posicao))
        {
            break;
        }

        ++clienteIt;

        if((*clienteIt)->cliente == 0)
            break;

        posicaoVetor += 1;
    }

    int i = 0;

    //Copia vetClientes para lista
    for(auto it = (*candidato->veiculo).listaClientes.begin(); ; it++, i++)
    {

        (**it) = vetCliente[i];

        if(*it == *(candidato->posicao))
        {
            break;
        }
    }
    auto it = candidato->posicao;
    it++;
    //Insere candidato
    (*candidato->veiculo).listaClientes.insert(it, candidato->candidato);
    candidato->candidato = NULL;



    auto clienteAux = candidato->posicao;
    clienteAux++;

    int k = (*candidato->posicao)->cliente;
    int j = (*clienteAux)->cliente;

    //Calcula poluica e combustivel para o resto da rota
    for(; clienteAux != (*candidato).veiculo->listaClientes.end(); ++candidato->posicao, clienteAux++)
    {

        int k = (*candidato->posicao)->cliente;
        int j = (*clienteAux)->cliente;

        determinaHorario(*candidato->posicao, *clienteAux, instancia, peso, candidato->veiculo->tipo);

        peso -= instancia->vetorClientes[(*clienteAux)->cliente].demanda;

        combustivelParcial += (*clienteAux)->combustivel;
        poluicaoParcial += (*clienteAux)->poluicao;


    }

    candidato->veiculo->poluicao = poluicaoParcial;
    candidato->veiculo->combustivel = combustivelParcial;
    candidato->veiculo->carga = candidato->veiculo->carga + instancia->vetorClientes[cliente].demanda;

    bool verificacao = VerificaSolucao::verificaVeiculo(candidato->veiculo, instancia);

    if(!verificacao)
        cout<<"LINHA "<<__LINE__<<" VEICULO ERADO!!!!!\n\n";


}


/* **********************************************************************************************************************************************
 *
 * Realiza a viagem de cliente1 para cliente2, considerando que cliente1 já esta inserido na solução.
 * Verifica a viabilidade (janela de tempo, tempo de espera maximo). insere informações da rota em cliente2.
 *
 * ***********************************************************************************************************************************************
 */
bool Construtivo::determinaHorario(Solucao::ClienteRota *cliente1, Solucao::ClienteRota *cliente2, const Instancia::Instancia *const instancia, const int peso, const int tipoVeiculo)
{


    double distancia = instancia->matrizDistancias[cliente1->cliente][cliente2->cliente];

    double horaPartida = cliente1->tempoSaida;
    double velocidade, tempoRestantePeriodo, horario, horaChegada, poluicaoAux = 0.0, poluicao = 0.0, combustivel = 0.0, combustivelAux = 0.0;
    double tempoEspera;
    int periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]
    int periodoChegada;
    static bool percorrePeriodo[5];

    for(int i = 0; i < 5; ++i)
        percorrePeriodo[i] = false;



    velocidade = instancia->matrizVelocidade[cliente1->cliente][cliente2->cliente][periodoSaida];




    if((distancia == 0.0) || (velocidade == 0.0))
        return false;


    do
    {

        if(periodoSaida >= 5)
            return false;

        velocidade = instancia->matrizVelocidade[cliente1->cliente][cliente2->cliente][periodoSaida];//velocidade -> km/h
        horario = horaPartida + (distancia / velocidade); //Horario de chegada considerando somente uma velocidade. Horario em horas
        periodoChegada = (instancia->retornaPeriodo(horario));

        percorrePeriodo[periodoSaida] = true;

        //Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
        if (periodoChegada != periodoSaida)
        {

            // Percorreu todo o periodoSaida e não chegou ao destino.

            tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;
            distancia -= tempoRestantePeriodo * velocidade;
            horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;


            poluicaoAux += VerificaSolucao::poluicaoRota(instancia, tipoVeiculo, tempoRestantePeriodo * velocidade,cliente1->cliente, cliente2->cliente, periodoSaida );
            combustivelAux += VerificaSolucao::combustivelRota(instancia, tipoVeiculo, tempoRestantePeriodo * velocidade,cliente1->cliente, cliente2->cliente, periodoSaida );
            periodoSaida += 1;


        }
        //horario é do mesmo periodo de periodoSaida, então o veículo chegou ao destino
        else
        {


            poluicaoAux += VerificaSolucao::poluicaoRota(instancia, tipoVeiculo, distancia,cliente1->cliente, cliente2->cliente, periodoSaida );
            combustivelAux += VerificaSolucao::combustivelRota(instancia, tipoVeiculo, distancia,cliente1->cliente, cliente2->cliente, periodoSaida );
            cliente2->poluicaoRota = poluicaoAux;
            cliente2->combustivelRota = combustivelAux;


            poluicaoAux += VerificaSolucao::poluicaoCarga(instancia, tipoVeiculo, peso,  instancia->matrizDistancias[cliente1->cliente][cliente2->cliente]);
            combustivelAux += VerificaSolucao::combustivelCarga(instancia, tipoVeiculo, peso,  instancia->matrizDistancias[cliente1->cliente][cliente2->cliente]);


            cliente2->poluicao = poluicaoAux;
            cliente2->combustivel = combustivelAux;



            distancia = 0;
            horaChegada = horario;

        }


    } while (distancia != 0.0);

        cliente2->tempoChegada = horaChegada;

        for(int i = 0; i < 5; ++i)
            cliente2->percorrePeriodo[i] =  percorrePeriodo[i];

        //Chegou após o inicio da janela
        if ((cliente2)->tempoChegada >= instancia->vetorClientes[(cliente2)->cliente].inicioJanela)
        {

            // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

            cliente2->tempoSaida = (cliente2)->tempoChegada + instancia->vetorClientes[(cliente2)->cliente].tempoServico;

            if (cliente2->tempoSaida <= instancia->vetorClientes[(cliente2)->cliente].fimJanela)
            {


                return true;

            }
            else
                return false;


        } else //Chegou antes do inicio da janela
        {

            cliente2->tempoSaida = instancia->vetorClientes[(cliente2)->cliente].inicioJanela + instancia->vetorClientes[(cliente2)->cliente].tempoServico;

            return true;

        }


}

/* *********************************************************************************************************************************************************************
 *
 * Tenta inserir candidato após iteradorCliente. Ajusta e verifica a viabilidade dos próximos clientes
 * combustivel e poluicao do veiculo até iteratorCliente (inclusive).
 * 1° parametro de retorno: viabilidade da solução.
 * 2° parametro de retorno: tamanho do vetor vetorClientes.
 * 3° parametro de retorno: poluicao.
 * 4° parametro de retorno: combustível.
 *
 ***********************************************************************************************************************************************************************/
TupleBID Construtivo::viabilidadeInserirCandidato(Solucao::ClienteRota *vetorClientes, ItClienteRota iteratorCliente,
                                                  const Instancia::Instancia *const instancia,
                                                  Solucao::ClienteRota *candidato, double combustivelParcial,
                                                  double poluicaoParcial, Solucao::Veiculo *veiculo, int peso,
                                                  int posicao, double *folga)
{

    //inicialisa vetor de clientes

    vetorClientes[posicao + 1] = *candidato;

    int i;

    double poluicao = poluicaoParcial;
    double combustivel = combustivelParcial;



    //Verifica viabilidade com candidato
    if(determinaHorario(&vetorClientes[posicao], &vetorClientes[posicao+1], instancia, peso, veiculo->tipo))
    {
        ++iteratorCliente;

        poluicao += vetorClientes[posicao+1].poluicao;
        combustivel += vetorClientes[posicao+1].combustivel;

        double aux =  (instancia->vetorClientes[vetorClientes[posicao+1].cliente].fimJanela - instancia->vetorClientes[vetorClientes[posicao+1].cliente].tempoServico) -vetorClientes[posicao +1].tempoChegada;



        if(aux < *folga)
            *folga = aux;

        //Percorre o resto dos clientes
        for(i = posicao+1; (iteratorCliente) !=  veiculo->listaClientes.end(); ++i)
        {
            peso -= instancia->vetorClientes[vetorClientes[i].cliente].demanda;


            //Adiciona o próximo cliente da lista na nova solução
            vetorClientes[i+1] = (**iteratorCliente);


            //Verifica viabilidade
            if(!determinaHorario(&vetorClientes[i], &vetorClientes[i + 1], instancia, peso,veiculo->tipo))
                return {false, -1, -1, -1};

            ++iteratorCliente;

            aux =  (instancia->vetorClientes[vetorClientes[i+1].cliente].fimJanela - instancia->vetorClientes[vetorClientes[i+1].cliente].tempoServico) - vetorClientes[i +1].tempoChegada;

            if(aux < *folga)
                *folga = aux;




            //Armazenar combustível e poluição
            poluicao += vetorClientes[i+1].poluicao;
            combustivel += vetorClientes[i+1].combustivel;

            //Verifica  o combustível
            if((instancia->vetorVeiculos[veiculo->tipo].combustivel - combustivel) <= -0.001)
                return {false, -1, -1, -1};
        }

        i +=1;

    }
    else
    {

        return {false, -1, -1, -1};
    }

    return {true, i, poluicao, combustivel};

}
