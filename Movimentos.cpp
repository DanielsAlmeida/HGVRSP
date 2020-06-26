//
// Created by igor on 10/02/2020.
//linha 1370

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "Exception.h"
#include "Movimentos_Paradas.h"

ExceptionVeiculo exceptionVeiculo;
ExceptionPeso exceptionPeso;
ExceptionEscolhido exceptionEscolhido;

using namespace Movimentos;

/* **************************************************************************************************************************************************************************************************
 * 0 - 5 - 8 - 10 - 20 - 7 - 0
 *
 * Escolhido := 10
 *
 * Até 8, rota (0 ... x - 10 -) não muda antes de x.
 *
 * A partir do 20, adicionar arco 8-20, com 7, 20-7 adicionando o peso do 10
 * ************************************************************************************************************************************************************************************************** */


ResultadosRota Movimentos::mvIntraRotaShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                            Solucao::ClienteRota *vetClienteRotaBest,
                                            Solucao::ClienteRota *vetClienteRotaAux, const bool percorreVeiculos,
                                            const bool percorreClientes, const bool pertubacao, double *vetLimiteTempo)
{
    string mvStr = "intraRotaShift";


    //Escolhe um veiculo

    int veiculoPrimeiro;

    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
        veiculoPrimeiro = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    else
        veiculoPrimeiro = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoPrimeiro];

    int posicaoClienteEscolhido = 1;
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;

    //ClienteEscolhido será reincerido em cada posição da rota

    bool inicio = false;
    int pesoAux;



    //Percorrer clientes da rota e acrecentar clienteEscolhido e recalcular rota


    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0; //Antes cliente escolhido
    double combustivelAux, poluicaoAux, combustivelBest, poluicaoBest = HUGE_VAL; //Rota completa
    int posicaoVetBest;

    int posicao = 0;
    bool inicioRota = true;
    bool passouEscolhido = false;
    Solucao::ClienteRota *nextClienteRota = NULL;


    Solucao::ClienteRota *clientePtr;

    //string seguencia = veiculo->printRota();

    int veiculoEscolhido = veiculoPrimeiro;

    do
    {

        veiculo = solucao->vetorVeiculos[veiculoEscolhido];

        if (veiculo->listaClientes.size() <= 2)
        {
            veiculoEscolhido++;

            if (solucao->veiculoFicticil)
                veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);
            else
                veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

            continue;
        }

        //Escolhe cliente

        posicaoClienteEscolhido = 1 + (rand_u32() % (veiculo->listaClientes.size() - 2));
        const int posicaoClienteOriginal = posicaoClienteEscolhido;


        do
        {

            clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
            escolhido = *clienteEscolhido;

            //Inicializa variaveis
            inicioRota = true;
            passouEscolhido = false;
            const int peso = veiculo->carga;
            combustivelRotaParcial = poluicaoRotaParcial = 0.0;
            posicao = 0;

            vetClienteRotaBest[0].cliente = (**veiculo->listaClientes.begin()).cliente;


            for (auto clienteIt = veiculo->listaClientes.begin(); clienteIt != veiculo->listaClientes.end();)
            {
                clientePtr = *clienteIt;


                //Verifica se chegou ao deposito
                if (!inicioRota && clientePtr->cliente == 0)
                    break;

                if (inicioRota)
                    inicioRota = false;

                //Atualiza o próximo cliente
                auto nextIt = clienteIt;
                nextIt++;
                nextClienteRota = *nextIt;



                //Verifica se o proximo cliente é igual a clienteEscolhido. ClienteEscolhido tem que ser pulado.
                if (nextClienteRota->cliente == (*clienteEscolhido)->cliente)
                {

                    clienteIt++;
                    clientePtr = *clienteIt;
                    nextIt = clienteIt;
                    nextIt++;
                    nextClienteRota = *nextIt;

                    if (!inicioRota && clientePtr->cliente == 0)
                        break;
                }

                //Verifica se cliente é igual a cliente escolhido
                if (clientePtr->cliente == (*clienteEscolhido)->cliente)
                {
                    //Add arco para next
                    vetClienteRotaBest[posicao + 1].cliente = nextClienteRota->cliente;
                    posicao += 1;
                    clienteIt++; //Pq eh Igual a escolhido
                    passouEscolhido = true;

                    continue;

                }


                vetClienteRotaBest[posicao + 1].cliente = (*clienteEscolhido)->cliente;

                auto prox = clienteIt;
                prox++;

                //Copiar o resto da solucao para vetClienteRotaAux
                int k = posicao + 2;

                for(;prox != veiculo->listaClientes.end(); )
                {
                    if((*prox)->cliente != (*clienteEscolhido)->cliente)
                    {   vetClienteRotaBest[k] = **prox;
                        ++k;
                    }

                    ++prox;

                }


                double combustivel, poluicao;
                bool resultado = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, k, peso, veiculo->tipo, &combustivel, &poluicao, NULL, NULL,
                                                              vetLimiteTempo, vetClienteRotaAux);

                if (resultado)
                {
                    //Verifica se é melhor do que a melhor solucao
                    if ((poluicao < veiculo->poluicao) || pertubacao)
                    {

                        poluicaoBest = poluicao;
                        combustivelBest = combustivel;
                        posicaoVetBest = k-1;


                        return {.poluicao = poluicaoBest, .combustivel = combustivelBest, .peso = peso, .viavel = true, .posicaoVet = posicaoVetBest, .veiculo = veiculo, .veiculoSecundario = NULL,
                                .poluicaoSecundario = -1.0, .combustivelSecundario = -1.0, .pesoSecundario = -1, .posicaoVetSecundario = -1};


                    }
                }




                /* ****************************************************************************************
                 *
                 * Adicionar arco vetCliente[posicao] - clienteIt(posicao+1)
                 *
                 **************************************************************************************** */

                vetClienteRotaBest[posicao + 1].cliente = nextClienteRota->cliente;

                posicao += 1;
                clienteIt++;
            }

            if (!percorreClientes)
                break;

            posicaoClienteEscolhido++;

            posicaoClienteEscolhido = ((posicaoClienteEscolhido) % (veiculo->listaClientes.size() - 2)) + 1;

        } while (posicaoClienteOriginal != posicaoClienteEscolhido);

        if (!percorreVeiculos)
            break;

        veiculoEscolhido++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);
        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

    } while (veiculoEscolhido != veiculoPrimeiro);


    return {.viavel = false};


}

/* ***********************************************************************************************************************************************************************************************
* Escolhe um veiculo da solucao, e um cliente do veiculo.
* Trocar a posicao do escolhido com todos os outros clientes do veiculo.
*
* Casos:
*
*      . 1º: Posicao do outro não passou escolhido:
*          Trocar possicao e recalcular rota. Tempo/poluicao/combustivel antes da possicao outro(exeto o arco x-outro) serão iguais, calcular fim da rota.
*
*      . 2º: Posicao do outro passou escolhido:
*          A rota a apartir da posicao escolhido deve ser recalculado.
*
*       0 - 5 - 9 - 10 - 2 - 15 - 11 - 8 - 0
*
*       Escolhido 15.
*
*       1º 15 <-> 5  : Calcular arco 0 - 15, e recalcular rota a partir 9 substituindo 15 por 5.                           1º
*       2° 15 <-> 9  : Arco 0-5 fica identico. Calcula arco 5 - 15, e recalcula rota a partir 10, substituindo 15 por 5.   1º
*       3º 15 <-> 10 : Arco 5-9 fica identico. Calcula arco 9 - 15, e recalcula rota a partir 2, substituindo 15 por 10.   1º
*       4º 15 <-> 2  : Arco 9-10 fica identico. Calcula arco 10-15 e recalcula rota a partir 15, substituindo 15 por 2.    1°
*       5º 15 <-> 15 .
*       6º 15 <-> 11: Calcula arco 2-11 e recalcula rota a partir do 11, substituindo 11 por 15.                           2°
*       7º 15 <-> 8 : Calcula arco 2-8 e recalcula rota a partir do 11, substituindo 8 por 15.                             2º
*
*********************************************************************************************************************************************************************************************** */
ResultadosRota Movimentos::mvIntraRotaSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                           Solucao::ClienteRota *vetClienteRotaBest,
                                           Solucao::ClienteRota *vetClienteRotaAux, const bool percorreVeiculos,
                                           const bool percorreClientes, bool pertubacao, double *vetLimiteTempo)
{

    //Escolhe um veiculo


    string mvStr = "mvIntraRotaSwap";

    int veiculoEscolhido;

    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);

    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    const int veiculoOriginal = veiculoEscolhido;

    /*while(true)
    {
        if(veiculo->listaClientes.size() > 2)
            break;
        else
        {
            veiculoEscolhido += 1;
            if(solucao->veiculoFicticil)
                veiculoEscolhido %= (solucao->vetorVeiculos.size() - 1);
            else
                veiculoEscolhido %= solucao->vetorVeiculos.size();

            *//* //Verifica se escolheu o veiculo ficticil
            if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
            {

                veiculoEscolhido = 0;
            }*//*

            veiculo = solucao->vetorVeiculos[veiculoEscolhido];
        }

    }*/

    //Escolhe cliente

    //int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);
    int posicaoClienteEscolhido = 1;
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto ptrEscolhido = *clienteEscolhido;

    bool inicio = false;
    int pesoAux;


    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0; //Antes cliente escolhido ou clienteIt
    double combustivelAux, poluicaoAux, combustivelBest, poluicaoBest = HUGE_VALF; //Rota completa
    int posicaoVetBest;

    int posicao = 0;
    bool inicioRota = true;
    bool passouEscolhido = false;
    auto proxEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido + 1);
    auto nextIt = veiculo->listaClientes.begin();
    Solucao::ClienteRota *nextClienteRota;

    do
    {

        veiculo = solucao->vetorVeiculos[veiculoEscolhido];

        if (veiculo->listaClientes.size() <= 2)
        {
            veiculoEscolhido++;

            if (solucao->veiculoFicticil)
                veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);
            else
                veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

            continue;
        }

        posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);

        const int PosicaoClienteInicial = posicaoClienteEscolhido;

        do
        {

            //Inicializa variaveis
            combustivelRotaParcial = poluicaoRotaParcial = 0.0;
            const int peso = veiculo->carga;

            passouEscolhido = false;

            //Escolhe cliente

            clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
            ptrEscolhido = *clienteEscolhido;
            posicao = 0;
            vetClienteRotaBest[0] = **veiculo->listaClientes.begin();

            Solucao::ClienteRota *clientePtr;
            string seguencia = veiculo->printRota();

            //Guarda o proximo cliente depois de escolhido.
            proxEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido + 1);
            nextIt = veiculo->listaClientes.begin();


            for (auto clienteIt = std::next(veiculo->listaClientes.begin(), 1); clienteIt != veiculo->listaClientes.end();)
            {
                clientePtr = *clienteIt;

                //Verifica se chegou ao deposito
                if (clientePtr->cliente == 0)
                    break;


                //Atualiza o próximo cliente
                if (!passouEscolhido)
                {
                    nextIt = clienteIt;
                    nextIt++;
                    nextClienteRota = *nextIt;
                }

                if (clientePtr->cliente == (*clienteEscolhido)->cliente)
                {
                    //posicao não é mais incrementado. posicao contem cliente antes de escolhido. peso tambem não muda mais.

                    clienteIt++;
                    passouEscolhido = true;

                    //Atualiza ponteiros

                    clientePtr = *clienteIt;
                    nextIt = clienteIt;
                    //nextIt++;
                    nextClienteRota = *nextIt;

                    continue;
                }

                if (!passouEscolhido)
                {
                    //Criar o arco posicao - escolhido

                    vetClienteRotaBest[posicao + 1].cliente = (*clienteEscolhido)->cliente;

                    int posicaoAux = posicao + 2;

                    for(auto it = nextIt; it != veiculo->listaClientes.end(); ++it)
                    {
                        if((*it)->cliente != (*clienteEscolhido)->cliente)
                            vetClienteRotaBest[posicaoAux].cliente = (*it)->cliente;
                        else
                            vetClienteRotaBest[posicaoAux].cliente = clientePtr->cliente;

                        ++posicaoAux;

                    }

                    //Calcular rota a partir de next e substituir candidato por clienteIt

                    double poluicao, combustivel;

                    bool resultado = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicaoAux, peso,
                                                                  veiculo->tipo, &combustivel, &poluicao, NULL,
                                                                  NULL, vetLimiteTempo , vetClienteRotaAux);

                    if (resultado)
                    {
                        //Se a nova solucao é melhor do que a solucao atual, atualiza solucao
                        if ((poluicao < veiculo->poluicao) || pertubacao)
                        {


                            return {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicaoAux-1, .veiculo = veiculo, .veiculoSecundario = NULL,
                                            .poluicaoSecundario = -1.0, .combustivelSecundario = -1.0, .pesoSecundario = -1, .posicaoVetSecundario = -1};
                        }
                    }




                    //Adicionar clienteIt ao vetClienteRotaAux.
                    vetClienteRotaBest[posicao + 1].cliente = clientePtr->cliente;

                    posicao += 1;
                    clienteIt++;

                    continue;

                } else
                {

                    //Criar arco entre posicao - clientePtr. Calcular o fim da rota substituindo, clientePtr por clienteEscolhido. Posicao não é atualizado.
                    vetClienteRotaBest[posicao + 1].cliente = clientePtr->cliente;


                    int posicaoAux = posicao + 2;

                    for(auto it = proxEscolhido; it != veiculo->listaClientes.end(); ++it)
                    {
                        if((*it)->cliente != clientePtr->cliente)
                            vetClienteRotaBest[posicaoAux].cliente = (*it)->cliente;
                        else
                            vetClienteRotaBest[posicaoAux].cliente = ptrEscolhido->cliente;
                        ++posicaoAux;
                    }


                    double poluicao, combustivel;

                    bool resultado = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicaoAux, peso,
                                                                  veiculo->tipo, &combustivel, &poluicao, NULL,
                                                                  NULL, vetLimiteTempo, vetClienteRotaAux);


                    //Verifica viabilidade
                    if (resultado)
                    {

                        if ((poluicao < veiculo->poluicao) || pertubacao)
                        {

                                return {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicaoAux-1, .veiculo = veiculo, .veiculoSecundario = NULL,
                                        .poluicaoSecundario = -1.0, .combustivelSecundario = -1.0, .pesoSecundario = -1, .posicaoVetSecundario = -1};
                        }
                    }




                    clienteIt++;

                }

            }

            if (!percorreClientes)
                break;

            posicaoClienteEscolhido++;

            posicaoClienteEscolhido = 1 + posicaoClienteEscolhido % (veiculo->listaClientes.size() - 2);

        } while (posicaoClienteEscolhido != PosicaoClienteInicial);

        if (!percorreVeiculos)
            break;

        veiculoEscolhido++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);
        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();


    } while (veiculoEscolhido != veiculoOriginal);


    return {.viavel = false};


}

namespace teste
{
    void breakPoint()
    {

    }
}

/*
 * Encontre dois veiculos, escolha um cliente (por exemplo) do segundo veiculo. Verificar se é possível remover esse cliente, e tentar inserir no primenro veiculo.
 *
 *  Sendo modificado para funcionar com solucoes inviaveis como busca OK
 */

ResultadosRota Movimentos::mvInterRotasShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                             Solucao::ClienteRota *vetClienteRotaBest,
                                             Solucao::ClienteRota *vetClienteRotaAux,
                                             Solucao::ClienteRota *vetClienteRotaSecundBest,
                                             const bool percorreVeiculos, bool pertubacao, double *vetLimiteTempo)
{

    string mvStr = "mvInterRotasShift";

    int veiculoEscolhido1;      //guarda o veiculo que irá receber um cliente
    int veiculoEscolhido2;      //Veiculo que retira um cliente

    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
    {
        veiculoEscolhido1 = rand_u32() % (solucao->vetorVeiculos.size() - 1);

        if(!pertubacao)
            veiculoEscolhido2 = (solucao->vetorVeiculos.size() - 1); //Força o veiculo2 ser o veiculo em que um cliente é retirado

        else
            veiculoEscolhido2 = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    } else
    {
        veiculoEscolhido1 = rand_u32() % solucao->vetorVeiculos.size();
        veiculoEscolhido2 = rand_u32() % solucao->vetorVeiculos.size();
    }



    auto *veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
    auto *veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];

/*    while(true)
    {
        if(veiculo1->listaClientes.size() > 2)
            break;
        else
        {
            veiculoEscolhido1++;

            if (solucao->veiculoFicticil)
                veiculoEscolhido1 %= (solucao->vetorVeiculos.size() - 1);
            else
                veiculoEscolhido1 %= solucao->vetorVeiculos.size();

            veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
        }
    }*/

    while (true)
    {
        if ((veiculoEscolhido1 != veiculoEscolhido2) && (veiculo2->listaClientes.size() > 2))
            break;
        else
        {
            while ((veiculo2->listaClientes.size() <= 2) || (veiculoEscolhido1 == veiculoEscolhido2))
            {


                veiculoEscolhido2 += 1;
                if (solucao->veiculoFicticil)
                    veiculoEscolhido2 %= (solucao->vetorVeiculos.size() - 1);
                else
                    veiculoEscolhido2 %= solucao->vetorVeiculos.size();

                veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];
            }
        }

    }



    Solucao::ClienteRota *clientePtr;
    Solucao::ClienteRota *nextClienteRota;
    u_int32_t posicaoClienteEscolhido, tam;

    double combustivelRotaParcialVeic1 = 0.0, poluicaoRotaParcialVeic1 = 0.0;
    double combustivelAuxVeic1, poluicaoAuxVeic1, combustivelBestVeic1, poluicaoBestVeic1 = HUGE_VALF;

    int pesoVeic1, pesoVeic2, pesoAux, posicao, posicaoBestVeic1;

    auto clienteEscolhido = std::next(veiculo2->listaClientes.begin(), 0);
    auto escolhido = *clienteEscolhido;
    bool inicioRota;
    const int vaiculo1Original = veiculoEscolhido1;

    do
    {


        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];

        if ((veiculo1->listaClientes.size() <= 2) || (veiculoEscolhido1 == veiculoEscolhido2))
        {
            veiculoEscolhido1++;

            if (!solucao->veiculoFicticil)
                veiculoEscolhido1 = (veiculoEscolhido1 % solucao->vetorVeiculos.size());
            else
                veiculoEscolhido1 = (veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1));

            continue;
        }

        //Escolhe um cliente do veiculo2
        posicaoClienteEscolhido = rand_u32();
        tam = (veiculo2->listaClientes.size() - 2);

        posicaoClienteEscolhido = posicaoClienteEscolhido % tam;

        posicaoClienteEscolhido += 1;

        clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
        escolhido = *clienteEscolhido;
        inicioRota = true;

        const int PosicaoClienteOriginal = posicaoClienteEscolhido;

        bool inviavel = false;

        if ((veiculo1->carga + instancia->vetorClientes[escolhido->cliente].demanda) > instancia->vetorVeiculos[veiculo1->tipo].capacidade)
        {
            posicaoClienteEscolhido += 1;
            posicaoClienteEscolhido = 1 + posicaoClienteEscolhido % (veiculo2->listaClientes.size() - 2);
            clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
            escolhido = *clienteEscolhido;



            //Escolhe um cliente enquanto o peso for maior do que a capacidade
            while ((veiculo1->carga + instancia->vetorClientes[escolhido->cliente].demanda) > instancia->vetorVeiculos[veiculo1->tipo].capacidade)
            {

                posicaoClienteEscolhido += 1;
                posicaoClienteEscolhido = 1 + posicaoClienteEscolhido % (veiculo2->listaClientes.size() - 2);
                clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
                escolhido = *clienteEscolhido;

                if (posicaoClienteEscolhido == PosicaoClienteOriginal)
                {

                    //Inviável, passa para o próximo veículo
                    veiculoEscolhido1++;
                    if (!solucao->veiculoFicticil)
                        veiculoEscolhido1 = (veiculoEscolhido1 % solucao->vetorVeiculos.size());
                    else
                        veiculoEscolhido1 = (veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1));

                    inviavel = true;
                    break;
                }
            }
        }

        if(inviavel)
            continue;




        //Variaveis de combustivel, poluicao
        combustivelRotaParcialVeic1 = poluicaoRotaParcialVeic1 = 0.0;


        //Ajusta o peso dos veiculo
        pesoVeic1 = veiculo1->carga + instancia->vetorClientes[escolhido->cliente].demanda;

        if(veiculo2->tipo != 2)
            pesoVeic2 = veiculo2->carga - instancia->vetorClientes[escolhido->cliente].demanda;
        else
            pesoVeic2 = 0;

        const int PesoVeiculo1 = pesoVeic1;
        const int PesoVeiculo2 = pesoVeic2;

        vetClienteRotaBest[0] = **veiculo1->listaClientes.begin();
        posicao = 0;



        //string strVeiculo1 = veiculo1->printRota();
        //string strVeiculo2 = veiculo2->printRota();



        for (auto clienteIt = std::next(veiculo1->listaClientes.begin(), 1); clienteIt != veiculo1->listaClientes.end();)
        {

            clientePtr = *clienteIt;

            //Verifica se chegou ao deposito
            if (!inicioRota && clientePtr->cliente == 0)
                break;


            if (inicioRota)
                inicioRota = false;

            //Atualiza o próximo cliente
            auto nextIt = clienteIt;
            nextIt++;
            nextClienteRota = *nextIt;

            const Solucao::ClienteRota clienteRotaPosicao = vetClienteRotaBest[posicao];

            vetClienteRotaBest[posicao + 1].cliente = (*clienteEscolhido)->cliente;




                auto cliente = vetClienteRotaBest[posicao + 1];

                    auto prox = clienteIt;

                    int posicaoAux = posicao + 2;

                    for(auto it = prox; it != veiculo1->listaClientes.end(); ++it)
                    {
                        vetClienteRotaBest[posicaoAux].cliente = (*it)->cliente;

                        ++posicaoAux;
                    }

                    bool resultado = true;

                    if(posicaoAux > 2)
                        resultado = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicaoAux,
                                                                  PesoVeiculo1, veiculo1->tipo, &combustivelAuxVeic1,
                                                                  &poluicaoAuxVeic1, NULL, NULL, vetLimiteTempo, vetClienteRotaAux);
                    else
                    {
                        poluicaoAuxVeic1 = combustivelAuxVeic1 = 0.0;
                        posicaoAux = 2;
                    }



                    posicaoBestVeic1 = posicaoAux - 1;

                    //Verifica se é melhor do que a melhor solucao
                    if (resultado)
                    {
                        //Copia solucao para best


                        poluicaoBestVeic1 = poluicaoAuxVeic1;
                        combustivelBestVeic1 = combustivelAuxVeic1;


                        const int posicaoOriginal = posicao;
                        posicao = 0;
                        bool resultadoVeic2 = false;
                        double poluicaoRotaVeic2 = 0.0, combustivelRotaVeic2 = 0.0;
                        vetClienteRotaSecundBest[0] = *(*veiculo2->listaClientes.begin());




                        int posicaoAux;

                        //Recalcular veiculo2 sem escolhido. Até escolhido, recalcular poluicao/combustivel relativo a carga, depois recalcular rota.
                        for (auto clienteIt = std::next(veiculo2->listaClientes.begin(), 1); clienteIt != veiculo2->listaClientes.end();)
                        {
                            clientePtr = *clienteIt;

                            if (clientePtr->cliente == (*clienteEscolhido)->cliente)
                            {

                                clienteIt++;

                                posicaoAux = posicao + 1;

                                for(auto it = clienteIt; it != veiculo2->listaClientes.end(); ++it)
                                {
                                    vetClienteRotaSecundBest[posicaoAux].cliente = (*it)->cliente;

                                    ++posicaoAux;
                                }

                                if(veiculo2->tipo != 2)
                                {
                                    if(posicaoAux > 2)
                                        resultadoVeic2 = Movimentos_Paradas::criaRota(instancia, vetClienteRotaSecundBest, posicaoAux, PesoVeiculo2, veiculo2->tipo, &combustivelRotaVeic2,
                                                                                      &poluicaoRotaVeic2,NULL, NULL, vetLimiteTempo, vetClienteRotaAux);

                                    else
                                    {
                                        resultadoVeic2 = true;
                                        combustivelRotaVeic2 = poluicaoRotaVeic2 = 0.0;


                                        posicaoAux = 2;
                                    }
                                }
                                else
                                {
                                    resultadoVeic2 = true;
                                    combustivelRotaVeic2 = 0.0;
                                    poluicaoRotaVeic2 = instancia->poluicaoCientesNaoAtendidos(veiculo2->listaClientes.size() - 3);
                                    //posicaoAux = 2;
                                }

                                break;

                            }

                            vetClienteRotaSecundBest[posicao + 1].cliente = (*clientePtr).cliente;

                            posicao += 1;
                            clienteIt++;

                        }

                        bool resultado = false;

                        if(veiculo2->tipo == 2)
                            resultado = ((resultadoVeic2 && ((poluicaoRotaVeic2 + poluicaoBestVeic1) < (veiculo1->poluicao + veiculo2->poluicao + solucao->poluicaoPenalidades))) || resultadoVeic2 && pertubacao);
                        else
                            resultado = ((resultadoVeic2 && ((poluicaoRotaVeic2 + poluicaoBestVeic1) < (veiculo1->poluicao + veiculo2->poluicao))) || resultadoVeic2 && pertubacao);


                        if (resultado)
                        {
                            ResultadosRota resultados;

                            resultados.viavel = true;

                            resultados.combustivel = combustivelBestVeic1;
                            resultados.poluicao = poluicaoBestVeic1;
                            resultados.veiculo = veiculo1;
                            resultados.posicaoVet = posicaoBestVeic1;
                            resultados.peso = PesoVeiculo1;

                            resultados.poluicaoSecundario = poluicaoRotaVeic2;
                            resultados.combustivelSecundario = combustivelRotaVeic2;
                            resultados.veiculoSecundario = veiculo2;
                            resultados.pesoSecundario = PesoVeiculo2;
                            resultados.posicaoVetSecundario = posicaoAux - 1;

                            return resultados;
                        }

                        posicao = posicaoOriginal;

                    }







            //Adicionar arco vetCliente[posicao] - clienteIt(posicao+1)
            vetClienteRotaBest[posicao + 1].cliente = (*clienteIt)->cliente;


            posicao++;
            clienteIt++;

        }

        if(!percorreVeiculos)
            break;


        veiculoEscolhido1++;

        if (!solucao->veiculoFicticil)
            veiculoEscolhido1 = (veiculoEscolhido1 % solucao->vetorVeiculos.size());
        else
            veiculoEscolhido1 = (veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1));

    } while (veiculoEscolhido1 != vaiculo1Original);

    return {false};

}

/*
 * Sendo modificado para funcionar com solucoes inviaveis como busca
 */
ResultadosRota Movimentos::mvInterRotasSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                            Solucao::ClienteRota *vetClienteRotaBest,
                                            Solucao::ClienteRota *vetClienteRotaAux,
                                            Solucao::ClienteRota *vetClienteRotaSecundBest,
                                            Solucao::ClienteRota *vetClienteRotaSecundAux, const bool percorreVeiculos,
                                            bool pertubacao, double *vetLimiteTempo)
{

    int veiculoEscolhido1;
    int veiculoEscolhido2;

    string mvStr = "mvInterRotasSwap";

    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
    {
        veiculoEscolhido1 = rand_u32() % (solucao->vetorVeiculos.size() - 1);
        veiculoEscolhido2 = solucao->vetorVeiculos.size() - 1;               //Força o veiculo2 ser o veiculo ficticil
    } else
    {
        veiculoEscolhido1 = rand_u32() % solucao->vetorVeiculos.size();
        veiculoEscolhido2 = rand_u32() % solucao->vetorVeiculos.size();
    }

    auto *veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
    auto *veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];

    while(veiculo1->listaClientes.size() <= 2)
    {
        ++veiculoEscolhido1;

        if (solucao->veiculoFicticil)
            veiculoEscolhido1 = veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1);
        else
            veiculoEscolhido1 = veiculoEscolhido1 % solucao->vetorVeiculos.size();

        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];

    }

    while (true)
    {
        if ((veiculoEscolhido1 != veiculoEscolhido2) && (veiculo2->listaClientes.size() > 2))
            break;
        else
        {
            while ((veiculo2->listaClientes.size() <= 2) || (veiculoEscolhido1 == veiculoEscolhido2))
            {

                veiculoEscolhido2 += 1;
                if (solucao->veiculoFicticil)
                    veiculoEscolhido2 %= (solucao->vetorVeiculos.size() - 1);
                else
                    veiculoEscolhido2 %= solucao->vetorVeiculos.size();

                veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];
            }


        }

    }


    int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo2->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
    const auto ConstClienteEscolhidoIt = *clienteEscolhido;

    //Variaveis de combustivel, poluicao
    double combustivelAuxVeic1, poluicaoAuxVeic1, combustivelBestVeic1, poluicaoBestVeic1 = HUGE_VALF; //Rota completa

    int pesoBestVeic1, pesoBestVeic2;

    double combustivelBestVeic2, poluicaoBestVeic2 = 0.0, combustivelAuxVeic2, poluicaoAuxVeic2;
    int posicaoBestVeic1, posicaoBestVeic2;

    //Ajusta o peso dos veiculo
    int pesoAux;


    //Copia veiculo2 para vetClienteRotaSecundAux até cliente escolhido
    int k = 0;
    for (auto it = veiculo2->listaClientes.begin(); it != clienteEscolhido; ++it, ++k)
        vetClienteRotaSecundBest[k].cliente = (*it)->cliente;

    int posicao = 0;
    Solucao::ClienteRota *clientePtr;

    bool inicioRota = true;
    const int demandaEscolhido = instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;

    const int Veiculo1PosicaoOriginal = veiculoEscolhido1;


    do
    {

        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];

        if((veiculoEscolhido1 == veiculoEscolhido2) || veiculo1->listaClientes.size() <= 2)
        {
            veiculoEscolhido1++;

            if(solucao->veiculoFicticil)
                veiculoEscolhido1 %= (solucao->vetorVeiculos.size() - 1);
            else
                veiculoEscolhido1 %= solucao->vetorVeiculos.size();

            continue;
        }

        posicao = 0;
        inicioRota = true;
        vetClienteRotaBest[0].cliente = 0;

        for (auto clienteIt = std::next(veiculo1->listaClientes.begin(), 1); clienteIt != veiculo1->listaClientes.end();)
        {

            clientePtr = *clienteIt;

            //Verifica se chegou ao deposito
            if (!inicioRota && clientePtr->cliente == 0)
                break;

            if (inicioRota)
                inicioRota = false;

            //Verificar pesos

            //Calcula novo peso do veiculo1
            const int PesoVeic1T = (veiculo1->carga - instancia->vetorClientes[clientePtr->cliente].demanda) + demandaEscolhido;

            //Verificar peso
            if (PesoVeic1T < instancia->vetorVeiculos[veiculo1->tipo].capacidade)
            {
                //Calcula novo peso do veiculo2
                const int PesoVeic2T = (veiculo2->carga - demandaEscolhido) + instancia->vetorClientes[clientePtr->cliente].demanda;


                //Verificar peso
                if (PesoVeic2T < instancia->vetorVeiculos[veiculo2->tipo].capacidade)
                {
                    //Para o veiculo1: Recalcular poluicao/combustivel, criar arco estre posicao e escolhido e calcular o fim da rota.
                    //Para o veiculo2: Recalcular poluicao/combustivel das cargas até escolhido. Calcular rota com clienteIt.

                    vetClienteRotaBest[posicao + 1].cliente = (*clienteEscolhido)->cliente;
                    combustivelAuxVeic1 = poluicaoAuxVeic1 = 0.0;

                    int cliente1, cliente2;
                    pesoAux = PesoVeic1T;


                    auto next = clienteIt;
                    next++;

                    bool resultadosRotaVeic1 = false, resultadosRotaVeic2 = false;

                    int posicaoAuxVeic1 = posicao + 2;

                    for(auto it = next; it != veiculo1->listaClientes.end(); ++it)
                    {
                        vetClienteRotaBest[posicaoAuxVeic1].cliente = (*it)->cliente;

                        ++posicaoAuxVeic1;
                    }

                    resultadosRotaVeic1 = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicaoAuxVeic1,
                                                                       PesoVeic1T, veiculo1->tipo, &combustivelAuxVeic1,
                                                                       &poluicaoAuxVeic1, NULL, NULL, vetLimiteTempo, vetClienteRotaAux);

                        if (resultadosRotaVeic1)
                        {
                            //Adicionar clienteIt ao veiculo2

                            pesoAux = PesoVeic2T;
                            int posicaoVeic2 = posicaoClienteEscolhido - 1;

                            //Recalcular combustivel/carga do veiculo2 com a nova carga

                            combustivelAuxVeic2 = poluicaoAuxVeic2 = 0.0;


                            //Adicioinar clienteIt ao veiculo2

                            vetClienteRotaSecundBest[posicaoVeic2 + 1].cliente = clientePtr->cliente;

                            auto nexClienteEscolhido = clienteEscolhido;
                            nexClienteEscolhido++;

                            int posicaoAuxVeic2 = posicaoVeic2 + 2;

                            for(auto it = nexClienteEscolhido; it != veiculo2->listaClientes.end(); ++it)
                            {

                                vetClienteRotaSecundBest[posicaoAuxVeic2].cliente = (*it)->cliente;

                                ++posicaoAuxVeic2;

                            }

                            if(veiculo2->tipo != 2)
                                resultadosRotaVeic2 = Movimentos_Paradas::criaRota(instancia, vetClienteRotaSecundBest, posicaoAuxVeic2, PesoVeic2T, veiculo2->tipo, &combustivelAuxVeic2,
                                                                               &poluicaoAuxVeic2, NULL, NULL, vetLimiteTempo, vetClienteRotaSecundAux);

                            else
                            {
                                resultadosRotaVeic2 = true;
                                poluicaoAuxVeic2 = solucao->poluicaoPenalidades;
                                combustivelAuxVeic2 = 0.0;

                            }


                            if (resultadosRotaVeic2)
                            {   //cout<<"Gerou solucao\n";
                                //Verificar se a solucao atual é melhor que a melhor solucao

                                double poluicaoNovaSolucao = poluicaoAuxVeic1 + poluicaoAuxVeic2;
                                bool viavel = false;

                                if(veiculo2->tipo != 2)
                                    viavel = (poluicaoNovaSolucao < (veiculo1->poluicao + veiculo2->poluicao)) || pertubacao;
                                else
                                    (poluicaoNovaSolucao < (veiculo1->poluicao + veiculo2->poluicao + solucao->poluicaoPenalidades)) || pertubacao;

                                if((poluicaoNovaSolucao < (veiculo1->poluicao + veiculo2->poluicao)) || pertubacao)
                                {
                                    //Atualizar nova solucao


                                    //Copia valores para resultado
                                    ResultadosRota resultados;

                                    resultados.viavel = true;
                                    resultados.combustivel = combustivelAuxVeic1;
                                    resultados.poluicao = poluicaoAuxVeic1;
                                    resultados.veiculo = veiculo1;
                                    resultados.posicaoVet = posicaoAuxVeic1 -1 ;
                                    resultados.peso = PesoVeic1T;
                                    resultados.poluicaoSecundario = poluicaoAuxVeic2;
                                    resultados.combustivelSecundario = combustivelAuxVeic2;
                                    resultados.veiculoSecundario = veiculo2;
                                    resultados.pesoSecundario = PesoVeic2T;
                                    resultados.posicaoVetSecundario = posicaoAuxVeic2 - 1;

                                    return resultados;



                                }

                            }
                        }




                }
            }

            //Escrever clienteIt em vetClienteAux

            vetClienteRotaBest[posicao + 1].cliente = clientePtr->cliente;
            posicao++;
            clienteIt++;

        }

        if(!percorreVeiculos)
            break;

        veiculoEscolhido1++;

        if(solucao->veiculoFicticil)
            veiculoEscolhido1 %= (solucao->vetorVeiculos.size() - 1);
        else
            veiculoEscolhido1 %= solucao->vetorVeiculos.size();

    }while(veiculoEscolhido1 != Veiculo1PosicaoOriginal);


    return {.viavel = false};


}

bool Movimentos::recalculaCombustivelPoluicaoCargas(Solucao::Veiculo *veiculo, double *poluicao, double *combustivel,
                                                    int *pesoTotal, const Instancia::Instancia *const instancia,
                                                    Solucao::ClienteRota *vetClienteRotaAux, int posicao)
{
    int cliente1, cliente2;

    //Recalcular poluicao/combustivel das cargas
    for (int i = 1; i <= posicao; ++i)
    {
        cliente1 = vetClienteRotaAux[i - 1].cliente;
        cliente2 = vetClienteRotaAux[i].cliente;

        double dist = instancia->matrizDistancias[cliente1][cliente2];

        vetClienteRotaAux[i].poluicao = vetClienteRotaAux[i].poluicaoRota +
                                        VerificaSolucao::poluicaoCarga(instancia, veiculo->tipo, *pesoTotal, dist);
        vetClienteRotaAux[i].combustivel = vetClienteRotaAux[i].combustivelRota +
                                           VerificaSolucao::combustivelCarga(instancia, veiculo->tipo, *pesoTotal,
                                                                             dist);

        *combustivel += vetClienteRotaAux[i].combustivel;
        *poluicao += vetClienteRotaAux[i].poluicao;

        *pesoTotal -= instancia->vetorClientes[cliente2].demanda;
    }

    if (VerificaSolucao::verificaCombustivel(*combustivel, veiculo, instancia))
        return true;
    else
        return false;


}

void Movimentos::copiaSolucao(Solucao::ClienteRota *bestPtr, Solucao::ClienteRota *auxPtr, double *poluicaoBest,
                              double *combustivelBest, ResultadosRota resultado, int *posicao)
{

    //Copia rota
    for (int i = 0; i <= resultado.posicaoVet; ++i)
    {
        *bestPtr = *auxPtr;

        bestPtr++;
        auxPtr++;
    }
    //cout<<'\n'<<'\n';
    *poluicaoBest = resultado.poluicao;
    *combustivelBest = resultado.combustivel;

    if (posicao)
        *posicao = resultado.posicaoVet;

}

ResultadosRota Movimentos::recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                         int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                         Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin)
{

    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto clienteAlvo = clienteEscolhido;

    if (posicaoAlvo > 1)
        clienteAlvo = std::next(veiculo->listaClientes.begin(), posicaoAlvo);

    double poluicao, combustivel;
    poluicao = combustivel = 0.0;


    //Copiar lista para vetClienteRota, até posicaoAlvo. Se clienteEscolhido for achado, deve-se calcular a rota sem ele.
    for (auto it = std::next(veiculo->listaClientes.begin(), begin); it != veiculo->listaClientes.end();)
    {


        if ((*it)->cliente != (*clienteEscolhido)->cliente &&
            (posicaoAlvo < 0 || (*it)->cliente != (*clienteAlvo)->cliente))
        {
            //Copia it para o vetor e atualiza as variaveis.
            vetClienteRotaAux[posicaoVet] = **it;

            //Recalcular poluicao e combustivel das rotas.

            auto proximo = it;
            proximo++;

            vetClienteRotaAux[posicaoVet].poluicao = vetClienteRotaAux[posicaoVet].poluicaoRota +
                                                     VerificaSolucao::poluicaoCarga(instancia, veiculo->tipo, peso,
                                                                                    instancia->matrizDistancias[(**it).cliente][(**proximo).cliente]);
            vetClienteRotaAux[posicaoVet].combustivel = vetClienteRotaAux[posicaoVet].combustivelRota +
                                                        VerificaSolucao::combustivelCarga(instancia, veiculo->tipo,
                                                                                          peso,
                                                                                          instancia->matrizDistancias[(**it).cliente][(**proximo).cliente]);

            //Atualiza variaveis
            peso -= instancia->vetorClientes[(**it).cliente].demanda;
            poluicao += vetClienteRotaAux[posicaoVet].poluicao;
            combustivel += vetClienteRotaAux[posicaoVet].combustivel;


        } else if ((posicaoAlvo > 0) && (*it)->cliente == (*clienteAlvo)->cliente)
        {
            //Somente insere it no vetor.
            vetClienteRotaAux[posicaoVet] = **it;
            peso -= instancia->vetorClientes[(**it).cliente].demanda;

            break;
        } else
        {
            //it é igual a  clienteEscolhido

            auto itAntes = it;
            itAntes--;
            it++;
            peso -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;

            vetClienteRotaAux[posicaoVet + 1] = **it;

            //Rota entre o cliente anterior de it e o cliente posterior de it
            if (!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet + 1],
                                               instancia, peso, veiculo->tipo, NULL, nullptr))
            {
                ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                return resultado;
            }

            //Atualiza valores
            poluicao += vetClienteRotaAux[posicaoVet + 1].poluicao;
            combustivel += vetClienteRotaAux[posicaoVet + 1].combustivel;
            peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet + 1].cliente].demanda;

            posicaoVet += 1;

            // Cliente alvo está depois de clienteEscolhido. Recalcular rotas até cliente alvo.
            while (true)
            {
                it++;

                vetClienteRotaAux[posicaoVet + 1] = **it;
                if (!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet + 1],
                                                   instancia, peso, veiculo->tipo, NULL, nullptr))
                {
                    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                //Atualiza valores
                poluicao += vetClienteRotaAux[posicaoVet + 1].poluicao;
                combustivel += vetClienteRotaAux[posicaoVet + 1].combustivel;
                peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet].cliente].demanda;
                posicaoVet += 1;

                //Verifical combustivel
                if ((combustivel - instancia->vetorVeiculos[veiculo->tipo].combustivel) >= -0.001)
                {
                    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                //Verifica se encontrou cliente alvo ou o deposito
                if ((*it)->cliente == (*clienteAlvo)->cliente || (*it)->cliente == 0)
                {
                    posicaoVet += 1;
                    break;
                }


            }

        }

        posicaoVet += 1;
        ++it;
    }

    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};
    return resultado;

}

// 0-1-2-3-4-5-0
//0-1-3-2- => 0-1-3-2-4-5-0

//Calcula rota ate o final.
//Esta escrevendo em posicaoVet !!!
//Substitui escolhido por substituto
ResultadosRota Movimentos::calculaFimRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, ItClienteRota proximoClienteIt, int peso,
                                          Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, double poluicao, double combustivel, const int clienteEscolhido,
                                          std::string mvStr, int *erro, const int substituto, const int pesoTotal, const int maisclientes)
{


    //Calcular rota do ultimo cliente em vetCliente
    if (clienteEscolhido == 0)
        throw exceptionEscolhido;
    for (auto itCliente = proximoClienteIt; itCliente != veiculo->listaClientes.end();)
    {
        auto antesPtrCliente = &vetClienteRotaAux[posicaoVet];
        auto ptrCliente = &vetClienteRotaAux[posicaoVet + 1];
        if ((**itCliente).cliente == clienteEscolhido)
        {
            if (substituto != -1)
            {
                vetClienteRotaAux[posicaoVet + 1].cliente = substituto;
            } else
            {
                itCliente++;
                vetClienteRotaAux[posicaoVet + 1] = **itCliente;
            }
        } else
        {
            vetClienteRotaAux[posicaoVet + 1] = **itCliente;
        }

        //Calcular rota entre posicaoVet e posicaoVet + 1
        if (!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet + 1],
                                           instancia, peso, veiculo->tipo, NULL, nullptr))
        {
            if(erro)
                *erro = 1;
            ResultadosRota resultados = {.viavel=false};
            //cout<<"mv recalculaRota. J.\n";
            return resultados;
        }

        //Atualiza combustivel e poluicao.
        peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet + 1].cliente].demanda;
        combustivel += vetClienteRotaAux[posicaoVet + 1].combustivel;
        poluicao += vetClienteRotaAux[posicaoVet + 1].poluicao;

        //Verifica peso
        if (peso < 0)
        {
            throw exceptionPeso;
        }

        //Verifica combustivel
        if (!VerificaSolucao::verificaCombustivel(combustivel, veiculo, instancia))
        {
            if(erro)
                *erro = 0;

            return {.viavel = false, .posicaoVet = posicaoVet};
        }
        posicaoVet += 1;
        ++itCliente;
    }

    if (peso > 0)
    {
        cout << "Erro peso nao eh 0\n";
    }

    if(pesoTotal != -1)
    {

        auto veiculoPtr = new Solucao::Veiculo(veiculo->tipo);

        for (auto it : veiculoPtr->listaClientes)
            delete it;


        veiculoPtr->listaClientes.erase(veiculoPtr->listaClientes.begin(), veiculoPtr->listaClientes.end());

        for (int i = 0; i <= posicaoVet; ++i)
        {
            auto cliente = new Solucao::ClienteRota;
            *cliente = vetClienteRotaAux[i];

            veiculoPtr->listaClientes.push_back(cliente);

        }

        auto clientePtr = *std::next(veiculoPtr->listaClientes.begin(), 1);


        veiculoPtr->poluicao = poluicao;
        veiculoPtr->combustivel = combustivel;
        veiculoPtr->carga = pesoTotal;

        if (!VerificaSolucao::verificaVeiculo(veiculoPtr, instancia))
        {
            cout<<"\n\n";
            for(auto it:veiculoPtr->listaClientes)
                cout<<it->cliente<<" ";

            cout<<'\n';

            delete veiculoPtr;
            cout << "*Func pai: " << mvStr << '\n';
            throw exceptionVeiculo;
        }

        delete veiculoPtr;


    }

    //Peso não se aplica.
    return {.poluicao = poluicao, .combustivel = combustivel, .peso = -1, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};


}


//**************************************************************************************************************************************************
//**************************************************************************************************************************************************


Movimentos::ResultadosRota
Movimentos::inverteRota(ItClienteRota itInicio, ItClienteRota itFim, Solucao::ClienteRota *vetClienteRotaAux, int posicao, int peso,
                        double poluicao,
                        double combustivel, const Instancia::Instancia *const instancia, int tipoVeiculo)
{
    //Percorre o veiculo na ordem inversa
    for (;; itFim--)
    {

        vetClienteRotaAux[posicao + 1].cliente = (*itFim)->cliente;

        //Calcula horário, poluicao e combustivel
        if (!Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao + 1], instancia,
                                           peso, tipoVeiculo, NULL, nullptr))
            return {.viavel = false};

        //Atualia poluicao e combustivel
        poluicao += vetClienteRotaAux[posicao + 1].poluicao;
        combustivel += vetClienteRotaAux[posicao + 1].combustivel;

        //Verifica capacidade
        if (!VerificaSolucao::verificaCombustivel(combustivel, tipoVeiculo, instancia))
            return {.viavel = false};

        //Atualia peso
        peso -= instancia->vetorClientes[(*itFim)->cliente].demanda;

        posicao++;
        if (itFim == itInicio)
            break;
    }

    return {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicao};

}

/*
 * Escolher uma rota e uma aresta. Percorrer as outras aresta e invertendo os clientes entre as duas arestas. As arestas não podem ser do mesmo cliente.
 * OK
 */
ResultadosRota Movimentos::mv_2optSwapIntraRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                                Solucao::ClienteRota *vetClienteRotaBest,
                                                Solucao::ClienteRota *vetClienteRotaAux, const bool percorreVeiculos,
                                                bool pertubacao, double *vetLimiteTempo)
{

    int veiculoEscolhido;      //guarda o veiculo
    string mvStr = "mv_2opt_intraRota";
    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);


    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    //Seleciona outro veiculo, enquanto o veiculo for vazio


    //Escolhe um cliente
    int posicaoClienteEscolhido = 1;
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;
    bool inicioRota = true;

    //cout<<"Escolhido "<<escolhido->cliente<<'\n';

    double combustivelAux, poluicaoAux;

    int pesoAux;

    int posicao = 0;
    int indexListaClientes = 1;
    bool clientePassouEscolhido = false;
    auto next = veiculo->listaClientes.begin();

    const int veiculoOriginal = veiculoEscolhido;

    do
    {

        veiculo = solucao->vetorVeiculos[veiculoEscolhido];

        if (veiculo->listaClientes.size() <= 2)
        {
            veiculoEscolhido++;

            if (solucao->veiculoFicticil)
                veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

            else
                veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

            continue;

        }

        posicao = 0;
        clientePassouEscolhido = false;
        indexListaClientes = 1;
        const int PesoVeiculo = veiculo->carga;
        vetClienteRotaBest[0].cliente = (*veiculo->listaClientes.begin())->cliente;

        posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);
        clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
        escolhido = *clienteEscolhido;
        inicioRota = true;

        for (auto clienteIt = std::next(veiculo->listaClientes.begin(), 1); clienteIt != veiculo->listaClientes.end();)
        {

            next = clienteIt;

            if ((*next)->cliente == 0)
                break;

            next++;

            if ((*next)->cliente == 0)
                break;

            //Cliente escolhido é a clienteIt.  duas arestas do mesmo cliente
            if (indexListaClientes == posicaoClienteEscolhido)
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

/*            auto prox = clienteEscolhido;
            prox++;*/



            if (clientePassouEscolhido)
            {
                fim = clienteIt;
                inicio = clienteEscolhido;
            }

            auto prox = fim;
            prox++;

            int posicaoAux = posicao + 1;

            //Inverte trecho da rota

            for(;;fim--)
            {
                vetClienteRotaBest[posicaoAux].cliente = (*fim)->cliente;

                ++posicaoAux;

                if(fim == inicio)
                    break;
            }

            //Escreve o resto da rota



            for(auto it = prox; it != veiculo->listaClientes.end(); ++it)
            {
                vetClienteRotaBest[posicaoAux].cliente = (*it)->cliente;

                ++posicaoAux;
            }

/*            cout<<"clientePassouEscolhido "<<clientePassouEscolhido;
            cout<<"\nprox: "<<(*prox)->cliente;
            cout<<"\nRota: ";

            for(int i = 0; i < posicaoAux; ++i)
                cout<<vetClienteRotaBest[i].cliente<<" ";

            cout<<"\n\n***************************************************************\n\n";*/

            bool resultadosRota = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicaoAux, PesoVeiculo,
                                                               veiculo->tipo, &combustivelAux, &poluicaoAux, NULL,
                                                               NULL, vetLimiteTempo, vetClienteRotaAux);

            if (resultadosRota)
            {

                    if ((poluicaoAux < veiculo->poluicao) || pertubacao)
                    {

                        Movimentos::ResultadosRota resultadosRota;

                        resultadosRota.viavel = true;
                        resultadosRota.poluicao = poluicaoAux;
                        resultadosRota.posicaoVet = posicaoAux - 1;
                        resultadosRota.combustivel = combustivelAux;
                        resultadosRota.peso = veiculo->carga;
                        resultadosRota.veiculo = veiculo;
                        resultadosRota.veiculoSecundario = NULL;

                        return resultadosRota;
                    }


            }

            if (!clientePassouEscolhido)
            {
                vetClienteRotaBest[posicao + 1].cliente = (*clienteIt)->cliente;
                posicao++;

            }
            //Se clienteIt passou escolhido, ele NÃO é copiado para o vetor. Porque a cada interação de vet[posicao] com (vet[posicao+1], igual a clienteIt), aresta irá mudar!!

            clienteIt++;
            indexListaClientes++;

        }

        if(!percorreVeiculos)
            break;

        veiculoEscolhido++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

    }while(veiculoEscolhido != veiculoOriginal);


    return {.viavel = false};

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
ResultadosRota
Movimentos::mv_2optSwapInterRotas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                  Solucao::ClienteRota *vetClienteRotaBest,
                                  Solucao::ClienteRota *vetClienteRotaAux,
                                  Solucao::ClienteRota *vetClienteRotaSecundBest,
                                  Solucao::ClienteRota *vetClienteRotaSecundAux,
                                  const bool percorreVeiculos, bool pertubacao, double *vetLimiteTempo)
{


    string mvStr = "2optInterRotas";

    int veiculoEscolhido1;      //guarda o veiculo1
    int veiculoEscolhido2;      //guarda o veiculo2

    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
    {
        veiculoEscolhido1 = rand_u32() % (solucao->vetorVeiculos.size() - 1);
        veiculoEscolhido2 = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    } else
    {
        veiculoEscolhido1 = rand_u32() % solucao->vetorVeiculos.size();
        veiculoEscolhido2 = rand_u32() % solucao->vetorVeiculos.size();
    }

    auto *veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
    auto *veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];


    if(!veiculo1 || !veiculo2)
        cout<<"Erro veiculos. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

    //Seleciona outro veiculo, enquanto o veiculo for vazio


    //Seleciona outro veiculo, enquanto o veiculo for vazio ou igual a veiculo1
    while ((veiculoEscolhido1 == veiculoEscolhido2) || (veiculo2->listaClientes.size() <= 2))
    {
        veiculoEscolhido2++;


        if (solucao->veiculoFicticil)
        {
            veiculoEscolhido2 = veiculoEscolhido2 % (solucao->vetorVeiculos.size() - 1);
        } else
        {
            veiculoEscolhido2 = veiculoEscolhido2 % solucao->vetorVeiculos.size();
        }

        veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];

    }



    if(!veiculo1 || !veiculo2)
        cout<<"Erro veiculos. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

    //Escolhe um cliente
    const int posicaoClienteEscolhido = rand_u32() % (veiculo2->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;

    if(!escolhido)
        cout<<"Erro escolhido\n";

    //Clientes a partir de cliente escolhido


    //Calcula peso dos clientes que serão retirados do veiculo2
    int pesoAux = 0;

    auto itAux = clienteEscolhido;
    itAux++;


    for (; itAux != veiculo2->listaClientes.end(); itAux++)
        pesoAux += instancia->vetorClientes[(*itAux)->cliente].demanda;


    const int pesoClientesVeic2AposEscolhido = pesoAux;
    const int pesoClientesVeic2Parcial = veiculo2->carga - pesoAux;

    double combustivelAux, poluicaoAux;
    double combustivelBest, poluicaoBest = HUGE_VALF;

    double combustivelBestSec, poluicaoBestSec;

    int posicaoBestVeic = 0, pesoBest, pesoBestSec;
    int posicaoBestSec = 0;

    int pesoVeic1Parcial = 0;



    auto it = veiculo1->listaClientes.begin();
    vetClienteRotaBest[0].cliente = (*it)->cliente;
    it++;
    vetClienteRotaBest[1].cliente = (*it)->cliente;



    int posicao = 1;
    int k = 0;


    const int tipoVeic2 = veiculo2->tipo;


    for (auto it : veiculo2->listaClientes)
    {


        vetClienteRotaSecundBest[k] = *it;
        if (it->cliente == (*clienteEscolhido)->cliente)
            break;

        k++;
    }


    auto ultimo = veiculo1->listaClientes.end(); //NULL

    const int veiculo1Original = veiculoEscolhido1;



    do
    {
        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];

        if(!veiculo1 || !veiculo2)
            cout<<"Erro veiculos. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

        if ((veiculo1->listaClientes.size() <= 2) || (veiculoEscolhido1 == veiculoEscolhido2))
        {
            veiculoEscolhido1++;

            if (solucao->veiculoFicticil)
                veiculoEscolhido1 = veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1);

            else
                veiculoEscolhido1 = veiculoEscolhido1 % solucao->vetorVeiculos.size();

            continue;
        }




        it = veiculo1->listaClientes.begin();

        if(!*it)
            cout<<"Erro cliente it. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

        vetClienteRotaBest[0].cliente = (*it)->cliente;
        it++;

        if(!*it)
            cout<<"Erro cliente it. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

        vetClienteRotaBest[1].cliente = (*it)->cliente;



        ultimo = veiculo1->listaClientes.end(); //NULL
        ultimo--; // 0
        ultimo--; //ultimo cliente

        if(!*ultimo)
            cout<<"Erro ultimo. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

        const int tipoVeic1 = veiculo1->tipo;

        posicao = 1;
        k = 0;



        for(auto clienteIt = std::next(veiculo1->listaClientes.begin(), 1); clienteIt != veiculo1->listaClientes.end();)
        {
            if(!*clienteIt)
                cout<<"Erro clienteIt. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

            if ((*clienteIt)->cliente == (*ultimo)->cliente)
                break;


                //Verifica peso Veiculo1

                    double combustivelVei1, poluicaoVeic1;

                    auto itAux = clienteEscolhido;
                    itAux++;
                    int posicaoAuxVeic1 = posicao + 1;

                    int p = 0;


                    //Adiciona clientes do veiculo2
                    for(; itAux != veiculo2->listaClientes.end(); ++itAux)
                    {

                        if(!*itAux)
                            cout<<"Erro itAux. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

                        vetClienteRotaBest[posicaoAuxVeic1].cliente = (*itAux)->cliente;

                        ++posicaoAuxVeic1;
                    }


                    for(int i = 0; i < posicaoAuxVeic1; ++i)
                    {
                        p += instancia->vetorClientes[vetClienteRotaBest[i].cliente].demanda;
                    }

                    const int PesoNovoVeiculo1 = p;

                    if(PesoNovoVeiculo1 < instancia->vetorVeiculos[veiculo1->tipo].capacidade)
                    {

                        itAux = clienteIt;
                        itAux++;

                        //Adiciona clientes do veiculo1

                        int posicaoAuxVeic2 = posicaoClienteEscolhido + 1;

                        for (; itAux != veiculo1->listaClientes.end(); ++itAux)
                        {
                            if(!*itAux)
                                cout<<"Erro itAux. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

                            vetClienteRotaSecundBest[posicaoAuxVeic2].cliente = (*itAux)->cliente;

                            ++posicaoAuxVeic2;
                        }

                        p = 0;

                        for (int i = 0; i < posicaoAuxVeic2; ++i)
                        {
                            p += instancia->vetorClientes[vetClienteRotaSecundBest[i].cliente].demanda;
                        }

                        const int PesoNovoVeiculo2 = p;

                        if(PesoNovoVeiculo2 < instancia->vetorVeiculos[veiculo2->tipo].capacidade)
                        {
                            bool resultadosRotaVeic1 = false;

                            if(posicaoAuxVeic1 > 2)
                                resultadosRotaVeic1 = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest,
                                                                                    posicaoAuxVeic1, PesoNovoVeiculo1,
                                                                                    veiculo1->tipo, &combustivelVei1,
                                                                                    &poluicaoVeic1,
                                                                                    NULL, NULL, vetLimiteTempo, vetClienteRotaAux);
                            else
                            {
                                poluicaoVeic1 = combustivelVei1 = 0.0;
                            }

                            if (resultadosRotaVeic1)
                            {

                                double combustivelVeic2, poluicaoVeic2;



                                //Calcula fim da rota adicionando clientes do veiculo1
                                bool resultadosRotaVeic2;

                                if(posicaoAuxVeic2 > 2)
                                    resultadosRotaVeic2 = Movimentos_Paradas::criaRota(instancia,
                                                                                        vetClienteRotaSecundBest,
                                                                                        posicaoAuxVeic2,
                                                                                        PesoNovoVeiculo2,
                                                                                        veiculo2->tipo,
                                                                                        &combustivelVeic2,
                                                                                        &poluicaoVeic2, NULL, NULL,
                                                                                        vetLimiteTempo, vetClienteRotaSecundAux);
                                else
                                {

                                    combustivelVeic2 = poluicaoVeic2 = 0.0;
                                    posicaoAuxVeic2 = 2;
                                }

                                if (resultadosRotaVeic2)
                                {

                                    if (((poluicaoVeic1 + poluicaoVeic2) < (veiculo1->poluicao + veiculo2->poluicao)) || pertubacao)
                                    {
                                        //Atualiza a melhor solucao


                                        Movimentos::ResultadosRota resultadosRota;

                                        resultadosRota.viavel = true;
                                        resultadosRota.poluicao = poluicaoVeic1;
                                        resultadosRota.posicaoVet = posicaoAuxVeic1 - 1;
                                        resultadosRota.combustivel = combustivelVei1;
                                        resultadosRota.peso = PesoNovoVeiculo1;
                                        resultadosRota.veiculo = veiculo1;
                                        resultadosRota.veiculoSecundario = veiculo2;
                                        resultadosRota.poluicaoSecundario = poluicaoVeic2;
                                        resultadosRota.combustivelSecundario = combustivelVeic2;
                                        resultadosRota.pesoSecundario = PesoNovoVeiculo2;
                                        resultadosRota.posicaoVetSecundario = posicaoAuxVeic2 - 1;


                                        return resultadosRota;

                                    }

                                }


                            }

                        }

                    }






            //Insere o próximo clienteIt no vetClienteRotaAux

            clienteIt++;

            if(!*clienteIt)
                cout<<"Erro clienteIt. Linha "<<__LINE__<<" arquivo: Movimentos.cpp\n";

            vetClienteRotaBest[posicao + 1].cliente = (*clienteIt)->cliente;
            posicao++;


        }

        if(!percorreVeiculos)
            break;

        veiculoEscolhido1++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido1 = veiculoEscolhido1 % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido1 = veiculoEscolhido1 % solucao->vetorVeiculos.size();


    }while(veiculoEscolhido1 != veiculo1Original);


    return {.viavel = false};

}
//ok
Movimentos::ResultadosRota
Movimentos::mvIntraRotaInverteRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                   Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                   bool pertubacao, double *vetLimiteTempo)
{
    string mvStr = "inverteRota";

    //Variaveis combustivel/poluicao
    Solucao::Veiculo *veiculo = NULL;
    double combustivelBest, poluicaoBest;
    int posicaoBest, pesoBest;
    Solucao::Veiculo *bestVeiculo = NULL;
    int numVeic = solucao->vetorVeiculos.size();;

    if (solucao->veiculoFicticil)
        numVeic -= 1;

    //percorre todos os veiculos
    for (int i = 0; i < numVeic; ++i)
    {

        //Seleciona o veiculo
        veiculo = solucao->vetorVeiculos[i];

        //Verifica se o veiculo é vazio
        if (veiculo->listaClientes.size() <= 2)
            continue;

        //Pega o ultimo cliente do veiculo
        auto itFim = veiculo->listaClientes.end(); //NULL
        itFim--; //0
        itFim--; //ultimo cliente

        //Insere o deposito
        vetClienteRotaBest[0] = **veiculo->listaClientes.begin();

        double poluicao, combustivel;

        int posicao = 1;

        for(;;--itFim)
        {
            vetClienteRotaBest[posicao].cliente = (*itFim)->cliente;

            ++posicao;
            if((*itFim)->cliente == 0)
                break;
        }

        bool resultadosRota = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicao, veiculo->carga,
                                                           veiculo->tipo, &combustivel, &poluicao, NULL, NULL, vetLimiteTempo,
                                                           vetClienteRotaAux);

        if (resultadosRota)
        {




            if (poluicao < veiculo->poluicao)
            {
                ResultadosRota resultadosRota;

                resultadosRota.veiculo = veiculo;
                resultadosRota.viavel = true;
                resultadosRota.poluicao = poluicao;
                resultadosRota.combustivel = combustivel;
                resultadosRota.posicaoVet = posicao - 1;
                resultadosRota.veiculoSecundario = NULL;
                resultadosRota.peso = veiculo->carga;

                return resultadosRota;
            }


        }
    }


    return {.viavel = NULL};


}

Movimentos::ResultadosRota
Movimentos::mvTrocarVeiculos(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                             Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                             Solucao::ClienteRota *vetClienteRotaSecundBest,
                             Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao, double *vetLimiteTempo)
{
    string mvStr = "trocaVeiculo";

    int veiculoEscolhido;
    //Vevifica se a solução é inviavel
    if (solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    Solucao::Veiculo *veiculo1 = solucao->vetorVeiculos[veiculoEscolhido];

    while (veiculo1->listaClientes.size() <= 2)
    {
        veiculoEscolhido++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido];
    }

    bool resultadosRota = false;
    int veiculoOriginal = veiculoEscolhido;

    int novoTipo;
    int posicaoVeic1, posicaoVeic2;

    double combustivelVeic1, poluicaoVeic1;
    double combustivelVeic2, poluicaoVeic2;

    //Troca o tipo do veiculo
    while (!resultadosRota)
    {
        veiculo1 = solucao->vetorVeiculos[veiculoEscolhido];

        if (veiculo1->listaClientes.size() > 2)
        {
            novoTipo = 0;

            if (veiculo1->tipo == 0)
                novoTipo = 1;

            if (veiculo1->carga < instancia->vetorVeiculos[novoTipo].capacidade)
            {

                posicaoVeic1 = 0;
                for(auto it : veiculo1->listaClientes)
                {
                    vetClienteRotaBest[posicaoVeic1].cliente = it->cliente;

                    ++posicaoVeic1;
                }



                resultadosRota = Movimentos_Paradas::criaRota(instancia, vetClienteRotaBest, posicaoVeic1,
                                                              veiculo1->carga, novoTipo, &combustivelVeic1,
                                                              &poluicaoVeic1, NULL, NULL, vetLimiteTempo, vetClienteRotaAux);
            }
        }
        veiculoEscolhido++;

        if (solucao->veiculoFicticil)
            veiculoEscolhido = veiculoEscolhido % (solucao->vetorVeiculos.size() - 1);

        else
            veiculoEscolhido = veiculoEscolhido % solucao->vetorVeiculos.size();

        if (veiculoEscolhido == veiculoOriginal)
            break;

    }

    if (!resultadosRota)
        return {.viavel = false};

    int i = novoTipo;  //Pegar veiculos do tipo: novoTipo

    int numVeiculos = solucao->vetorVeiculos.size();

    if (solucao->veiculoFicticil)
        numVeiculos -= 1;

    Solucao::Veiculo *veiculo2;
    int novoTipoVeic1 = 0, novoTipoVeic2 = 1;

    if (veiculo1->tipo == 0)
    {
        novoTipoVeic1 = 1;
        novoTipoVeic2 = 0;
    }


    //Percorre os veiculos do tipo novoTipoVeic1 e tenta passar para novoTipoVeic2
    for (; i < numVeiculos; i += 2)
    {

        veiculo2 = solucao->vetorVeiculos[i];

        //Verifica a capacidade
        if (veiculo2->carga > instancia->vetorVeiculos[novoTipoVeic2].capacidade)
            continue;

        vetClienteRotaSecundBest[0].cliente = 0;

        auto it = veiculo2->listaClientes.begin(); //0
        it++; //primeiro cliente

        bool resultadosRota2;

        if(veiculo2->listaClientes.size() > 2)
        {
            posicaoVeic2 = 1;
            for(; it != veiculo2->listaClientes.end(); ++it)
            {
                vetClienteRotaSecundBest[posicaoVeic2].cliente = (*it)->cliente;

                ++posicaoVeic2;
            }

            resultadosRota2 = Movimentos_Paradas::criaRota(instancia, vetClienteRotaSecundBest, posicaoVeic2,
                                                           veiculo2->carga, novoTipoVeic2, &combustivelVeic2,
                                                           &poluicaoVeic2, NULL, NULL, vetLimiteTempo, vetClienteRotaSecundAux);
        }
        else
        {
            resultadosRota2 = true;
            combustivelVeic2 = 0.0;
            poluicaoVeic2 = 0.0;
            posicaoVeic2 = 2;

            vetClienteRotaSecundBest[0].cliente = 0;
            vetClienteRotaSecundBest[1].cliente = 0;
        }

        if (resultadosRota2)
        {
            if (((poluicaoVeic1 + poluicaoVeic2) < (veiculo1->poluicao + veiculo2->poluicao)) || pertubacao)
            {

                Movimentos::ResultadosRota resultados;

                resultados.viavel = true;
                resultados.veiculo = veiculo2;
                resultados.poluicao = poluicaoVeic1;
                resultados.combustivel = combustivelVeic1;
                resultados.posicaoVet = posicaoVeic1 - 1;
                resultados.peso = veiculo1->carga;

                resultados.veiculoSecundario = veiculo1;
                resultados.pesoSecundario = veiculo2->carga;
                resultados.combustivelSecundario = combustivelVeic2;
                resultados.poluicaoSecundario = poluicaoVeic2;
                resultados.posicaoVetSecundario = posicaoVeic2 - 1;

                return resultados;
            }
        }


    }


    return {.viavel = false};

}

ResultadosRota Movimentos::calculaFimRota_2OptInter(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                     Solucao::Veiculo *veiculo2, ItClienteRota proximoClienteIt, int peso,
                                     Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, double poluicao,
                                     double combustivel, const int clienteEscolhido, std::string mvStr,
                                     const int substituto, const int pesoTotal, const int maisclientes)
{
    string saida;
    const int posicaoOriginal = posicaoVet;

    saida+="ClienteIt "+std::to_string((**proximoClienteIt).cliente)+'\n';
    saida+"tempoSaida posicao: "+std::to_string(vetClienteRotaAux[posicaoVet].tempoSaida)+'\n'+"cliente posicao: "+std::to_string(vetClienteRotaAux[posicaoVet].cliente)+'\n';


    //Calcular rota do ultimo cliente em vetCliente
    if (clienteEscolhido == 0)
        throw exceptionEscolhido;
    for (auto itCliente = proximoClienteIt; itCliente != veiculo2->listaClientes.end();)
    {
        auto antesPtrCliente = &vetClienteRotaAux[posicaoVet];
        auto ptrCliente = &vetClienteRotaAux[posicaoVet + 1];
        if ((**itCliente).cliente == clienteEscolhido)
        {
            if (substituto != -1)
            {
                vetClienteRotaAux[posicaoVet + 1].cliente = substituto;
            } else
            {
                itCliente++;
                vetClienteRotaAux[posicaoVet + 1] = **itCliente;
            }
        } else
        {
            vetClienteRotaAux[posicaoVet + 1] = **itCliente;
        }

        //Calcular rota entre posicaoVet e posicaoVet + 1
        if (!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet + 1],
                                           instancia, peso, veiculo->tipo, NULL, nullptr))
        {
            Movimentos::ResultadosRota resultados = {.viavel=false};
            //cout<<"mv recalculaRota. J.\n";
            return resultados;
        }

        //Atualiza combustivel e poluicao.
        peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet + 1].cliente].demanda;
        combustivel += vetClienteRotaAux[posicaoVet + 1].combustivel;
        poluicao += vetClienteRotaAux[posicaoVet + 1].poluicao;

        //Verifica peso
        if (peso < 0)
        {
            throw exceptionPeso;
        }

        //Verifica combustivel
        if (!VerificaSolucao::verificaCombustivel(combustivel, veiculo, instancia))
        {
            return {.viavel = false};
        }
        posicaoVet += 1;
        ++itCliente;
    }
    if(pesoTotal != -1)
    {

        auto veiculoPtr = new Solucao::Veiculo(veiculo->tipo);
        for (auto it : veiculoPtr->listaClientes)
            delete it;

        veiculoPtr->listaClientes.erase(veiculoPtr->listaClientes.begin(), veiculoPtr->listaClientes.end());

        for (int i = 0; i <= posicaoVet; ++i)
        {
            auto cliente = new Solucao::ClienteRota;
            *cliente = vetClienteRotaAux[i];
            veiculoPtr->listaClientes.push_back(cliente);

        }

        auto clientePtr = *std::next(veiculoPtr->listaClientes.begin(), 1);


        veiculoPtr->poluicao = poluicao;
        veiculoPtr->combustivel = combustivel;
        veiculoPtr->carga = pesoTotal;

        if (!VerificaSolucao::verificaVeiculo(veiculoPtr, instancia))
        {

            //cout<<saida;

            for(auto it:veiculoPtr->listaClientes)
                cout<<it->cliente<<" *( "<<it->tempoChegada<<" "<<it->tempoSaida<<")\n";
            cout<<'\n';

            cout<<"Tam "<<veiculoPtr->listaClientes.size()<<'\n';
            cout<<"Tipo: "<<veiculoPtr->tipo<<'\n';

            cout << "Carga: " << veiculoPtr->carga << "\n*Capacidade: "
                 << instancia->vetorVeiculos[veiculo->tipo].capacidade << '\n';
            delete veiculoPtr;
            cout << "Func pai: " << mvStr << '\n';
            cout<<"Posicao original: "<<posicaoOriginal<<'\n';
            throw exceptionVeiculo;
        }

        delete veiculoPtr;
    }

    //Peso não se aplica.
    return {.poluicao = poluicao, .combustivel = combustivel, .peso = -1, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};
}

Movimentos::ResultadosRota
Movimentos::aplicaMovimento(int movimento, const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                            Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                            bool pertubacao, Solucao::ClienteRota *vetClienteRotaSecundBest,
                            Solucao::ClienteRota *vetClienteRotaSecundAux, double *vetLimiteTempo)
{

    switch (movimento)
    {
        case 0:
            return Movimentos::mvIntraRotaShift(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, true, false,
                                                pertubacao, vetLimiteTempo);
            break;

        case 1:
            return Movimentos::mvIntraRotaSwap(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, true, false,
                                               pertubacao, vetLimiteTempo);
            break;

        case 2:
            return Movimentos::mvInterRotasShift(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux,
                                                 vetClienteRotaSecundBest, true, pertubacao, vetLimiteTempo);
            break;
        case 3:
            return Movimentos::mvInterRotasSwap(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux,
                                                vetClienteRotaSecundBest, vetClienteRotaSecundAux, true, pertubacao,
                                                vetLimiteTempo);
            break;

        case 4:
            return Movimentos::mv_2optSwapIntraRota(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, true,
                                                    pertubacao, vetLimiteTempo);
            break;

        case 5:
            return Movimentos::mv_2optSwapInterRotas(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux,
                                                     vetClienteRotaSecundBest, vetClienteRotaSecundAux, true,
                                                     pertubacao, vetLimiteTempo);
            break;

        case 6:
            return Movimentos::mvIntraRotaInverteRota(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux,
                                                      pertubacao, vetLimiteTempo);
            break;

        case 7:
            return Movimentos::mvTrocarVeiculos(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux,
                                                vetClienteRotaSecundBest, vetClienteRotaSecundAux, pertubacao, vetLimiteTempo);
            break;
    }

    exit(-1);
    return {.viavel=false};

}

void Movimentos::atualizaSolucao(ResultadosRota resultado, Solucao::Solucao *solucao,
                                 Solucao::ClienteRota *vetClienteRotaBest,
                                 Solucao::ClienteRota *vetClienteRotaSecundBest,
                                 const Instancia::Instancia *const instancia, const int movimento)
{


    //Calcula a diferença
    int tam = resultado.veiculo->listaClientes.size();
    int diferenca = (resultado.posicaoVet + 1) - tam;
    bool positivo = true;

    if (diferenca < 0)
        positivo = false;

    //Adiciona ou remove clientes da lista
    while (diferenca != 0)
    {

        if (positivo)
        {
            auto cliente = new Solucao::ClienteRota;
            resultado.veiculo->listaClientes.push_back(cliente);
            diferenca--;
        } else
        {
            auto cliente = *resultado.veiculo->listaClientes.begin();
            delete cliente;
            resultado.veiculo->listaClientes.pop_front();

            diferenca++;
        }

    }

    int l = 0;

    //Copia o resultado
    for (auto clienteIt : resultado.veiculo->listaClientes)
    {
        *clienteIt = vetClienteRotaBest[l];
        l += 1;
    }
    
    if(l != resultado.veiculo->listaClientes.size())
        cout<<"Erro memoria!!!\n";


    //Atualiza combustivel/poluiao/peso

    solucao->poluicao -= resultado.veiculo->poluicao;

    if(resultado.veiculo->listaClientes.size() > 2)
    {
        resultado.veiculo->combustivel = resultado.combustivel;
        resultado.veiculo->poluicao = resultado.poluicao;
        resultado.veiculo->carga = resultado.peso;
        solucao->poluicao += resultado.veiculo->poluicao;
    }
    else if(resultado.veiculo->listaClientes.size() == 2)
    {
        resultado.veiculo->carga = 0;
        resultado.veiculo->poluicao = 0.0;
        resultado.veiculo->combustivel = 0.0;

/*        for(auto it : resultado.veiculo->listaClientes)
        {
            if(it->cliente != 0)
            {
                cout<<"Erro, veiculo com dois clientes diferentes do deposito \nLinha: "<<__LINE__<<" Arquivo: Movimentos.cpp\n";
                cout<<"Movimento: "<<movimento<<'\n';

                for(l = 0; l <= resultado.posicaoVet; ++l)
                {
                    cout<<vetClienteRotaBest[l].cliente<<' ';
                }
                cout<<'\n';

                exit(-1);
            }

        }*/
    }
    else
    {
        cout<<"Erro, veiculo com menos de 2 clientes(deposito)\nLinha: "<<__LINE__<<" Arquivo: Movimentos.cpp\n";
        cout<<"Movimento: "<<movimento<<'\n';
        exit(-1);

    }

    if (resultado.veiculoSecundario)
    {
        //Calcula a diferença
        diferenca = (resultado.posicaoVetSecundario + 1) - resultado.veiculoSecundario->listaClientes.size();

        positivo = true;

        if (diferenca < 0)
            positivo = false;

        //Adiciona ou remove clientes da lista
        while (diferenca != 0)
        {

            if (positivo)
            {
                auto cliente = new Solucao::ClienteRota;
                resultado.veiculoSecundario->listaClientes.push_back(cliente);
                diferenca--;
            } else
            {
                auto cliente = *resultado.veiculoSecundario->listaClientes.begin();
                delete cliente;
                resultado.veiculoSecundario->listaClientes.pop_front();

                diferenca++;
            }

        }


        l = 0;

        //Copia resultado
        for (auto clienteIt : resultado.veiculoSecundario->listaClientes)
        {
            *clienteIt = vetClienteRotaSecundBest[l];
            l += 1;
        }
        
        if(l != resultado.veiculoSecundario->listaClientes.size())
            cout<<"Erro memoria!!!\n";


        if(resultado.veiculoSecundario->tipo != 2)
        {
            solucao->poluicao -= resultado.veiculoSecundario->poluicao;
            if(resultado.veiculoSecundario->listaClientes.size() > 2)
            {
                //Atualiza combustivel/poluiao/peso

                resultado.veiculoSecundario->combustivel = resultado.combustivelSecundario;
                resultado.veiculoSecundario->poluicao = resultado.poluicaoSecundario;
                resultado.veiculoSecundario->carga = resultado.pesoSecundario;
                solucao->poluicao += resultado.veiculoSecundario->poluicao;
            }
            else if(resultado.veiculoSecundario->listaClientes.size() == 2)
            {
                resultado.veiculoSecundario->carga = 0;
                resultado.veiculoSecundario->poluicao = 0.0;
                resultado.veiculoSecundario->combustivel = 0.0;

/*                for(auto it : resultado.veiculoSecundario->listaClientes)
                {
                    if(it->cliente != 0)
                    {
                        cout<<"Erro, veiculo2 com dois clientes diferentes do deposito \nLinha: "<<__LINE__<<" Arquivo: Movimentos.cpp\n";
                        cout<<"Movimento: "<<movimento<<'\n';


                        cout<<"Vetor: ";
                        for(l = 0; l <= resultado.posicaoVetSecundario; ++l)
                        {
                            cout<<vetClienteRotaSecundBest[l].cliente<<' ';
                        }
                        cout<<'\n';

                        cout<<"Lista: ";

                        for(auto it : )

                        exit(-1);
                    }

                }*/


            }
            else
            {
                cout<<"Erro, veiculo com menos de 2 clientes(deposito)\nLinha: "<<__LINE__<<" Arquivo: Movimentos.cpp\n";
                cout<<"Movimento: "<<movimento<<'\n';
                exit(-1);

            }
        }
        else
        {
            if(resultado.veiculoSecundario->listaClientes.size() == 2)
            {
                auto ptr = resultado.veiculoSecundario->listaClientes.begin();

                delete *ptr;
                ptr++;

                delete *ptr;

                resultado.veiculoSecundario->listaClientes.pop_back();
                resultado.veiculoSecundario->listaClientes.pop_back();

                delete resultado.veiculoSecundario;

                solucao->vetorVeiculos.pop_back();
                solucao->poluicaoPenalidades = 0.0;
                solucao->veiculoFicticil = false;
            }
            else
            {
                instancia->atualizaPoluicaoSolucao(solucao);
            }
        }
    }
}
