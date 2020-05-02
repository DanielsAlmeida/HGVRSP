//
// Created by igor on 27/04/2020.
// Adicionar o peso a mapPeso, 0 - i - j - k - 0, mapPeso[i] = peso de j a 0

#include "Movimentos_Paradas.h"
#include "mersenne-twister.h"
#include <iomanip>
#include "Movimentos.h"
#include "list"
#include "VerificaSolucao.h"
#include <boost/heap/fibonacci_heap.hpp>

using namespace Movimentos_Paradas;

class ExceptionViabilidade: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: mvPercorreRotaParadas. \nMotivo: resultado deveria ser viavel\n";
    }
};

class ExceptionPossicaoVetor: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: mvPercorreRotaParadas. \nMotivo: ultrapassou o tamanho maximo do vetor vetVetCliente[k]\n";
    }
};

class ExceptionIndex: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: dijkstra. \nMotivo: index retornado de buscaBinaria : -1\n";
    }
};

bool Movimentos_Paradas::mvPercorreRotaParadas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRota)
{
    std::setprecision(3);

    //Selecionar uma veiculo
    int veiculoEscolhido;

    if(solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);

    else
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size());

    const int VeiculoOriginal = veiculoEscolhido;
    Solucao::Veiculo *veiculo;

    std::unordered_map<int, No*> hashNo;
    std::map<int,int> mapVetor;
    std::list<Aresta*> listaAresta;
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
        if(veiculo->listaClientes.size() <= 2)
        {
            //passa para o próximo veiculo
            veiculoEscolhido++;

            if(solucao->veiculoFicticil)
                veiculoEscolhido %= (solucao->vetorVeiculos.size() - 1);

            else
                veiculoEscolhido %= solucao->vetorVeiculos.size();

            continue;
        }

        //Cria rotas para veiculo

        //Cria vetores
        const int tamVetor = veiculo->listaClientes.size();

        vetVetCliente = new VetCliente[tamVetor];

        //Cria vetores
        for(int i = 0; i < tamVetor; ++i)
        {
            tam = 2*(i+1) + 1;                                 //Tamanho maximo de nos para um vetor
            vetVetCliente[i].tamReal = tam;
            vetVetCliente[i].vetCliente = new Cliente[tam];
            vetVetCliente[i].tam = 0;

        }

        int posicao = 0;
        int nextId = 0;
        int linha = 0;
        bool inicio = true;
        int pesoAux = veiculo->carga;

        for(auto it : veiculo->listaClientes)
        {
            pesoAux -= instancia->vetorClientes[it->cliente].demanda;

            if(!inicio && it->cliente == 0)
                break;

            if(inicio)
                inicio = false;

            for(int i = 0; i < 2; ++i)
            {
                if (linha == 0)
                {
                    vetClienteRota[0].cliente = 0;
                    vetClienteRota[0].tempoSaida = it->tempoSaida;
                    vetClienteRota[0].combustivel = 0.0;
                    vetClienteRota[0].poluicao = 0.0;
                    vetClienteRota[0].combustivelRota = 0.0;
                    vetClienteRota[0].poluicaoRota = 0.0;

                    Movimentos::ResultadosRota resultadosRota = Movimentos::calculaFimRota(instancia, veiculo, std::next(veiculo->listaClientes.begin(), 1),
                                                                                           veiculo->carga, vetClienteRota, posicao, 0.0, .0, -1, "paradas", -1, -1);

                    if (!resultadosRota.viavel)
                    {
                        funcLiberaMemoria(tamVetor);
                        ExceptionViabilidade exception;
                        throw exception;
                    }

                    Aresta *aresta = NULL;
                    No *no = NULL;

                    double combustivelParcial = 0.0;

                    //Cria os Nos
                    for (int j = 0; j <= resultadosRota.posicaoVet; ++j)
                    {
                        combustivelParcial += vetClienteRota[j].combustivel;

                        //Cria aresta
                        aresta = new Aresta{vetClienteRota[j].poluicao, vetClienteRota[j].combustivel, vetClienteRota[j].poluicaoRota, vetClienteRota[j].combustivelRota};
                        listaAresta.push_back(aresta);

                        bool *ptrVetCliente = vetClienteRota[j].percorrePeriodo;
                        bool *ptrAresta = aresta->vetPeriodos;

                        //Copia os periodos percorridos
                        for (int k = 0; k < 5; ++k)
                        {
                            *ptrAresta = *ptrVetCliente;

                            ptrAresta++;
                            ptrVetCliente++;
                        }

                        bool final = false;

                        if(j != 0 && vetClienteRota[j].cliente == 0)
                        {
                            final = true;
                            vetClienteRota[j].tempoSaida = vetClienteRota[j].tempoChegada;
                        }

                        handle_type handle;

                        //Cria um no
                        no = new No{nextId, vetClienteRota[j].cliente, vetClienteRota[j].tempoSaida, -1, -1, HUGE_VAL, combustivelParcial, aresta,
                                    final, handle, vetClienteRota[j].tempoChegada, false};


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

                }
                else
                {
                    //Encontra o id do no anterior
                    int idAnterior = vetVetCliente[posicao+1].vetCliente[linha - 1].id;
                    No *noChegada = hashNo[idAnterior];

                    idAnterior = vetVetCliente[posicao].vetCliente[linha-1].id;
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
                    for(int k = 0; k < linha; ++k)
                    {
                        int id = vetVetCliente[posicao].vetCliente[k].id;
                        No *noAux = hashNo[id];

                        if(noAux->combustivel < combustivelParcial)
                            combustivelParcial = noAux->combustivel;

                    }


                    Movimentos::ResultadosRota resultadosRota{.viavel = false};

                    if (sumPeriodos == 1)
                    {
                        //Calcula o novo tempoSaida como tempoSaida mais metade do tempo de viagem
                        double tempoSaidaAnt = no->tempoSaida;

                        double tempoChegada = (noChegada->tempoSaida - instancia->vetorClientes[noChegada->cliente].tempoServico);
                        double tempoViagem = tempoChegada - tempoSaidaAnt;

                        vetClienteRota[posicao].tempoSaida = tempoSaidaAnt + tempoViagem / 2.0;

                        //Calcula nova rota
                        resultadosRota = Movimentos::calculaFimRota(instancia, veiculo, std::next(veiculo->listaClientes.begin(), posicao+1),
                                                                    pesoAux, vetClienteRota, posicao, -1 , combustivelParcial, -1, "paradas", -1. -1, -1, 0);


                    } else
                    {
                        //Encontra o primeiro periodo
                        int periodoInicial;

                        for(int k = 0; k < 5; k++)
                        {
                            if(vetVetCliente[posicao+1].vetCliente[linha-1].aresta.vetPeriodos[k])
                            {
                                periodoInicial = k;
                                break;
                            }
                        }

                        if((periodoInicial + 1 ) < 5)
                        {

                            //Veiculo vai partir do inicio do periodo periodoInicial + 1
                            vetClienteRota[posicao].tempoSaida = instancia->vetorPeriodos[periodoInicial + 1].inicio;
                            vetClienteRota[posicao].poluicao = 0.0;
                            vetClienteRota[posicao].combustivel = 0.0;

                            resultadosRota = Movimentos::calculaFimRota(instancia, veiculo, std::next(veiculo->listaClientes.begin(), posicao+1), pesoAux,
                                                                        vetClienteRota, posicao, -1, combustivelParcial, -1, "paradas");
                        }


                    }

                    Aresta *aresta = NULL;

                    //Verifica viabilidade
                    if (resultadosRota.viavel)
                    {

                        /*cout<<"tempoSaida: "<<vetClienteRota[posicao].tempoSaida<<" "<<vetClienteRota[posicao].cliente<<" - "<<vetClienteRota[posicao+1].cliente<<" pol: "<<
                        vetClienteRota[posicao+1].poluicao<<" comb: "<<vetClienteRota[posicao+1].combustivel<<"\n";*/


                        //Cria os nos.
                        for(int k = posicao; k <= resultadosRota.posicaoVet; ++k)
                        {

                            combustivelParcial += vetClienteRota[k].combustivel;

                            //Cria aresta
                            aresta = new Aresta{vetClienteRota[k].poluicao, vetClienteRota[k].combustivel, vetClienteRota[k].poluicaoRota, vetClienteRota[k].combustivelRota};
                            listaAresta.push_back(aresta);

                            bool *ptrVetCliente = vetClienteRota[k].percorrePeriodo;
                            bool *ptrAresta = aresta->vetPeriodos;

                            //Copia os periodos percorridos
                            for (int l = 0; l < 5; ++l)
                            {
                                *ptrAresta = *ptrVetCliente;

                                ptrAresta++;
                                ptrVetCliente++;
                            }

                            bool final = false;

                            if(k != 0 && vetClienteRota[k].cliente == 0)
                            {
                                final = true;
                                vetClienteRota[k].tempoSaida = vetClienteRota[k].tempoChegada;
                            }

                            handle_type handle;

                            //Cria um no
                            no = new No{ nextId, vetClienteRota[k].cliente, vetClienteRota[k].tempoSaida, -1, -1, HUGE_VAL, combustivelParcial, aresta,
                                        final, handle, vetClienteRota[k].tempoChegada, false};

                            if(vetVetCliente[k].tam + 1 > vetVetCliente[k].tamReal)
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

        //Dijkstra

        int idResposta = dijkstra(&hashNo, &mapVetor, vetVetCliente, instancia->vetorVeiculos[veiculo->tipo].capacidade);


        //Verifica se encontrou um caminho
        if(idResposta!=-1)
        {

            //Escrever a solucao

            No *no;
            //Solucao::Veiculo veiculoComParadas(*veiculo);
            no = hashNo[idResposta];

            if(no->poluicao < veiculo->poluicao)
            {

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

                    bool *ptrIt = (*it)->percorrePeriodo;
                    bool *ptrNo = (no->aresta)->vetPeriodos;

                    for (int i = 0; i < 5; ++i)
                    {
                        *ptrIt = *ptrNo;

                        ptrIt++;
                        ptrNo++;
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

/*            cout<<"\nRota: "<<veiculoComParadas.getRota()<<'\n';

            cout<<"Cliente \t Hora chegada \t\t Hora saida \t\t poluicao \t\t combustivel:\n\n";

            for(auto it:veiculoComParadas.listaClientes)
            {
                if(it->cliente != 0)
                    cout << it->cliente << "\t\t" << it->tempoChegada << "\t\t\t" << it->tempoSaida << "\t\t\t"<< it->poluicao << "\t\t\t" << it->combustivel << "\n";
                else
                {
                    if(it->tempoSaida == 0 || it->tempoSaida == 0.5)
                        cout << it->cliente << "\t\t" << "---" << "\t\t\t" << it->tempoSaida << "\t\t\t"<< it->poluicao << "\t\t\t" << it->combustivel << "\n";
                    else
                        cout << it->cliente << "\t\t" << it->tempoChegada << "\t\t\t" << "---" << "\t\t\t"<< it->poluicao << "\t\t\t" << it->combustivel << "\n";
                }

            }*/

                if (VerificaSolucao::verificaVeiculo(veiculo, instancia))
                {

                } else
                    std::cout << "solucao errada!!!\n\n";

            }

        }

        //Imprime solucoes

        /*linha = 0;
        int coluna = 0;

        for(auto it:veiculo->listaClientes)
            std::cout<<it->cliente<<"     ";

        std::cout<<"\n\n\n";

        for(int j = 0; j < vetVetCliente[tamVetor-1].tam; ++j)
        {

            while(linha >= vetVetCliente[coluna].tam)
            {
                coluna++;
            }

            for(int i = 0; i < coluna;++i)
                std::cout<<"     ";

            for(int i = coluna; i < tamVetor; ++i)
            {
                No *no = hashNo[vetVetCliente[i].vetCliente[linha].id];
                std::cout<<no->cliente<<"(Pol"<<vetVetCliente[i].vetCliente[linha].aresta.poluicao<<")     ";
                //<<",Pol"<<vetVetCliente[i].vetCliente[linha].aresta.poluicao
            }

            std::cout<<'\n';

            if((linha+1) >= vetVetCliente[coluna].tam)
            {
                coluna++;
            }

            linha++;

        }

        std::cout<<'\n';*/

        //Deleta memoria
        funcLiberaMemoria(tamVetor);

        //std::cout<<"********************************************************************************************************\n\n";



        veiculoEscolhido++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido %= (solucao->vetorVeiculos.size()-1);
        else
            veiculoEscolhido %= solucao->vetorVeiculos.size();



    }while(veiculoEscolhido != VeiculoOriginal);


    return false;

}

//Acha o caminho minimo e retorna o id do final da solucao
int Movimentos_Paradas::dijkstra(std::unordered_map<int, No*> *hashNo, std::map<int,int> *mapVetor, VetCliente *vetVetCliente, const double maxCombustivel)
{

    //Cria uma lista de prioridade (Heap minimo)
    boost::heap::fibonacci_heap<HeapNo, boost::heap::compare<compare_HeapNo>> heap;

    double poluicao;
    handle_type handle;
    No *no, *noAux, *noProx;

    //Insere os nos na heap
    for(auto it:*hashNo)
    {
        no = it.second;

        poluicao = HUGE_VAL;

        if((!no->final) && (no->cliente == 0))
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

    while(!heap.empty())
    {

        //Pega o topo da heap
        heapNo = heap.top();
        heap.pop();

        //Verifica se o topo da heap foi atualizado
        if(heapNo.poluicao == HUGE_VAL)
            break;

        //cout<<"Topo: "<<heapNo.poluicao<<"\n\n";

        //Recupera o no
        int id = heapNo.id;
        no = (*hashNo)[id];

        no->fechado = true;

        if(no->final)
        {
            //cout<<"Ultimo tempoSaida: "<<no->predecessorTempoSaida<<'\n';
            return no->id;
        }

        //Pelo tempoSaida achar index em vetVetCliente

        if(no->id != 0)
            coluna = (*mapVetor)[no->cliente];
        else
            coluna = 0;


        linha = buscaBinaria(vetVetCliente[coluna].vetCliente, no->tempoSaida, vetVetCliente[coluna].tam);

        if(linha == -1)
        {
            std::cout<<'\n'<<"Coluna "<<coluna<<'\n';
            std::cout<<"Cliente: "<<no->cliente<<'\n';
            std::cout<<"tempoSaida: "<<no->tempoSaida<<"\nVetor: ";
            for(int i = 0; i < vetVetCliente[coluna].tam;++i)
                std::cout<<vetVetCliente[coluna].vetCliente[i].tempo<<"     ";

            std::cout<<"\n\n\n\n";
            ExceptionIndex exceptionIndex;
            throw exceptionIndex;
        }

        id = vetVetCliente[coluna+1].vetCliente[linha].id;
        noProx = (*hashNo)[id];


        //Atualizar todos os nos apartir de linha. Setar NoId em coluna+1 e poluicao na heap, caso no seja atualizado
        for(; linha < vetVetCliente[coluna].tam; linha++)
        {

            id = vetVetCliente[coluna + 1].vetCliente[linha].id;
            noProx = (*hashNo)[id];

            if (noProx->fechado)
                continue;

            id = vetVetCliente[coluna].vetCliente[linha].id;
            noAux = (*hashNo)[id];


            aresta = vetVetCliente[coluna + 1].vetCliente[linha].aresta;


            if((no->poluicao + aresta.poluicao) < (*noProx->handle).poluicao)
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
    fim = tam-1;

    while(inicio <= fim)
    {

        meio = inicio + int((fim+1-inicio)/2);

        if(vetCliente[meio].tempo == tempoSaida)
            return meio;

        if(tempoSaida > vetCliente[meio].tempo)
            inicio = meio+1;
        else
            fim = meio-1;

    }

    return -1;
}