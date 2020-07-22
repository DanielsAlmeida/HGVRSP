// Created by igor on 27/04/2020.

#include "Movimentos_Paradas.h"
#include "mersenne-twister.h"
#include <iomanip>
#include "Movimentos.h"
#include "Construtivo.h"
#include "list"
#include "VerificaSolucao.h"
#include "Constantes.h"
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/tuple/tuple.hpp>
#include <chrono>

constexpr const bool Debug = false;

using namespace Movimentos_Paradas;

class ExceptionViabilidade : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Erro, fuc: criaRotas. \nMotivo: no nao possui antencessor valido\n";
    }
};



class ExceptionPossicaoVetor : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Erro, fuc: mvPercorreRotaParadas. \nMotivo: ultrapassou o tamanho maximo do vetor vetVetCliente[k]\n";
    }
};

class ExceptionRota : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Na funcao\nErro, fuc: Movimentos_Paradas::criaRota. \nMotivo: rota deve comecar e terminar com o deposito\n";
    }
};

class ExceptionIndex : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Erro, fuc: dijkstra. \nMotivo: index retornado de buscaBinaria : -1\n";
    }
};

class ExceptionNull : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: Realloc falhou\n";
    }
};

int comparadorTempo(const void *cliente1, const void *cliente2)
{
    return ((const Movimentos_Paradas::Cliente *) cliente1)->tempo >
           ((const Movimentos_Paradas::Cliente *) cliente2)->tempo;
}

bool Movimentos_Paradas::mvPercorreRotaParadas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                               Solucao::ClienteRota *vetClienteRota)
{
    std::setprecision(3);

    constexpr const bool BuscaLocal = true;

    bool melhoraSolucao = false;

    //Selecionar uma veiculo
    int veiculoEscolhido;

    if (solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);

    else
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size());

    const int VeiculoOriginal = veiculoEscolhido;
    Solucao::Veiculo *veiculo;

    std::unordered_map<int, No *> hashNo;
    std::map<int, int> mapVetor;
    std::list<Aresta *> listaAresta;
    VetCliente *vetVetCliente;

    int tam;

    auto funcLiberaMemoria = [&](const int tamVetor)
    {
        for (auto it:listaAresta)
        {
            delete it;
        }

        listaAresta.erase(listaAresta.begin(), listaAresta.end());

        for (auto it:hashNo)
        {
            delete it.second;
        }

        hashNo.erase(hashNo.begin(), hashNo.end());

/*        std::cout << "Solucoes viaveis: " << vetVetCliente[tamVetor - 1].tam << " de "
                  << vetVetCliente[tamVetor - 1].tamReal << '\n';*/

        for (int i = 0; i < tamVetor; ++i)
            delete[]vetVetCliente[i].vetCliente;

        delete[]vetVetCliente;

        mapVetor.erase(mapVetor.begin(), mapVetor.end());
    };

    //Laço do é responsável por criar e deletar No*, aresta*, vetVetCliente[]


    do
    {

        veiculo = solucao->vetorVeiculos[veiculoEscolhido];

        //Verifica se veiculo é vazio
        if (veiculo->listaClientes.size() <= 2)
        {
            //passa para o próximo veiculo
            veiculoEscolhido++;

            if (solucao->veiculoFicticil)
                veiculoEscolhido %= (solucao->vetorVeiculos.size() - 1);

            else
                veiculoEscolhido %= solucao->vetorVeiculos.size();

            continue;
        }

        string erro;
        erro = "";

        //Cria rotas para veiculo

        //Cria vetores
        const int tamVetor = veiculo->listaClientes.size();

        vetVetCliente = new VetCliente[tamVetor];

        //Cria vetores
        for (int i = 0; i < tamVetor; ++i)
        {
            tam = 2 * (i + 1) + 1;                                 //Tamanho maximo de nos para um vetor
            vetVetCliente[i].tamReal = tam;
            vetVetCliente[i].vetCliente = new Cliente[tam];
            vetVetCliente[i].tam = 0;

        }

        int posicao = 0;
        int nextId = 0;
        int linha = 0;
        bool inicio = true;
        int pesoAux = veiculo->carga;

        for (auto it : veiculo->listaClientes)
        {
            pesoAux -= instancia->vetorClientes[it->cliente].demanda;

            if (!inicio && it->cliente == 0)
                break;

            if (inicio)
                inicio = false;

            for (int i = 0; i < 2; ++i)
            {
                if (linha == 0)
                {
                    vetClienteRota[0].cliente = 0;
                    vetClienteRota[0].tempoSaida = instancia->vetorVeiculos[veiculo->tipo].inicioJanela;
                    vetClienteRota[0].combustivel = 0.0;
                    vetClienteRota[0].poluicao = 0.0;
                    vetClienteRota[0].combustivelRota = 0.0;
                    vetClienteRota[0].poluicaoRota = 0.0;

                    Movimentos::ResultadosRota resultadosRota = Movimentos::calculaFimRota(instancia, veiculo,
                                                                                           std::next(
                                                                                                   veiculo->listaClientes.begin(),
                                                                                                   1),
                                                                                           veiculo->carga,
                                                                                           vetClienteRota, 0, 0.0, .0,
                                                                                           -1, "paradas", nullptr, -1,
                                                                                           veiculo->carga, 0);

                    if (!resultadosRota.viavel)
                    {

                        int l = 0;
                        for (auto it:veiculo->listaClientes)
                        {
                            vetClienteRota[l] = *it;
                            l++;
                        }

                        resultadosRota.viavel = true;
                        resultadosRota.posicaoVet = veiculo->listaClientes.size() - 1;
                        resultadosRota.poluicao = veiculo->poluicao;
                        resultadosRota.combustivel = veiculo->combustivel;

                        resultadosRota.veiculo = veiculo;
                    }

                    Aresta *aresta = NULL;
                    No *no = NULL;

                    double combustivelParcial = 0.0;

                    //Cria os Nos
                    for (int j = 0; j <= resultadosRota.posicaoVet; ++j)
                    {
                        combustivelParcial += vetClienteRota[j].combustivel;

                        //Cria aresta
                        aresta = new Aresta{vetClienteRota[j].poluicao, vetClienteRota[j].combustivel,
                                            vetClienteRota[j].poluicaoRota, vetClienteRota[j].combustivelRota};
                        listaAresta.push_back(aresta);



                        //Copia os periodos percorridos
                        for (int k = 0; k < 5; ++k)
                        {

                            aresta->vetPeriodos[k] = vetClienteRota[j].percorrePeriodo[k];
                            aresta->vetTempo[k] = vetClienteRota[j].tempoPorPeriodo[k];
                            aresta->vetDistancia[k] = vetClienteRota[j].distanciaPorPeriodo[k];


                        }

                        bool final = false;

                        if (j != 0 && vetClienteRota[j].cliente == 0)
                        {
                            final = true;
                            vetClienteRota[j].tempoSaida = vetClienteRota[j].tempoChegada;
                        }

                        handle_type handle;

                        //Cria um no
                        no = new No{nextId, vetClienteRota[j].cliente, vetClienteRota[j].tempoSaida, -1, -1, HUGE_VAL,
                                    combustivelParcial, aresta,
                                    final, vetClienteRota[j].tempoChegada, false};

                        no->handle = handle;

                        //Atualiza vetVetCliente
                        vetVetCliente[j].tam = 1;
                        vetVetCliente[j].vetCliente[linha].id = nextId;
                        vetVetCliente[j].vetCliente[linha].aresta = *aresta;
                        vetVetCliente[j].vetCliente[linha].tempo = vetClienteRota[j].tempoSaida;

                        //Adiciona no a hash
                        hashNo[nextId] = no;
                        nextId++;

                        if (j != 0)
                            mapVetor[vetClienteRota[j].cliente] = j;

                    }

                    linha++;
                    break;

                } else
                {
                    //Encontra o id do no anterior
                    int idAnterior = vetVetCliente[posicao + 1].vetCliente[linha - 1].id;
                    No *noChegada = hashNo[idAnterior];

                    idAnterior = vetVetCliente[posicao].vetCliente[linha - 1].id;
                    No *no = hashNo[idAnterior];

                    int sumPeriodos = 0;

                    //Soma os periodos percorridos
                    for (auto ptr = noChegada->aresta->vetPeriodos; ptr < &noChegada->aresta->vetPeriodos[5]; ptr++)
                    {
                        if (*ptr)
                            sumPeriodos++;
                    }


                    double combustivelParcial = HUGE_VAL;

                    //Encontra o menor combustivel
                    for (int k = 0; k < linha; ++k)
                    {
                        int id = vetVetCliente[posicao].vetCliente[k].id;
                        No *noAux = hashNo[id];

                        if (noAux->combustivel < combustivelParcial)
                            combustivelParcial = noAux->combustivel;

                    }


                    Movimentos::ResultadosRota resultadosRota{.viavel = false};

                    if (sumPeriodos == 1)
                    {
                        //Calcula o novo tempoSaida como tempoSaida mais metade do tempo de viagem
                        double tempoSaidaAnt = no->tempoSaida;

                        double tempoChegada = noChegada->tempoChegada;
                        double tempoViagem = tempoChegada - tempoSaidaAnt;

                        if (tempoViagem < 0.0)
                        {
                            cout << "Erro tempo de viagem negativo\n";
                        }

                        vetClienteRota[posicao].tempoSaida = tempoSaidaAnt + tempoViagem / 2.0;


                        //Calcula nova rota
                        resultadosRota = Movimentos::calculaFimRota(instancia, veiculo,
                                                                    std::next(veiculo->listaClientes.begin(),
                                                                              posicao + 1),
                                                                    pesoAux, vetClienteRota, posicao, -1,
                                                                    combustivelParcial, -1, "paradas", nullptr, -1. - 1,
                                                                    -1, 0);


                    } else
                    {
                        //Encontra o primeiro periodo
                        int periodoInicial;

                        for (int k = 0; k < 5; k++)
                        {
                            if (vetVetCliente[posicao + 1].vetCliente[linha - 1].aresta.vetPeriodos[k])
                            {
                                periodoInicial = k;
                                break;
                            }
                        }

                        if ((periodoInicial + 1) < 5)
                        {

                            //Veiculo vai partir do inicio do periodo periodoInicial + 1
                            vetClienteRota[posicao].tempoSaida = instancia->vetorPeriodos[periodoInicial + 1].inicio;
                            vetClienteRota[posicao].poluicao = 0.0;
                            vetClienteRota[posicao].combustivel = 0.0;


                            resultadosRota = Movimentos::calculaFimRota(instancia, veiculo,
                                                                        std::next(veiculo->listaClientes.begin(),
                                                                                  posicao + 1), pesoAux,
                                                                        vetClienteRota, posicao, -1, combustivelParcial,
                                                                        -1, "paradas", nullptr, 0, 0, 0);
                        }


                    }

                    Aresta *aresta = NULL;

                    //Verifica viabilidade
                    if (resultadosRota.viavel)
                    {

                        //Cria os nos.
                        for (int k = posicao; k <= resultadosRota.posicaoVet; ++k)
                        {

                            combustivelParcial += vetClienteRota[k].combustivel;

                            //Cria aresta
                            aresta = new Aresta{vetClienteRota[k].poluicao, vetClienteRota[k].combustivel,
                                                vetClienteRota[k].poluicaoRota, vetClienteRota[k].combustivelRota};
                            listaAresta.push_back(aresta);


                            //Copia os periodos percorridos
                            for (int l = 0; l < 5; ++l)
                            {

                                aresta->vetPeriodos[l] = vetClienteRota[k].percorrePeriodo[l];
                                aresta->vetTempo[l] = vetClienteRota[k].tempoPorPeriodo[l];
                                aresta->vetDistancia[l] = vetClienteRota[k].distanciaPorPeriodo[l];
                            }

                            bool final = false;

                            if (k != 0 && vetClienteRota[k].cliente == 0)
                            {
                                final = true;
                                vetClienteRota[k].tempoSaida = vetClienteRota[k].tempoChegada;
                            }

                            handle_type handle;

                            //Cria um no
                            no = new No{nextId, vetClienteRota[k].cliente, vetClienteRota[k].tempoSaida, -1, -1,
                                        HUGE_VAL, combustivelParcial, aresta,
                                        final, vetClienteRota[k].tempoChegada, false};

                            no->handle = handle;

                            if (vetVetCliente[k].tam + 1 > vetVetCliente[k].tamReal)
                            {
                                funcLiberaMemoria(tamVetor);

                                ExceptionPossicaoVetor exception;
                                throw exception;
                            }

                            //Atualiza vetVetCliente
                            vetVetCliente[k].tam += 1;
                            vetVetCliente[k].vetCliente[linha].id = nextId;
                            vetVetCliente[k].vetCliente[linha].aresta = *aresta;
                            vetVetCliente[k].vetCliente[linha].tempo = vetClienteRota[k].tempoSaida;

                            //Adiciona no a hash
                            hashNo[nextId] = no;
                            nextId++;
                        }

                        linha++;
                    }


                }


            }

            posicao++;

        }



        //Ordenar vetores
        for (int i = 0; i < tamVetor; ++i)
        {
            std::qsort(vetVetCliente[i].vetCliente, vetVetCliente[i].tam, sizeof(Cliente), comparadorTempo);

        }


        for (int i = 0; i < tamVetor; ++i)
        {
            for (int j = 0; j < vetVetCliente[i].tam; ++j)
                erro += std::to_string(vetVetCliente[i].vetCliente[j].tempo) + " ";

            erro += '\n';
        }

        //Dijkstra
        int idResposta = dijkstra(&hashNo, &mapVetor, vetVetCliente, instancia->vetorVeiculos[veiculo->tipo].capacidade,
                                  &erro);


        //Verifica se encontrou um caminho
        if (idResposta != -1)
        {

            //Escrever a solucao

            No *no;
            //Solucao::Veiculo veiculoComParadas(*veiculo);
            no = hashNo[idResposta];

            if (no->poluicao < veiculo->poluicao)
            {
                melhoraSolucao = true;

                auto it = veiculo->listaClientes.end();
                it--; //0
                double tempoSaida;

                while (idResposta != -1)
                {

                    no = hashNo[idResposta];

                    (*it)->tempoChegada = no->tempoChegada;

                    if (no->final)
                    {
                        solucao->poluicao -= veiculo->poluicao;
                        veiculo->poluicao = no->poluicao;
                        veiculo->combustivel = no->combustivel;

                        solucao->poluicao += veiculo->poluicao;

/*                    cout<<no->cliente<<" predecessor tempoSaida: "<<no->predecessorTempoSaida<<'\n';
                    cout<<"predecessor: "<<no->predecessorId<<'\n'<<"Id: ";*/

                    } else
                    {
                        (*it)->tempoSaida = tempoSaida;

                    }



                    for (int i = 0; i < 5; ++i)
                    {


                        (*it)->percorrePeriodo[i] = (no->aresta)->vetPeriodos[i];
                        (*it)->tempoPorPeriodo[i] = (no->aresta)->vetTempo[i];
                        (*it)->distanciaPorPeriodo[i] = (no->aresta)->vetDistancia[i];

                    }

                    //cout<<no->id<<" ";

                    int index = instancia->retornaPeriodo(no->predecessorTempoSaida);

                    if ((!(*it)->percorrePeriodo[index]) && ((no->cliente != 0) || (no->final)))
                        cout << "Erro percorrePeriodo\n\n\n";

                    (*it)->combustivelRota = (no->aresta)->combustivelRotas;
                    (*it)->combustivel = (no->aresta)->combustivel;
                    (*it)->poluicaoRota = (no->aresta)->poluicaoRotas;
                    (*it)->poluicao = (no->aresta)->poluicao;

                    tempoSaida = no->predecessorTempoSaida;
                    idResposta = no->predecessorId;

                    it--;

                }

                if (VerificaSolucao::verificaVeiculo(veiculo, instancia))
                {

                } else
                {

                    std::cout << "**solucao errada!!!\n\n";

                    cout << veiculo->getRota() << '\n';

                    cout << "0 " << (*veiculo->listaClientes.begin())->tempoSaida << "\n";

                    for (auto it = veiculo->listaClientes.begin(); it != veiculo->listaClientes.end(); ++it)
                    {
                        auto prox = it;
                        prox++;

                        cout << (*it)->cliente << " " << (*prox)->cliente << " T (" << (*prox)->tempoChegada << ','
                             << (*prox)->tempoSaida << ") P C (" << (*prox)->poluicao << ',' << (*prox)->combustivel
                             << ")\n\n";

                        if ((*prox)->cliente == 0)
                            break;
                    }

                    ExceptionViabilidade exceptionViabilidade;

                    throw exceptionViabilidade;


                }

            }

        }



        //Deleta memoria
        funcLiberaMemoria(tamVetor);



        if (melhoraSolucao)
            return true;



        veiculoEscolhido++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido %= (solucao->vetorVeiculos.size() - 1);
        else
            veiculoEscolhido %= solucao->vetorVeiculos.size();


    } while (veiculoEscolhido != VeiculoOriginal);



    return false;


}

//Acha o caminho minimo e retorna o id do final da solucao
int Movimentos_Paradas::dijkstra(std::unordered_map<int, No *> *hashNo, std::map<int, int> *mapVetor,
                                 VetCliente *vetVetCliente, const double maxCombustivel, string *erro)
{

    //Cria uma lista de prioridade (Heap minimo)
    boost::heap::fibonacci_heap<HeapNo, boost::heap::compare<compare_HeapNo>> heap;

    double poluicao;
    handle_type handle;
    No *no, *noAux, *noProx;

    //Insere os nos na heap
    for (auto it:*hashNo)
    {
        no = it.second;

        poluicao = HUGE_VAL;

        if ((!no->final) && (no->cliente == 0))
        {
            poluicao = 0;
            no->poluicao = 0.0;
            no->combustivel = 0.0;
        }

        handle = heap.push({no->id, poluicao});
        no->handle = handle;


    }

    /* Retirar um no da heap(Fechar um no), pegar as arestas possiveis do no e atualizar os valores
     *
     * Condicao de parada: Fechar um no com cliente 0, final = true e poluicao diferent de Inf.
     */

    HeapNo heapNo;
    int coluna, linha;
    Aresta aresta;

    while (!heap.empty())
    {

        //Pega o topo da heap
        heapNo = heap.top();
        heap.pop();

        //Verifica se o topo da heap foi atualizado
        if (heapNo.poluicao == HUGE_VAL)
            break;

        //cout<<"Topo: "<<heapNo.poluicao<<"\n\n";

        //Recupera o no
        int id = heapNo.id;
        no = (*hashNo)[id];

        no->fechado = true;

        if (no->final)
        {
            //cout<<"Ultimo tempoSaida: "<<no->predecessorTempoSaida<<'\n';
            return no->id;
        }

        //Pelo tempoSaida achar index em vetVetCliente

        if (no->id != 0)
            coluna = (*mapVetor)[no->cliente];
        else
            coluna = 0;


        linha = buscaBinaria(vetVetCliente[coluna].vetCliente, no->tempoSaida, vetVetCliente[coluna].tam);

        if (linha == -1)
        {
            cout << "\n\n\n" << *erro << "\n";
            std::cout << '\n' << "Coluna " << coluna << '\n';
            std::cout << "Cliente: " << no->cliente << '\n';
            std::cout << "tempoSaida: " << no->tempoSaida << "\nVetor: ";
            for (int i = 0; i < vetVetCliente[coluna].tam; ++i)
                std::cout << vetVetCliente[coluna].vetCliente[i].tempo << "     ";

            std::cout << "\n\n\n\n";
            ExceptionIndex exceptionIndex;
            throw exceptionIndex;
        }

        id = no->id + 1;
        noProx = (*hashNo)[id];


        //Atualizar todos os nos apartir de linha. Setar NoId em coluna+1 e poluicao na heap, caso no seja atualizado
        for (; linha < vetVetCliente[coluna].tam; linha++)
        {

            //id = vetVetCliente[coluna + 1].vetCliente[linha].id;
            id = vetVetCliente[coluna].vetCliente[linha].id + 1;
            noProx = (*hashNo)[id];

            if (noProx->fechado)
                continue;

            id = vetVetCliente[coluna].vetCliente[linha].id;
            noAux = (*hashNo)[id];


            //aresta = vetVetCliente[coluna + 1].vetCliente[linha].aresta;
            aresta = *noProx->aresta;


            if ((no->poluicao + aresta.poluicao) < (*noProx->handle).poluicao)
            {
                //Verifica o combustivel

                if ((no->combustivel + aresta.combustivel) > maxCombustivel)
                    continue;

                //Atualiza no
                noProx->poluicao = no->poluicao + aresta.poluicao;
                noProx->combustivel = no->combustivel + aresta.combustivel;
                noProx->predecessorId = no->id;
                noProx->predecessorTempoSaida = noAux->tempoSaida;


                //cout << "Atualizando coluna +1 : " << coluna + 1 << "Linha: " << linha << " tempoSaida: " << noAux->tempoSaida << " poluicao: " << noProx->poluicao << " Aresta poluica: " << aresta.poluicao << '\n';

                //Atualiza Heap
                heap.decrease((noProx->handle), {(*noProx->handle).id, no->poluicao + aresta.poluicao});


            }


        }


    }

    return -1;


}

int Movimentos_Paradas::buscaBinaria(Cliente *vetCliente, double tempoSaida, int tam)
{
    int meio, inicio, fim;


    inicio = 0;
    fim = tam - 1;

    while (inicio <= fim)
    {

        meio = inicio + int((fim + 1 - inicio) / 2);

        if (vetCliente[meio].tempo == tempoSaida)
            return meio;

        if (tempoSaida > vetCliente[meio].tempo)
            inicio = meio + 1;
        else
            fim = meio - 1;

    }

    return -1;
}

bool
Movimentos_Paradas::criaRota(const Instancia::Instancia *const instancia, Solucao::ClienteRota *vetClienteRota, int tam,
                             const int peso, const int tipoVeiculo, double *combustivel, double *poluicao,
                             double *folga, TempoCriaRota *tempoCriaRota, double *vetLimiteTempo,
                             Solucao::ClienteRota *vetClienteRotaAux)
{

    if (tam == 2)
    {

        vetClienteRota[0].tempoSaida = (tipoVeiculo == 0 ? 0.0 : 0.5);

        if(poluicao)
            *poluicao = 0.0;

        if(combustivel)
            *combustivel = 0.0;

        return true;
    }
    else if(tam < 2)
    {
        cout<<"Erro, tamanho do veiculo menor que 2!!\n";
        exit(-1);
    }


    auto c_start = std::chrono::high_resolution_clock::now();

    if (vetClienteRota[0].cliente != 0 || vetClienteRota[tam - 1].cliente != 0)
        throw ExceptionRota();

    int pesoParcial = peso;

    // Verifica viabilidade da janela de tempo e a menor folga
    vetClienteRota[0].tempoSaida = (tipoVeiculo == 0 ? 0.0 : 0.5);
    vetClienteRotaAux[0] = vetClienteRota[0];

    double menor = HUGE_VAL;
    double combustivelPrimeiraRota, poluicaoPrimeiraRota;

    combustivelPrimeiraRota = poluicaoPrimeiraRota = 0.0;

    for(int i = 0; i < tam; ++i)
        vetClienteRotaAux[i].cliente = vetClienteRota[i].cliente;

    //static double vetLimiteTempo[20];

    for(int i = 0; i <= (tam - 2); ++i)
    {
        if(!Construtivo::determinaHorario(&vetClienteRota[i], &vetClienteRota[i + 1], instancia, pesoParcial, tipoVeiculo, NULL, NULL))
        {

            if(tempoCriaRota)
            {
                auto c_end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double> tempoCpu = c_end - c_start;
                tempoCriaRota->tempoCpu += tempoCpu.count();
            }

            return false;

        }




/*        for(int p = 0; p < 5; ++p)
        {
            vetClienteRotaAux[i+1].percorrePeriodo[p] = vetClienteRota[i+1].percorrePeriodo[p];
            vetClienteRotaAux[i+1].tempoPorPeriodo[p] = vetClienteRota[i+1].tempoPorPeriodo[p];

        }*/


        vetClienteRotaAux[i+1].swap(&vetClienteRota[i+1]);

        combustivelPrimeiraRota += vetClienteRota[i+1].combustivel;
        poluicaoPrimeiraRota += vetClienteRota[i+1].poluicao;

        if(folga)
        {
            double aux = instancia->vetorClientes[vetClienteRota[i+1].cliente].fimJanela - vetClienteRota[i+1].tempoSaida;

            if(aux < menor)
                menor = aux;
        }

        if(vetClienteRota[i+1].cliente != 0)
            vetLimiteTempo[i+1] = vetClienteRota[i+1].tempoSaida + 1.8;

        pesoParcial -= instancia->vetorClientes[vetClienteRota[i+1].cliente].demanda;
    }




    if(folga)
        *folga = menor;

    Solucao::ClienteRota solucaoPrimeiroC;
    solucaoPrimeiroC.swap(&vetClienteRota[1]);



    geraLimiteTempo(instancia, vetClienteRotaAux, tam, tipoVeiculo, vetLimiteTempo);

    vetClienteRota[1].swap(&solucaoPrimeiroC);
    pesoParcial = peso;

    if(VerificaSolucao::verificaCombustivel(combustivelPrimeiraRota, tipoVeiculo, instancia))
    {
        if((vetLimiteTempo[tam-1] - vetClienteRota[tam-1].tempoSaida) <  0.166666667)
        {
            *combustivel = combustivelPrimeiraRota;
            *poluicao = poluicaoPrimeiraRota;

            if(tempoCriaRota)
            {
                auto c_end = std::chrono::high_resolution_clock::now();

                std::chrono::duration<double> tempoCpu = c_end - c_start;
                tempoCriaRota->tempoCpu += tempoCpu.count();
            }

            return true;
        }
    }


    /*double maior = -HUGE_VAL;

    for(int i = 0; i < tam-1; ++i)
    {
        double aux = vetLimiteTempo[i] - vetClienteRota[i].tempoSaida;

        if(aux > maior)
            maior = aux;
    }




    if(tempoCriaRota)
    {
        if(maior > tempoCriaRota->maior)
            tempoCriaRota->maior = maior;
    }*/

    //Cria estruturas auxiliares
    std::unordered_map<int, No *> hashNo;
    //std::map<int, int> mapVetor;
    //std::list<Aresta *> listaAresta;
    int nextId = 0;
    pesoParcial = peso;
    const double IncrementoTempo = 0.666666667;                         //40 minutos
    const double TempoMax = 1.8;                                     //Tempo maximo de diferenca da primeira rota ate a ultima
    const int FreqVerificacao = 4; //6
    const int IncrementoEspaco = 5;
    const int TamRealVetor = 6;

    bool *ptrAresta, *ptrVet;

    VetCliente clienteI{0, TamRealVetor};
    VetCliente clienteJ{0, TamRealVetor};

    //Inicializa o deposito
/*    clienteI.vetCliente[0].tempo = (tipoVeiculo == 0 ? 0.0 : 0.5);
    clienteI.vetCliente[0].id = nextId;
    clienteI.tam = 1;*/


    No *no = new No(nextId, 0, (tipoVeiculo == 0 ? 0.0 : 0.5), -1, -1, 0.0, 0.0, NULL, false,
              -1, false);

    No *noAux = NULL;
    hashNo[nextId] = no;
    nextId++;

    Aresta *aresta;
    aresta = new Aresta;

    aresta->combustivelRotas = vetClienteRota[1].combustivelRota;
    aresta->combustivel = vetClienteRota[1].combustivel;
    aresta->poluicaoRotas = vetClienteRota[1].poluicaoRota;
    aresta->poluicao = vetClienteRota[1].poluicao;


    for(int i = 0; i < 5; ++i)
    {


        aresta->vetPeriodos[i] = vetClienteRota[1].percorrePeriodo[i];
        aresta->vetDistancia[i] = vetClienteRota[1].distanciaPorPeriodo[i];
        aresta->vetTempo[i] = vetClienteRota[1].tempoPorPeriodo[i];
    }

    no = new No(nextId, vetClienteRota[1].cliente, vetClienteRota[1].tempoSaida, 0, (tipoVeiculo == 0 ? 0.0 : 0.5), aresta->poluicao, aresta->combustivel, aresta,
            false, vetClienteRota[1].tempoChegada, false);

    clienteI.vetCliente[0].tempo = no->tempoSaida;
    clienteI.vetCliente[0].id = nextId;
    clienteI.vetCliente[0].aresta = *aresta;
    clienteI.tam = 1;

    hashNo[nextId] = no;

    no = NULL;
    aresta = NULL;
    nextId += 1;

    //Atualiza o peso. Saiada do cliente na posicao 1
    pesoParcial -= instancia->vetorClientes[vetClienteRota[1].cliente].demanda;

    auto funcLiberaMemoria = [&]()
    {


        for (auto it:hashNo)
        {
            delete it.second;
        }

        hashNo.erase(hashNo.begin(), hashNo.end());

        if(tempoCriaRota)
        {
            auto c_end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> tempoCpu = c_end - c_start;
            tempoCriaRota->tempoCpu += tempoCpu.count();
        }

        //delete []vetLimiteTempo;

    };


    //A cada interacao cria arcos i-j
    for (int i = 1; i < (tam - 1); ++i)
    {


        int j = i + 1;

        clienteJ.tam = 0;

        if (clienteI.tam == 0)
        {
            //Nao houve atualizacao
            funcLiberaMemoria();
            return false;

        }

        //Percorre todos os tempos, criando  arestas i-j
        for(int k = 0; k < clienteI.tam; ++k)
        {


            vetClienteRota[i].tempoSaida = clienteI.vetCliente[k].tempo;
            bool resultado = true;

            if(k != 0)
                resultado = Construtivo::determinaHorario(&vetClienteRota[i], &vetClienteRota[j], instancia, pesoParcial, tipoVeiculo, NULL, nullptr);




            //Verifica viabilidade da janela de tempo
            if(resultado)
            {
                //Cria um novo no

                //Cria uma aresta
                aresta = new Aresta;
                aresta->combustivelRotas = vetClienteRota[j].combustivelRota;
                aresta->combustivel = vetClienteRota[j].combustivel;
                aresta->poluicaoRotas = vetClienteRota[j].poluicaoRota;
                aresta->poluicao = vetClienteRota[j].poluicao;


                for(int l = 0; l < 5; ++l)
                {

                    aresta->vetPeriodos[l] = vetClienteRota[j].percorrePeriodo[l];
                    aresta->vetDistancia[l] = vetClienteRota[j].distanciaPorPeriodo[l];
                    aresta->vetTempo[l] = vetClienteRota[j].tempoPorPeriodo[l];
                }



                no = new No(nextId, vetClienteRota[j].cliente, vetClienteRota[j].tempoSaida, -1, clienteI.vetCliente[k].tempo, HUGE_VAL, 0.0, aresta, false,
                            vetClienteRota[j].tempoChegada, false);

/*                if(clienteI.vetCliente[k].tempo != no->predecessorTempoSaida)
                {
                    cout << "Tempos diferentes\n\n";
                    cout<<clienteI.vetCliente[k].tempo<<'\n';
                    cout<<no->predecessorTempoSaida<<"\n\n";
                }*/

                clienteJ.vetCliente[k].tempo = vetClienteRota[j].tempoSaida;
                clienteJ.vetCliente[k].id = nextId;
                clienteJ.vetCliente[k].aresta = *aresta;
                clienteJ.tam += 1;


                no->aresta = aresta;
                hashNo[nextId] = no;
                nextId++;

                no = NULL;
                aresta = NULL;


            }
            else
            {
                //Como o tempo k falhou, todos os outros tempos > k também vão falhar, porque : tempoSaida(k+1), ... , tempoSaida(tam-1) > tempoSaida(k)
                clienteI.tam = k;
                clienteJ.tam = k;
                break;

            }
        }

        if(clienteI.tam == 0)
        {
            funcLiberaMemoria();
            return false;
        }


        //Cria rotas intermediarias
        for(int k = 0; k < (clienteI.tam - 1); ++k)
        {
            if((clienteI.tam + 1) > TamRealVetor)
                break;

            double diferencaTempo = clienteI.vetCliente[k+1].tempo  -  clienteI.vetCliente[k].tempo;

            //Verifica o espaçamento
            if(diferencaTempo > IncrementoTempo)
            {
                //Cria uma nova aresta
                vetClienteRota[i].tempoSaida = clienteI.vetCliente[k].tempo + IncrementoTempo;

                if(Construtivo::determinaHorario(&vetClienteRota[i], &vetClienteRota[j], instancia, pesoParcial,
                                                 tipoVeiculo, NULL, nullptr))
                {

                    //Compara poluicao com k e k+1
                    if((vetClienteRota[j].poluicao != clienteJ.vetCliente[k].aresta.poluicao) || (vetClienteRota[j].poluicao != clienteJ.vetCliente[k + 1].aresta.poluicao))
                    {

                        //Verifica o espaco
                        if((clienteI.tam + 1) > clienteI.tamReal)
                        {
                            //Espaco nao é suficiente. Alacar mais espaco nos vetores
                            clienteI.vetCliente = (Cliente*)(realloc(clienteI.vetCliente, sizeof(Cliente) * (clienteI.tamReal + IncrementoEspaco)));
                            clienteJ.vetCliente =  (Cliente*)(realloc(clienteJ.vetCliente, sizeof(Cliente) * (clienteJ.tamReal + IncrementoEspaco)));

                            clienteI.tamReal += IncrementoEspaco;
                            clienteJ.tamReal += IncrementoEspaco;

                            if(!clienteI.vetCliente || !clienteJ.vetCliente)
                            {
                                throw ExceptionNull();
                            }

                            //cout<<"Aumentando vetor para "<<clienteI.tamReal<<'\n';
                        }


                        //Copia a partir de k+1 ate o final.
                        for(int l = clienteI.tam; l >= k+2; --l)
                        {
                            //cout<<"l = "<<l<<'\n';

                            clienteI.vetCliente[l].swap(clienteI.vetCliente[l-1]);
                            clienteJ.vetCliente[l].swap(clienteJ.vetCliente[l-1]);

                        }

                        //Cria um novo no
                        no = new No(nextId, vetClienteRota[i].cliente, vetClienteRota[i].tempoSaida, -1, -1, 0.0, 0.0, NULL, true,
                                    -1.0, false);



                        clienteI.vetCliente[k+1].id = nextId;
                        clienteI.vetCliente[k+1].tempo =  vetClienteRota[i].tempoSaida;

                        //Nao possui aresta, ja que nao é acessivel


                        hashNo[nextId] = no;
                        nextId++;


                        //Cria a aresta
                        aresta = new Aresta;
                        aresta->combustivelRotas = vetClienteRota[j].combustivelRota;
                        aresta->combustivel = vetClienteRota[j].combustivel;
                        aresta->poluicaoRotas = vetClienteRota[j].poluicaoRota;
                        aresta->poluicao = vetClienteRota[j].poluicao;



                        //Copia os periodos percorridos
                        for(int l = 0; l < 5; ++l)
                        {
                            aresta->vetPeriodos[l] = vetClienteRota[j].percorrePeriodo[l];
                            aresta->vetDistancia[l] = vetClienteRota[j].distanciaPorPeriodo[l];
                            aresta->vetTempo[l] = vetClienteRota[j].tempoPorPeriodo[l];
                        }

                        no = new No(nextId, vetClienteRota[j].cliente, vetClienteRota[j].tempoSaida, -1, vetClienteRota[i].tempoSaida, HUGE_VAL, 0.0, aresta,
                                    false, vetClienteRota[j].tempoChegada, false);


                        clienteJ.vetCliente[k+1].tempo = vetClienteRota[j].tempoSaida;
                        clienteJ.vetCliente[k+1].id = nextId;
                        clienteJ.vetCliente[k+1].aresta = *aresta;

                        hashNo[nextId] = no;
                        nextId++;

                        clienteI.tam++;
                        clienteJ.tam++;

                        aresta = NULL;
                        no = NULL;



                    }
                }

            }
        }


        /* **********************************************************************************************************************************************
         *
         * Verificar se a diferenca entre o tempo de saida da  primeira rota e da ultima rota é menor que 1.8.
         * Se for: Incrementar o tempo de saida da ultima rota em IncrementoTempo (10 Min)
         *
         ********************************************************************************************************************************************** */

        int k = 1;

        /* |********************************************************************************************** */
        //Enquanto o incremento no tempo de saida da ultima rota for menor que o tempoMax, adicionar uma nova aresta

        //while(((clienteI.vetCliente[clienteI.tam-1].tempo - clienteI.vetCliente[0].tempo) + k*IncrementoTempo) <= TempoMax)
        while((clienteI.vetCliente[clienteI.tam-1].tempo + k*IncrementoTempo) <= vetLimiteTempo[i])
        {
            if((clienteI.tam + 1) > TamRealVetor)
                break;

            //Cria aresta i-j
            vetClienteRota[i].tempoSaida = clienteI.vetCliente[clienteI.tam - 1].tempo + k * IncrementoTempo;
            bool resultado = Construtivo::determinaHorario(&vetClienteRota[i], &vetClienteRota[j], instancia, pesoParcial, tipoVeiculo, NULL, nullptr);

            if(resultado)
            {
                //Verifica se a poluicao é igual a ultima rota
                if(vetClienteRota[j].poluicao != clienteJ.vetCliente[clienteI.tam - 1].aresta.poluicao)
                {

                    //Verifica o espaco
                    if((clienteI.tam + 1) > clienteI.tamReal)
                        break;


                    //Cria um novo no
                    no = new No(nextId, vetClienteRota[i].cliente, vetClienteRota[i].tempoSaida, -1, -1.0, 0.0, 0.0, NULL, true,
                                -1.0, false);

                    clienteI.vetCliente[clienteI.tam].id = nextId;
                    clienteI.vetCliente[clienteI.tam].tempo =  vetClienteRota[i].tempoSaida;


                    //Nao possui aresta, ja que nao é acessivel

                    hashNo[nextId] = no;
                    nextId++;

                    //Cria a aresta
                    aresta = new Aresta;
                    aresta->combustivelRotas = vetClienteRota[j].combustivelRota;
                    aresta->combustivel = vetClienteRota[j].combustivel;
                    aresta->poluicaoRotas = vetClienteRota[j].poluicaoRota;
                    aresta->poluicao = vetClienteRota[j].poluicao;


                    //Copia os periodos percorridos
                    for(int l = 0; l < 5; ++l)
                    {

                        aresta->vetPeriodos[l] = vetClienteRota[j].percorrePeriodo[l];
                        aresta->vetDistancia[l] = vetClienteRota[j].distanciaPorPeriodo[l];
                        aresta->vetTempo[l] = vetClienteRota[j].tempoPorPeriodo[l];
                    }

                    //Cria um novo no
                    no = new No(nextId, vetClienteRota[j].cliente, vetClienteRota[j].tempoSaida, -1, vetClienteRota[i].tempoSaida, HUGE_VAL, 0.0, aresta, false,
                                vetClienteRota[j].tempoChegada, false);


                    clienteJ.vetCliente[clienteI.tam].tempo = vetClienteRota[j].tempoSaida;
                    clienteJ.vetCliente[clienteI.tam].id = nextId;
                    clienteJ.vetCliente[clienteI.tam].aresta = *aresta;


                    hashNo[nextId] = no;
                    nextId++;

                    clienteI.tam++;
                    clienteJ.tam++;

                    aresta = NULL;
                    no = NULL;

                    k = 0;


                }
            }
            else
                break;

            ++k;
        }

        /* *************************************************************************************************************************************************************************************************
         *
         * Realizar a atualizacao dos nos de j com a poluicao e combustivel parcial de i.
         * Verificar o combustivel
         *
         ************************************************************************************************************************************************************************************************ */

        bool atualizacao = false;

        //Verifica se foram criadas arestas
        if(clienteJ.tam == 0)
        {

            return false;
        }


        for(int p = 0; p < clienteI.tam; ++p)
        {

            //A partir de k, verificar todas as possivei arestas compativeis (tempo de saida)

            no = hashNo[clienteI.vetCliente[p].id];

            if(!no)
            {
                cout << "Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: id: " << clienteI.vetCliente[p].id << " nao foi encontrado \n\n";

                cout<<"Hash: ";
                for(auto it:hashNo)
                {
                    cout<<"("<<it.first<<" "<<it.second->id<<")  ";
                }

                cout<<'\n';

                funcLiberaMemoria();
                exit(-1);
            }

            if(no->final || no->poluicao == HUGE_VAL)
                continue;

            double tempo = clienteI.vetCliente[p].tempo;

            //int indice = buscaBinaria(clienteI.vetCliente, tempo, clienteI.tam);
            int indice = p;


            //Percorre as arestas
            for(;indice < clienteJ.tam; ++indice)
            {

                noAux = hashNo[clienteJ.vetCliente[indice].id];


                if(noAux == NULL)
                {
                    cout<<"Erro!!, Linha "<<__LINE__<<" fuc: Movimentos_Paradas::criaRota. \nMotivo: id: "<<clienteJ.vetCliente[indice].id<<" nao foi encontrado \n\n";
                    cout<<"i : "<<i<<", j : "<<j<<'\n';
                    cout<<"indice: "<<indice<<'\n';
                    cout<<"tam I: "<<clienteI.tam<<'\n';
                    cout<<"tam J "<<clienteJ.tam<<'\n';
                    cout<<"hash vazia: "<<hashNo.empty()<<'\n';


                    cout<<"Hash: ";
                    for(auto it:hashNo)
                    {
                        cout<<"("<<it.first<<" "<<it.second->id<<")  ";
                    }

                    cout<<'\n';

                    funcLiberaMemoria();
                    exit(-1);
                }

                 double combustivel = no->combustivel + clienteJ.vetCliente[indice].aresta.combustivel;

                 //Verifica o combustivel
                 if(VerificaSolucao::verificaCombustivel(combustivel, tipoVeiculo, instancia))
                 {
                     if(!noAux->aresta)
                     {
                         cout<<"Erro, noAux aresta igual a NULL\n";
                         cout<<"no id: "<<noAux->id<<"\n\n";

                         cout<<"id I: :";
                         for(int r = 0; r< clienteI.tam; ++r)
                             cout<<clienteI.vetCliente[r].id<<" ";

                         cout<<"\n\nid J: :";
                         for(int r = 0; r< clienteJ.tam; ++r)
                             cout<<clienteJ.vetCliente[r].id<<" ";

                         exit(-1);
                     }

                    //Verifica a poluicao
                    if((no->poluicao + noAux->aresta->poluicao) < noAux->poluicao)
                    {
                        //Atualiza o noAux
                        noAux->predecessorId = no->id;
                        noAux->combustivel = combustivel;
                        noAux->poluicao = no->poluicao + noAux->aresta->poluicao;

                        atualizacao = true;
                    }
                 }
            }

        }


        if constexpr (Debug)
        {


            cout << "cliente: " << vetClienteRota[i].cliente << " Tempos de saida I:\n\n";

            for (int t = 0; t < clienteI.tam; ++t)
                cout << clienteI.vetCliente[t].tempo << "\n";

            cout << "cliente: " << vetClienteRota[j].cliente << " Tempo de saida j e tempoSaida do predecessor\n\n";

            for (int t = 0; t < clienteJ.tam; ++t)
            {
                no = hashNo[clienteJ.vetCliente[t].id];

                cout << clienteJ.vetCliente[t].tempo << "  " << no->predecessorTempoSaida << "\n";
            }


            cout << "\n\n\n";

        }


        if(atualizacao == false)
        {   
            //Não existe solucao!!!
            //cout<<"Nao existe solucao\n\n";
            funcLiberaMemoria();
            return false;

        }

        pesoParcial -= instancia->vetorClientes[vetClienteRota[j].cliente].demanda;

        if(pesoParcial < 0)
        {
            funcLiberaMemoria();
            cout<<"Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: peso negativo\n\n";
            exit(-1);
        }

        clienteI.swap(&clienteJ);

    }

    if(pesoParcial != 0)
    {
        funcLiberaMemoria();
        cout<<"Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: peso nao eh 0\n\n";
        exit(-1);

    }

    //Copiar a solucao para o vetor

    double tempoSaida = -1;
    double best = HUGE_VAL;
    int index = tam -1 ;
    no = NULL;

    //Pega a melhor solucao
    for(int k = 0; k < clienteI.tam; ++k)
    {
        noAux = hashNo[clienteI.vetCliente[k].id];

        if(noAux->poluicao < best)
        {
            no = noAux;
            best = noAux->poluicao;
        }
    }


    //Verifica se existe solucao
    if(best == HUGE_VAL)
    {
        cout<<"Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: Ultima coluna nao foi atualizada\n\n";
        funcLiberaMemoria();
        exit(-1);
    }

    *poluicao = no->poluicao;
    *combustivel = no->combustivel;

    if(!VerificaSolucao::verificaCombustivel(*combustivel, tipoVeiculo, instancia))
    {
        cout<<"Erro! func Movimentos_Parados::criaRota.\nMotivo: combustivel a mais\n";
        funcLiberaMemoria();
        exit(-1);
    }

    if(no->cliente != 0)
    {
        cout<<"Erro ultimo clientete nao eh o deposito\n";
        exit(-1);
    }


    while(no)
    {
        if((no->cliente == 0 && no->final) || (no->id == 0))
        {
            vetClienteRota[index].poluicao = 0.0;
            vetClienteRota[index].poluicaoRota = 0.0;
            vetClienteRota[index].combustivel = 0.0;
            vetClienteRota[index].combustivelRota = 0.0;
            vetClienteRota[index].tempoSaida = tempoSaida;
            vetClienteRota[index].tempoChegada = -1.0;

            break;
        }

        aresta = no->aresta;

        if(!aresta)
        {
            cout<<"Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: ptr aresta igual a NULL\n\n";
            cout<<"no id: "<<no->id<<"\ncliente: "<<no->cliente<<'\n';
            funcLiberaMemoria();
            exit(-1);
        }


        vetClienteRota[index].poluicao = aresta->poluicao;
        vetClienteRota[index].poluicaoRota = aresta->poluicaoRotas;
        vetClienteRota[index].combustivel = aresta->combustivel;
        vetClienteRota[index].combustivelRota = aresta->combustivelRotas;
        vetClienteRota[index].tempoSaida = tempoSaida;
        vetClienteRota[index].tempoChegada = no->tempoChegada;


        for(int l = 0; l < 5; ++l)
        {
            vetClienteRota[index].percorrePeriodo[l] = aresta->vetPeriodos[l] ;
            vetClienteRota[index].distanciaPorPeriodo[l] = aresta->vetDistancia[l];
            vetClienteRota[index].tempoPorPeriodo[l] = aresta->vetTempo[l];
        }



        tempoSaida = no->predecessorTempoSaida;

        if(no->predecessorId != -1)
            no = hashNo[no->predecessorId];
        else
        {
            if (no->cliente != 0)
            {
                cout << "Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: peso negativo\n\n";
                funcLiberaMemoria();
                exit(-1);
            } else
                no = NULL;
        }

        index--;

    }

    //Cria um veiculo para verificar a solucao

    //*****************************************************************************************************************************************

    /*Solucao::Veiculo veiculoVerifica(tipoVeiculo);

    for(auto it : veiculoVerifica.listaClientes)
        delete it;

    veiculoVerifica.listaClientes.erase(veiculoVerifica.listaClientes.begin(), veiculoVerifica.listaClientes.end());

    Solucao::ClienteRota *clienteRota;

    //cria os clientes
    for(int i = 0; i < tam; ++i)
    {
        clienteRota = new Solucao::ClienteRota(vetClienteRota[i]);
        veiculoVerifica.listaClientes.push_back(clienteRota);
    }

    veiculoVerifica.carga = peso;
    veiculoVerifica.poluicao = *poluicao;
    veiculoVerifica.combustivel = *combustivel;

    if(!VerificaSolucao::verificaVeiculo(&veiculoVerifica, instancia))
    {
        cout<<"Erro, fuc: Movimentos_Paradas::criaRota. \nMotivo: Veiculo inviavel\n\n";
        funcLiberaMemoria();
        exit(-1);

    }*/

    //*****************************************************************************************************************************************

    if(tempoCriaRota)
    {
        tempoCriaRota->num += 1;
        tempoCriaRota->tamVet += clienteI.tam;
    }

    funcLiberaMemoria();
    return true;
}

void Movimentos_Paradas::geraLimiteTempo(const Instancia::Instancia *const instancia, Solucao::ClienteRota *vetClienteRota, const int tam,
                                         const int tipoVeiculo, double *vetLimiteTempo)
{


    //Inicializa o limite de tempo do deposito
    vetLimiteTempo[tam - 1] = 9.0;

    double menor;
    double janelaFinal;
    double diferenca;

    for(int i = tam - 2; i >= 1; --i)
    {
        janelaFinal = instancia->vetorClientes[vetClienteRota[i].cliente].fimJanela;
        menor = (janelaFinal < vetLimiteTempo[i+1]) ? janelaFinal : vetLimiteTempo[i+1];
        menor -= 0.5;


        if(menor < instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela)
        {
            menor = instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela + instancia->vetorClientes[vetClienteRota[i].cliente].tempoServico;
        }


        vetClienteRota[i].tempoSaida = menor;



        if(menor < 0.0)
        {
            cout<<"Arquivo: Movimentos_parados.cpp Erro linha: "<<__LINE__<<"\nTempo negativo\n";
            cout<<"tam: "<<tam<<'\n';
            cout<<"janelaFinal "<<janelaFinal<<'\n';
            cout<<"janelaInicio: "<<instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela<<'\n';
            cout<<"vetLimiteTempo[i + 1] "<<vetLimiteTempo[i + 1]<<'\n';
            cout<<"janelaInicio[i+1] "<<instancia->vetorClientes[vetClienteRota[i+1].cliente].inicioJanela<<'\n';


            exit(-1);
        }

        if(!Construtivo::determinaHorario(&vetClienteRota[i], &vetClienteRota[i+1], instancia, 0, tipoVeiculo, NULL, &diferenca))
        {
            do
            {
                menor -= diferenca;


                if(menor < 0.0)
                {
                    cout<<"Arquivo: Movimentos_parados.cpp Erro linha: "<<__LINE__<<"\nTempo negativo\n";
                    exit(-1);
                }

                if(menor < instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela)
                {
                    menor = instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela + instancia->vetorClientes[vetClienteRota[i].cliente].tempoServico;
                }

                vetClienteRota[i].tempoSaida = menor;
/*                cout<<"while\n";

                cout<<"c "<<vetClienteRota[i].cliente<<" tS "<<vetClienteRota[i].tempoSaida<<"\n";
                cout<<instancia->vetorClientes[vetClienteRota[i].cliente].inicioJanela<<"\n";
                cout<<"c "<<vetClienteRota[i+1].cliente<<"\n";
                cout<<instancia->vetorClientes[vetClienteRota[i].cliente].fimJanela<<'\n';
                cout<<"diferenca "<<diferenca<<"\n\n\n";*/

            }while (!Construtivo::determinaHorario(&vetClienteRota[i], &vetClienteRota[i+1], instancia, 0, tipoVeiculo, NULL, &diferenca));
        }

        if(menor < vetLimiteTempo[i])
            vetLimiteTempo[i] = menor;

    }



}