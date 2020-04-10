//
// Created by igor on 07/04/2020.
//

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "Exception.h"
using namespace Movimentos;

ExceptionVeiculo exceptionVeiculo;
ExceptionPeso exceptionPeso;
ExceptionEscolhido exceptionEscolhido;

ResultadosRota Movimentos::inverteRota(auto itInicio, auto itFim, Solucao::ClienteRota *vetClienteRotaAux, int posicao, int peso, double poluicao,
                           double combustivel, const Instancia::Instancia *const instancia, int tipoVeiculo)
{
    //Percorre o veiculo na ordem inversa
    for(;;itFim--)
    {

        vetClienteRotaAux[posicao+1].cliente = (*itFim)->cliente;

        //Calcula horário, poluicao e combustivel
        if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, tipoVeiculo))
            return {.viavel = false};

        //Atualia poluicao e combustivel
        poluicao += vetClienteRotaAux[posicao+1].poluicao;
        combustivel += vetClienteRotaAux[posicao+1].combustivel;

        //Verifica capacidade
        if(!VerificaSolucao::verificaCombustivel(combustivel, tipoVeiculo, instancia))
            return {.viavel = false};

        //Atualia peso
        peso -= instancia->vetorClientes[(*itFim)->cliente].demanda;

        posicao++;
        if(itFim == itInicio)
            break;
    }

    return {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicao};

}

/*
 * Escolher uma rota e uma aresta. Percorrer as outras aresta e invertendo os clientes entre as duas arestas. As arestas não podem ser do mesmo cliente.
 *
 */
ResultadosRota Movimentos::mv_2optSwapIntraRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,Solucao::ClienteRota *vetClienteRotaBest,
                                    Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{

    int veiculoEscolhido;      //guarda o veiculo


    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);


    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    //Seleciona outro veiculo, enquanto o veiculo for vazio
    while(veiculo->listaClientes.size() <= 2)
    {
        veiculoEscolhido++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

        veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    }

    //Escolhe um cliente
    const int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;
    bool inicioRota = true;

    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0, combustivelAux, poluicaoAux;
    double combustivelBest, poluicaoBest = HUGE_VALF;
    int posicaoBestVeic, posicaoAux, pesoBest;

    int pesoVeic = veiculo->carga;
    int pesoAux;

    vetClienteRotaAux[0] = **veiculo->listaClientes.begin();
    int posicao = 0;
    int indexListaClientes = 1;
    bool clientePassouEscolhido = false;

    for(auto clienteIt = std::next(veiculo->listaClientes.begin(),1); clienteIt != veiculo->listaClientes.end(); )
    {
        //Cliente escolhido é a clienteIt.  duas arestas do mesmo cliente
        if(indexListaClientes == posicaoClienteEscolhido)
        {
            indexListaClientes += 2;
            clienteIt++;
            clienteIt++;

            clientePassouEscolhido = true;
            continue;
        }

        //Seleciona as possicões para inverter
        auto fim = clienteEscolhido;
        auto inicio = clienteIt;

        if(clientePassouEscolhido)
        {
            fim = clienteIt;
            inicio = clienteEscolhido;
        }

        //Inverte rota inicio fim.
        ResultadosRota resultadosRota = inverteRota(inicio, fim, vetClienteRotaAux, posicao, pesoVeic, poluicaoRotaParcial,
                                                    combustivelRotaParcial, instancia, veiculo->tipo);

        if(resultadosRota.viavel)
        {
            //Atualiza variaveis temporárias
            pesoAux = resultadosRota.peso;
            combustivelAux = resultadosRota.combustivel;
            poluicaoAux = resultadosRota.poluicao;
            posicaoAux = resultadosRota.posicaoVet;
            fim++;

            //Calcula o fim da rota
            resultadosRota = calculaFimRota(instancia, veiculo, fim, pesoAux, vetClienteRotaAux, posicaoAux, poluicaoAux, combustivelAux, -1, -1, veiculo->carga, 0);

            if(resultadosRota.viavel)
            {
                if(resultadosRota.poluicao < poluicaoBest)
                {
                    pesoBest = pesoAux;

                    copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota, &posicaoBestVeic);

                    if(pertubacao)
                        break;
                }
            }

        }

        if(!clientePassouEscolhido)
        {
            //Copia clienteIt para o vetor

            pesoVeic -= instancia->vetorClientes[(*clienteIt)->cliente].demanda;
            combustivelRotaParcial += (*clienteIt)->combustivel;
            poluicaoRotaParcial += (*clienteIt)->poluicao;

            vetClienteRotaAux[posicao+1] = **clienteIt;
            posicao++;

        }
        //Se clienteIt passou escolhido, ele NÃO é copiado para o vetor. Porque a cada interação de vet[posicao] com (vet[posicao+1], igual a clienteIt), aresta irá mudar!!

        clienteIt++;
        indexListaClientes++;

    }

    if(poluicaoBest == HUGE_VALF)
        return {.viavel = false};

    ResultadosRota resultadosRota;

    resultadosRota.viavel = true;
    resultadosRota.poluicao = poluicaoBest;
    resultadosRota.posicaoVet = posicaoBestVeic;
    resultadosRota.combustivel = combustivelBest;
    resultadosRota.peso = veiculo->carga;
    resultadosRota.veiculo = veiculo;
    resultadosRota.veiculoSecundario = NULL;

    return resultadosRota;

}

/*
 * Escolhe 2 rotas e duas arestas.
 * 0 - 1 - 2 *** 3 - 4 - 5 - 0
 * 0 - 6 - 7 *** 8 - 9 - 10 - 0
 *
 * 0 - 1 - 2 - 8 - 9 - 10 - 0
 * 0 - 6 - 7 - 3 - 4 - 5 - 0
 *
 */
ResultadosRota Movimentos::mv_2optSwapInterRotas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                     Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao)
{
    int veiculoEscolhido1;      //guarda o veiculo1
    int veiculoEscolhido2;      //guarda o veiculo2

    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil)
    {
        veiculoEscolhido1 = rand_u32() % (solucao->vetorVeiculos.size() - 1);
        veiculoEscolhido2 = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    }
    else
    {
        veiculoEscolhido1 = rand_u32() % solucao->vetorVeiculos.size();
        veiculoEscolhido2 = rand_u32() % solucao->vetorVeiculos.size();
    }
    auto *veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
    auto *veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];

    //Seleciona outro veiculo, enquanto o veiculo for vazio
    while(veiculo1->listaClientes.size() <= 2)
    {
        veiculoEscolhido1++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido1 = veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido1 = veiculoEscolhido1 % solucao->vetorVeiculos.size();

        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];

    }

    //Seleciona outro veiculo, enquanto o veiculo for vazio ou igual a veiculo1
    while((veiculoEscolhido1 == veiculoEscolhido2) || (veiculo2->listaClientes.size() <= 2))
    {
        veiculoEscolhido2++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido2 = veiculoEscolhido2 % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido2 = veiculoEscolhido2 % solucao->vetorVeiculos.size();

        veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];

    }

    //Escolhe um cliente
    const int posicaoClienteEscolhido = rand_u32() % (veiculo2->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;

    //Clientes a partir de cliente escolhido


    //Calcula peso dos clientes que serão retirados do veiculo2
    int pesoAux = 0;

    auto itAux = clienteEscolhido;
    itAux++;

    for(;itAux != veiculo2->listaClientes.end();itAux++)
        pesoAux += instancia->vetorClientes[(*itAux)->cliente].demanda;

    const int pesoClientesVeic2AposEscolhido = pesoAux;
    const int pesoClientesVeic2Parcial = veiculo2->carga - pesoAux;

    double combustivelAux, poluicaoAux;
    double combustivelBest, poluicaoBest = HUGE_VALF;

    double combustivelBestSec, poluicaoBestSec;

    int posicaoBestVeic, pesoBest, pesoBestSec;
    int posicaoBestSec;

    int pesoVeic1Parcial = 0;

    auto it = veiculo1->listaClientes.begin();
    vetClienteRotaAux[0] = **it;
    it++;
    vetClienteRotaAux[1] = **it;

    pesoVeic1Parcial += instancia->vetorClientes[(*it)->cliente].demanda;

    int posicao = 1;
    int k = 0;

    for(auto it : veiculo2->listaClientes)
    {

        vetClienteRotaSecundAux[k] = *it;
        if(it->cliente == (*clienteEscolhido)->cliente)
            break;

        k++;
    }
    int pesoVeic1DepClienteIt;
    auto ultimo = veiculo1->listaClientes.end(); //NULL
    ultimo--; // 0
    ultimo--; //ultimo cliente

    string strVeiculo1 = veiculo1->getRota();
    string strVeiculo2 = veiculo2->getRota();

    for(auto clienteIt = std::next(veiculo1->listaClientes.begin(), 1); clienteIt != veiculo1->listaClientes.end(); )
    {

        if((*clienteIt)->cliente == (*ultimo)->cliente)
            break;


        //Verificar o peso

        //Peso dos clientes depois de clienteIt
        pesoVeic1DepClienteIt = veiculo1->carga - pesoVeic1Parcial;

        //Verifica peso veiculo2
        if((pesoVeic1DepClienteIt + pesoClientesVeic2Parcial) < instancia->vetorVeiculos[veiculoEscolhido2].capacidade)
        {
            //Calcula os novos pesos

            int pesoNovoVeiculo2 = pesoVeic1DepClienteIt + pesoClientesVeic2Parcial;
            int pesoNovoVeiculo1 = (pesoClientesVeic2AposEscolhido) + pesoVeic1Parcial;

            //Verifica peso Veiculo1

            if(pesoNovoVeiculo1 < instancia->vetorVeiculos[veiculoEscolhido1].capacidade)
            {

                combustivelAux = 0.0;
                poluicaoAux = 0.0;
                pesoAux = pesoNovoVeiculo1;

                //Calculo veiculo1: Recalcular poluicao/combustivel relativo as cargas até clienteIt
                if(recalculaCombustivelPoluicaoCargas(veiculo1, &poluicaoAux, &combustivelAux, &pesoAux, instancia, vetClienteRotaAux, posicao))
                {

                    auto itAux = clienteEscolhido;
                    itAux++;

                    //cout<<"viculo1\n";

                    //Calcula fim da rota adicionando clientes do veiculo2
                    ResultadosRota resultadosRotaVeic1 = calculaFimRota_2OptInter(instancia, veiculo1, veiculo2, itAux, pesoAux, vetClienteRotaAux, posicao, poluicaoAux, combustivelAux, -1, -1, pesoNovoVeiculo1);

                    if(resultadosRotaVeic1.viavel)
                    {

                        combustivelAux = 0.0;
                        poluicaoAux = 0.0;
                        pesoAux = pesoNovoVeiculo2;

                        //Calculo veiculo2: Recalcular poluicao/combustivel relativo as cargas até clienteEscolhido
                        if(recalculaCombustivelPoluicaoCargas(veiculo2, &poluicaoAux, &combustivelAux, &pesoAux, instancia, vetClienteRotaSecundAux, posicaoClienteEscolhido))
                        {
                            itAux = clienteIt;
                            itAux++;

                            //cout<<"viculo2\n";

                            //Calcula fim da rota adicionando clientes do veiculo1
                            ResultadosRota resultadosRotaVeic2 = calculaFimRota_2OptInter(instancia, veiculo2, veiculo1, itAux, pesoAux, vetClienteRotaSecundAux, posicaoClienteEscolhido, poluicaoAux, combustivelAux,
                                                                               -1, -1, pesoNovoVeiculo2);

                            if(resultadosRotaVeic2.viavel)
                            {

                                if((resultadosRotaVeic1.poluicao + resultadosRotaVeic2.poluicao) < (poluicaoBest + poluicaoBestSec))
                                {
                                    //Atualiza a melhor solucao

                                    pesoBest = pesoNovoVeiculo1;
                                    copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRotaVeic1, &posicaoBestVeic);

                                    pesoBestSec = pesoNovoVeiculo2;
                                    copiaSolucao(vetClienteRotaSecundBest, vetClienteRotaSecundAux, &poluicaoBestSec, &combustivelBestSec, resultadosRotaVeic2, &posicaoBestSec);

                                    if(pertubacao)
                                        break;

                                }

                            }

                        }


                    }
                }

            }

        }

        //Insere o próximo clienteIt no vetClienteRotaAux

        clienteIt++;
        vetClienteRotaAux[posicao+1] = **clienteIt;
        pesoVeic1Parcial += instancia->vetorClientes[(*clienteIt)->cliente].demanda;
        posicao++;



    }

    if(poluicaoBest == HUGE_VALF)
        return {.viavel = false};



    ResultadosRota resultadosRota;

    resultadosRota.viavel = true;
    resultadosRota.poluicao = poluicaoBest;
    resultadosRota.posicaoVet = posicaoBestVeic;
    resultadosRota.combustivel = combustivelBest;
    resultadosRota.peso = pesoBest;
    resultadosRota.veiculo = veiculo1;
    resultadosRota.veiculoSecundario = veiculo2;
    resultadosRota.poluicaoSecundario = poluicaoBestSec;
    resultadosRota.combustivelSecundario = combustivelBestSec;
    resultadosRota.pesoSecundario = pesoBestSec;
    resultadosRota.posicaoVetSecundario = posicaoBestSec;

    return resultadosRota;

}

ResultadosRota Movimentos::mvIntraRotaInverteRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,Solucao::ClienteRota *vetClienteRotaBest,
                                      Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{
    //Variaveis combustivel/poluicao
    Solucao::Veiculo *veiculo = NULL;
    double combustivelBest, poluicaoBest;
    int posicaoBest, pesoBest;
    Solucao::Veiculo *bestVeiculo = NULL;
    int numVeic = solucao->vetorVeiculos.size();;

    if(solucao->veiculoFicticil)
        numVeic -= 1;

    //percorre todos os veiculos
    for(int i = 0; i < numVeic; ++i)
    {

        //Seleciona o veiculo
        veiculo = solucao->vetorVeiculos[i];

        //Verifica se o veiculo é vazio
        if(veiculo->listaClientes.size() <= 2)
            continue;

        //Pega o ultimo cliente do veiculo
        auto itFim = veiculo->listaClientes.end(); //NULL
        itFim--; //0
        itFim--; //ultimo cliente

        //Insere o deposito
        vetClienteRotaAux[0] = **veiculo->listaClientes.begin();

        ResultadosRota resultadosRota = inverteRota(veiculo->listaClientes.begin(), itFim, vetClienteRotaAux, 0, veiculo->carga, 0, 0, instancia, veiculo->tipo);

        if(resultadosRota.viavel)
        {


            if(bestVeiculo)
            {
                if((veiculo->poluicao - resultadosRota.poluicao) > (bestVeiculo->poluicao  - poluicaoBest))
                {
                    //cout << "Viavel.\n";
                    bestVeiculo = veiculo;
                    pesoBest = veiculo->carga;

                    copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota,
                                 &posicaoBest);

                    if ((resultadosRota.poluicao < veiculo->poluicao) || pertubacao)
                        break;
                }
            }
            else
            {
                bestVeiculo = veiculo;
                pesoBest = veiculo->carga;

                copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota,
                             &posicaoBest);

                if ((resultadosRota.poluicao < veiculo->poluicao) || pertubacao)
                    break;
            }

        }
    }

    if(bestVeiculo == NULL)
        return {.viavel = NULL};

    ResultadosRota resultadosRota;

    resultadosRota.viavel = true;
    resultadosRota.veiculo = bestVeiculo;
    resultadosRota.poluicao = poluicaoBest;
    resultadosRota.combustivel = combustivelBest;
    resultadosRota.peso = pesoBest;
    resultadosRota.posicaoVet = posicaoBest;

    return resultadosRota;
}

ResultadosRota Movimentos::mvTrocarVeiculos(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest,Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao)
{

    int veiculoEscolhido;
    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    Solucao::Veiculo *veiculo1 = solucao->vetorVeiculos[veiculoEscolhido];

    while(veiculo1->listaClientes.size() <= 2)
    {
        veiculoEscolhido++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido];
    }

    ResultadosRota resultadosRota{.viavel = false};
    int veiculoOriginal = veiculoEscolhido;

    int novoTipo;

    //Troca o tipo do veiculo
    while(!resultadosRota.viavel)
    {
        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido];

        if(veiculo1->listaClientes.size() > 2)
        {
            novoTipo = 0;

            if (veiculo1->tipo == 0)
                novoTipo = 1;

            vetClienteRotaBest[0] = **solucao->vetorVeiculos[novoTipo]->listaClientes.begin();

            auto it = veiculo1->listaClientes.begin(); //0
            it++;

            resultadosRota = calculaFimRota_2OptInter(instancia, solucao->vetorVeiculos[novoTipo], veiculo1, it, veiculo1->carga, vetClienteRotaBest, 0, 0, 0, -1, -1, veiculo1->carga);
        }
        veiculoEscolhido++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

        if(veiculoEscolhido == veiculoOriginal)
            break;

    }

    if(!resultadosRota.viavel)
        return {.viavel = false};

    int i = novoTipo;  //Pegar veiculos do tipo: novoTipo

    int numVeiculos = solucao->vetorVeiculos.size();

    if(solucao->veiculoFicticil)
        numVeiculos -= 1;

    Solucao::Veiculo *veiculo2;
    int novoTipoVeic1 = 0, novoTipoVeic2 = 1;

    if(veiculo1->tipo == 0)
    {
        novoTipoVeic1 = 1;
        novoTipoVeic2 = 0;
    }


    //Percorre os veiculos do tipo novoTipoVeic1 e tenta passar para novoTipoVeic2
    for(;i < numVeiculos; i += 2)
    {

        veiculo2 = solucao->vetorVeiculos[i];

        //Verifica a capacidade
        if(veiculo2->carga > instancia->vetorVeiculos[novoTipoVeic2].capacidade)
            continue;

        vetClienteRotaSecundBest[0] = **solucao->vetorVeiculos[novoTipoVeic2]->listaClientes.begin();

        auto it = veiculo2->listaClientes.begin(); //0
        it++; //primeiro cliente

        ResultadosRota resultadosRota2 = calculaFimRota_2OptInter(instancia, solucao->vetorVeiculos[novoTipoVeic2], veiculo2, it, veiculo2->carga, vetClienteRotaSecundBest, 0, 0, 0, -1, -1, veiculo2->carga);

        if(resultadosRota2.viavel)
        {
            if(((resultadosRota.poluicao + resultadosRota2.poluicao) < (veiculo1->poluicao + veiculo2->poluicao)) || pertubacao)
            {
                ResultadosRota resultados;

                resultados.trocaTipos = true;
                resultados.viavel = true;
                resultados.veiculo = veiculo2;
                resultados.poluicao = resultadosRota.poluicao;
                resultados.combustivel = resultadosRota.combustivel;
                resultados.posicaoVet = resultadosRota.posicaoVet;
                resultados.peso = veiculo1->carga;

                resultados.veiculoSecundario = veiculo1;
                resultados.pesoSecundario = veiculo2->carga;
                resultados.combustivelSecundario = resultadosRota2.combustivel;
                resultados.poluicaoSecundario = resultadosRota2.poluicao;
                resultados.posicaoVetSecundario = resultadosRota2.posicaoVet;

                return resultados;
            }
        }


    }


    return {.viavel = false};

}

ResultadosRota Movimentos::calculaFimRota_2OptInter(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, Solucao::Veiculo *veiculo2,
                                        auto proximoClienteIt, int peso, Solucao::ClienteRota *vetClienteRotaAux,
                                        int posicaoVet, double poluicao, double combustivel,
                                        const int clienteEscolhido, const int substituto, const int pesoTotal, const int maisclientes)
{

    //Calcular rota do ultimo cliente em vetCliente
    if(clienteEscolhido == 0)
        throw  exceptionEscolhido;
    for(auto itCliente = proximoClienteIt; itCliente != veiculo2->listaClientes.end(); )
    {
        auto antesPtrCliente = &vetClienteRotaAux[posicaoVet];
        auto ptrCliente = &vetClienteRotaAux[posicaoVet + 1];
        if((**itCliente).cliente == clienteEscolhido)
        {
            if(substituto != -1)
            {
                vetClienteRotaAux[posicaoVet+1].cliente = substituto;
            }
            else
            {
                itCliente++;
                vetClienteRotaAux[posicaoVet + 1] = **itCliente;
            }
        }
        else
        {
            vetClienteRotaAux[posicaoVet + 1] = **itCliente;
        }

        //Calcular rota entre posicaoVet e posicaoVet + 1
        if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet+1], instancia, peso, veiculo->tipo))
        {
            ResultadosRota resultados = {.viavel=false};
            //cout<<"mv recalculaRota. J.\n";
            return resultados;
        }

        //Atualiza combustivel e poluicao.
        peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet + 1].cliente].demanda;
        combustivel += vetClienteRotaAux[posicaoVet+1].combustivel;
        poluicao += vetClienteRotaAux[posicaoVet+1].poluicao;

        //Verifica peso
        if(peso < 0)
        {
            throw exceptionPeso;
        }

        //Verifica combustivel
        if(!VerificaSolucao::verificaCombustivel(combustivel, veiculo, instancia))
        {
            return {.viavel = false};
        }
        posicaoVet += 1;
        ++itCliente;
    }

    auto veiculoPtr = new Solucao::Veiculo(veiculo->tipo);



    veiculoPtr->listaClientes.erase(veiculoPtr->listaClientes.begin(), veiculoPtr->listaClientes.end());

    for(int i = 0; i <= posicaoVet; ++i)
    {
        auto cliente = new Solucao::ClienteRota;
        *cliente = vetClienteRotaAux[i];

        veiculoPtr->listaClientes.push_back(cliente);

    }

    auto clientePtr = *std::next(veiculoPtr->listaClientes.begin(), 1);


    veiculoPtr->poluicao = poluicao;
    veiculoPtr->combustivel = combustivel;
    veiculoPtr->carga = pesoTotal;

    if(!VerificaSolucao::verificaVeiculo(veiculoPtr, instancia))
    {
        delete veiculoPtr;

        throw exceptionVeiculo;
    }

    delete veiculoPtr;

    //Peso não se aplica.
    return {.poluicao = poluicao, .combustivel = combustivel,  .peso = -1, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};
}

