/* ***************************************************************************************************************************************************************************
 *
 * Corrigir Poluição => Dependencia do peso da poluição relativo a cargas.
 * Poluição errada. ???
 *
 * Erro: rota: 0 -> 1 -> 2 -> 0 . Para incluir o cliente 3, os clientes 0 e 1 precissão ser alterados, combustível e poluição.!!!
 *
 * ***************************************************************************************************************************************************************************/

#include "Construtivo.h"
#include <cmath>
#include <tuple>
#include "mersenne-twister.h"

using namespace Construtivo;
using namespace std;

void breakPoint()
{

}

Solucao::Solucao *Construtivo::reativo(const Instancia::Instancia *const instancia, bool (*comparador)(Instancia::Cliente &, Instancia::Cliente &), float *vetorAlfa,
                                       int tamAlfa, const int numInteracoes, const int numIntAtualizarProb)
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
    int numSolInviaveis = 0;

    //Inicializa a melhor solução
    vetorFrequencia[0] = 1;
    Solucao::Solucao *best = geraSolucao(instancia, comparador, vetorAlfa[0], vetorClienteBest, vetorClienteAux);
    vetorProbabilidade[0] = 1.0/tamAlfa;
    solucaoAcumulada[0] = best->poluicao;
    Solucao::Solucao *solucaoAux;
    int posicaoAlfa;
    int ultimaAtualizacao = -1;
    double poluicaoUltima = -1.0;

    //Inicializa os vetores de todos os alfas
    for(int i = 1; i < tamAlfa; ++i)
    {
        vetorProbabilidade[i] = 1/tamAlfa;
        vetorFrequencia[i] = 1;

        //Inicializa a solucaoAcumulada para o alfa
        solucaoAux = geraSolucao(instancia, comparador, vetorAlfa[i], vetorClienteBest, vetorClienteAux);
        solucaoAcumulada[i] = solucaoAux->poluicao + solucaoAux->poluicaoPenalidades;

        if(best->veiculoFicticil)
        {
            if(!solucaoAux->veiculoFicticil)
            {
                delete best;
                best = solucaoAux;
                solucaoAux = NULL;

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

            if(j >= tamAlfa)
                break;

            somaProb+= int(100.0 * vetorProbabilidade[j]);
            posicaoAlfa = j;

        }

        //Cria solução com o alfa escolhido
        solucaoAux = geraSolucao(instancia, comparador, vetorAlfa[posicaoAlfa], vetorClienteBest, vetorClienteAux);

        solucaoAcumulada[posicaoAlfa] += solucaoAux->poluicao;
        vetorFrequencia[posicaoAlfa] += 1;

        if(solucaoAux->veiculoFicticil)
            numSolInviaveis += 1;

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
                ultimaAtualizacao = i;
                poluicaoUltima = best->poluicao;
            }
            else
            {
                if(solucaoAux->poluicao  < best->poluicao)
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

    std::cout<<"Numero de solucoes inviaveis: "<<numSolInviaveis<<'\n';

    //Libera memória
    delete []vetorClienteBest;
    delete []vetorClienteAux;
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

    cout<<"Ultima Atualizacao: "<<ultimaAtualizacao<<'\n';
    cout<<"Poluicao: "<<poluicaoUltima<<'\n';
    cout<<"Veiculo Ficticio: "<<best->veiculoFicticil<<'\n';

    if(best->poluicao <= 0.1)
        cout<<"Poluicao = 0.0\n";

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

Solucao::Solucao * Construtivo::geraSolucao(const Instancia::Instancia *const instancia,
                                            bool (*comparador)(Instancia::Cliente &, Instancia::Cliente &), float alfa,
                                            Solucao::ClienteRota *vetorClienteBest,
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
    int minVeiculos = instancia->numVeiculos;
    Solucao::Solucao *solucao = new Solucao::Solucao(minVeiculos);


    Solucao::Veiculo *melhorVeiculo;
    std::list<Solucao::ClienteRota *>::iterator melhorPosicao, posicaoAux;
    double melhorPoluicao, melhorCombustivel, auxPoluicao, auxCombustivel;

    auto *candidato = new Solucao::ClienteRota;
    Instancia::Cliente clienteAux;

    Solucao::ClienteRota *vetorClienteSwap; //Ponteiro para trocar os vetores.

    int tamVetBest, tamVetAux, peso;

    double combustivelParcial, poluicaoParcial; //Acumula combustivel e poluicao da rota parcial
    auto clienteItAux = solucao->vetorVeiculos[0]->listaClientes.begin();
    bool viavel;
    uint32_t tamLista;
    uint32_t escolhido;

    /* *******************************************************************************************************************************
     *
     * Enquanto a lista de candidatos for diferente de vazio, escolha um cliente, calcule o acrescimo de poluição para cada
     * posição possível da solução, desde que a solução seja viável.
     *
     **********************************************************************************************************************************/
    while(!listaCandidatos.empty())
    {


        melhorPoluicao = HUGE_VALF;
        tamLista = listaCandidatos.size();                      //retira um cliente da lista
        tamLista = uint32_t(alfa*listaCandidatos.size()) + 1;   //Escolhe um valor aleatorio
        escolhido = rand_u32();
        escolhido = escolhido % tamLista;
        iteratorLisCand = listaCandidatos.begin();

        advance (iteratorLisCand, escolhido);                   //move iterator para escolhido
        clienteAux = (*iteratorLisCand);
        listaCandidatos.erase(iteratorLisCand);                 //Apaga escolhido da lista
        candidato->cliente = clienteAux.cliente;


        //Percorre os veículos
        for(auto veiculo = solucao->vetorVeiculos.begin(); veiculo != solucao->vetorVeiculos.end(); ++veiculo)
        {


            //Verifica se a capacidade max será atingida
            if((*veiculo)->carga + clienteAux.demanda > instancia->vetorVeiculos->capacidade)
                continue;

            //Variaveis para armazenar a soma do combustivel e da poluicao do veiculo até clienteIt
            poluicaoParcial = 0.0;
            combustivelParcial = 0.0;
            peso = (*veiculo)->carga + clienteAux.demanda;


            /* ************************************************************************************************************
             *Percorrer os clientes.
             *Candidato tentara ser inserido após clienteIt
             *
             **************************************************************************************************************/
            for(auto clienteIt = (*veiculo)->listaClientes.begin(); clienteIt != (*veiculo)->listaClientes.end(); )
            {

                tie(viavel, tamVetAux, auxPoluicao, auxCombustivel) = viabilidadeInserirCandidato(vetorClienteAux,
                                                                                  clienteIt,
                                                                                  instancia, candidato,
                                                                                  combustivelParcial,
                                                                                  poluicaoParcial,
                                                                                  *veiculo,  peso);

                if(viavel)
                {

                    if((auxPoluicao - (*veiculo)->poluicao) < (melhorPoluicao - (*melhorVeiculo).poluicao))
                    {
                        //if(auxPoluicao >= 1)
                        //    cout<<"auxPoluicao > 1\n\n\n";

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

                //Calcula poluicao e combustivel entre --clienteIt --> clienteIt
                if((*clienteIt)->cliente != 0)
                {

                    clienteItAux = clienteIt;
                    --clienteItAux;

                    //Poluicao da rota não muda
                    poluicaoParcial += (*clienteIt)->poluicaoRota;

                    //Poluicao das cargas muda
                    poluicaoParcial += VerificaSolucao::poluicaoCarga(instancia, (*veiculo)->tipo, peso, instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);

                    combustivelParcial += VerificaSolucao::combustivelCarga(instancia,(*veiculo)->tipo , peso, instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);

/* ************************************************************************************************************************************************************************* */
/* ************************************************************************************************************************************************************************* */
                    combustivelParcial += (*clienteIt)->combustivelRota;

                }

                peso -= instancia->vetorClientes[(*clienteIt)->cliente].demanda;


            }

        }

        /* Se encontrou uma solução, incluir candidato no veículo de posicao melhorVeiculo,
         *    Caso não encontrou uma solução, criar um novo veículo com o candidato.
         */


        if(melhorPoluicao == HUGE_VALF)
        {

           /* *********************************************************************************************************************
            *
            * Não conseguiu inserir o cliente em nenhuma posição. Cria um novo veiculo.
            *corrigir horário de saida do deposito, tempo de espera.
            *
            ***********************************************************************************************************************/

           Solucao::Veiculo *veiculo;

           if(solucao->veiculoFicticil == false)
           {
               solucao->veiculoFicticil = true;
               veiculo = new Solucao::Veiculo(2);
               solucao->vetorVeiculos.push_back(veiculo);

           }
           else
           {
               veiculo = solucao->vetorVeiculos[instancia->numVeiculos];
           }

            auto iterador = veiculo->listaClientes.begin();
            ++iterador;

            veiculo->listaClientes.insert(iterador, candidato);


            candidato = new Solucao::ClienteRota;


            solucao->poluicaoPenalidades = 0.05 * solucao->poluicao;

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
            melhorVeiculo->carga += instancia->vetorClientes[candidato->cliente].demanda;
            melhorVeiculo->combustivel = melhorCombustivel;

            candidato = new Solucao::ClienteRota;
            solucao->poluicao -= aux;
            solucao->poluicao += melhorPoluicao;

            if(!VerificaSolucao::verificaVeiculo(melhorVeiculo, instancia))
            {
                std::cout<<"Solucao incorreta.\n";
                breakPoint();


            }



        }
    }

    delete candidato;
    return solucao;


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

    //if(tipoVeiculo >2)
        //breakPoint();


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

            poluicaoAux += VerificaSolucao::poluicaoRota(instancia, tipoVeiculo, tempoRestantePeriodo * velocidade,cliente1->cliente, cliente2->cliente, periodoSaida );
            combustivelAux += VerificaSolucao::combustivelRota(instancia, tipoVeiculo, tempoRestantePeriodo * velocidade,cliente1->cliente, cliente2->cliente, periodoSaida );
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
                return true;
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
                                                  double poluicaoParcial, Solucao::Veiculo *veiculo, int peso)
{

    //inicialisa vetor de clientes
    vetorClientes[0] = **iteratorCliente;
    vetorClientes[1] = *candidato;

    int i;

    double poluicao = poluicaoParcial;
    double combustivel = combustivelParcial;



    //Verifica viabilidade com candidato
    if(determinaHorario(&vetorClientes[0], &vetorClientes[1], instancia, peso, veiculo->tipo))
    {
        ++iteratorCliente;

        poluicao += vetorClientes[1].poluicao;
        combustivel += vetorClientes[1].combustivel;



        //Percorre o resto dos clientes
        for(i = 1; (iteratorCliente) !=  veiculo->listaClientes.end(); ++i)
        {
            peso -= instancia->vetorClientes[(*iteratorCliente)->cliente].demanda;
            //Verifica  o combustível
            if(combustivel > instancia->vetorVeiculos[veiculo->tipo].combustivel)
                return {false, -1, -1, -1};

            //Adiciona o próximo cliente da lista na nova solução
            vetorClientes[i+1] = (**iteratorCliente);

            //Verifica viabilidade
            if(!determinaHorario(&vetorClientes[i], &vetorClientes[i + 1], instancia, peso,veiculo->tipo))
                return {false, -1, -1, -1};

            ++iteratorCliente;

            //Armazenar combustível e poluição
            poluicao += vetorClientes[i+1].poluicao;
            combustivel += vetorClientes[i+1].combustivel;
        }

        i +=1;

    }
    else
    {

        return {false, -1, -1, -1};
    }

    return {true, i, poluicao, combustivel};

}
