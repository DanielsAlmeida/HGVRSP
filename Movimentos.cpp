//
// Created by igor on 10/02/2020.
//linha 1370

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
#include "Exception.h"

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
                                            Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                            bool pertubacao)
{
    string mvStr = "intraRotaShift";

    //Escolhe um veiculo

    int veiculoEscolhido;

    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);
    else
        veiculoEscolhido  = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    while(true)
    {
        if(veiculo->listaClientes.size() > 2)
            break;
        else
        {
            veiculoEscolhido += 1;


            if(solucao->veiculoFicticil)
                veiculoEscolhido %= (solucao->vetorVeiculos.size()-1);
            else
                veiculoEscolhido %= solucao->vetorVeiculos.size();


            veiculo = solucao->vetorVeiculos[veiculoEscolhido];
        }

    }


    int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;

    //ClienteEscolhido será reincerido em cada posição da rota

    bool inicio = false;
    int peso = veiculo->carga;
    int pesoAux;



    //Percorrer clientes da rota e acrecentar clienteEscolhido e recalcular rota

    peso = veiculo->carga;
    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0; //Antes cliente escolhido
    double combustivelAux, poluicaoAux, combustivelBest, poluicaoBest = HUGE_VAL; //Rota completa
    int posicaoVetBest;

    int posicao = 0;
    bool inicioRota = true;
    bool passouEscolhido = false;
    Solucao::ClienteRota *nextClienteRota = NULL;

    vetClienteRotaAux[0] = **veiculo->listaClientes.begin();

    Solucao::ClienteRota *clientePtr;



    string seguencia = veiculo->printRota();


    for(auto clienteIt = veiculo->listaClientes.begin(); clienteIt != veiculo->listaClientes.end();)
    {
        clientePtr = *clienteIt;


        //Verifica se chegou ao deposito
        if(!inicioRota && clientePtr->cliente == 0)
            break;

        if(inicioRota)
            inicioRota = false;

        //Atualiza o próximo cliente
        auto nextIt = clienteIt;
        nextIt++;
        nextClienteRota = *nextIt;

        //*********************************ERRO!!!****************************************

        //Verifica se o proximo cliente é igual a clienteEscolhido. ClienteEscolhido tem que ser pulado.
        if(nextClienteRota->cliente == (*clienteEscolhido)->cliente)
        {

            clienteIt++;
            clientePtr = *clienteIt;
            nextIt = clienteIt;
            nextIt++;
            nextClienteRota = *nextIt;

            if(!inicioRota && clientePtr->cliente == 0)
                break;
        }

        //Verifica se cliente é igual a cliente escolhido
        if(clientePtr->cliente == (*clienteEscolhido)->cliente)
        {
            //Add arco para next

            auto clientePosicao = &vetClienteRotaAux[posicao];
            auto clientePosicaoProx = &vetClienteRotaAux[posicao+1];

            vetClienteRotaAux[posicao + 1].cliente = nextClienteRota->cliente;

            if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, veiculo->tipo))
            {
                //cout<<"MV inviavel J.\n";
                break;
            }
            //cout<<vetClienteRotaAux[posicao].cliente<<" ";
            combustivelRotaParcial += vetClienteRotaAux[posicao+1].combustivel;
            poluicaoRotaParcial += vetClienteRotaAux[posicao+1].poluicao;

            //verifica combustivel
            if(instancia->vetorVeiculos[veiculo->tipo].combustivel - combustivelRotaParcial< -0.001)
            {
                //cout<<"MV inviavel Comb.\n";
                break;
            }

            posicao += 1;

            clienteIt++; //Pq eh Igual a escolhido


            passouEscolhido = true;

            //retira clientePtr

            peso -= instancia->vetorClientes[nextClienteRota->cliente].demanda;

            continue;

        }

        const Solucao::ClienteRota clienteRotaPosicao = vetClienteRotaAux[posicao];

        vetClienteRotaAux[posicao + 1].cliente = (*clienteEscolhido)->cliente;



        //Calcular rota entre posicao e posicao + 1
        if(Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, veiculo->tipo))
        {

            combustivelAux = combustivelRotaParcial;
            poluicaoAux = poluicaoRotaParcial;

            auto cliente = vetClienteRotaAux[posicao+1];

            combustivelAux += vetClienteRotaAux[posicao+1].combustivel;
            poluicaoAux += vetClienteRotaAux[posicao+1].poluicao;

            //Verifica combustivel
            if(VerificaSolucao::verificaCombustivel(combustivelAux, veiculo->tipo, instancia))
            {


                //calcula o resto da rota
                pesoAux = peso;
                pesoAux -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;
                auto prox = clienteIt;
                prox++;

                auto resultado = calculaFimRota(instancia, veiculo, prox, pesoAux, vetClienteRotaAux, posicao + 1,
                                                poluicaoAux, combustivelAux, (*clienteEscolhido)->cliente, mvStr, -1,
                                                veiculo->carga, 0);

                if (resultado.viavel)
                {
                    //Verifica se é melhor do que a melhor solucao
                    if (resultado.poluicao < poluicaoBest)
                    {
                        //Copia solucao para best


                        auto bestPtr = &vetClienteRotaBest[0];
                        auto auxPtr = &vetClienteRotaAux[0];

                        //Copia rota
                        for (int i = 0; i < resultado.posicaoVet + 1; ++i)
                        {
                            *bestPtr = *auxPtr;

                            //cout<<bestPtr->cliente<<" ";

                            bestPtr++;
                            auxPtr++;
                        }
                        //cout<<'\n'<<'\n';
                        poluicaoBest = resultado.poluicao;
                        combustivelBest = resultado.combustivel;
                        posicaoVetBest = resultado.posicaoVet;

                        if (pertubacao)
                            break;

                    }
                }

            }
            else
            {
                //cout<<"MV combustivel 2\n";
            }

        }

        /* ****************************************************************************************
         * Adicionar arco vetCliente[posicao] - clienteIt(posicao+1)
         *
         * Se passouEscolhido recalcular combustivel/poluicao relativo ao peso.
         **************************************************************************************** */

        vetClienteRotaAux[posicao] = clienteRotaPosicao; //posicao+1 foi alterada.
        vetClienteRotaAux[posicao+1] = *nextClienteRota;

        if(passouEscolhido)
        {
            //Atualiza vetRotaAux

            if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, veiculo->tipo))
                break;

        }

        //Adiciona combustivel/poluicao ao "veiculo" parcial

        combustivelRotaParcial += vetClienteRotaAux[posicao+1].combustivel;
        poluicaoRotaParcial += vetClienteRotaAux[posicao+1].poluicao;

        peso -= instancia->vetorClientes[nextClienteRota->cliente].demanda;
        posicao += 1;

        clienteIt++;
    }


     if(poluicaoBest == HUGE_VALF)
        return {.viavel = false};
    else
    {

        return {.poluicao = poluicaoBest, .combustivel = combustivelBest, .peso = veiculo->carga, .viavel = true, .posicaoVet = posicaoVetBest, .veiculo = veiculo, .veiculoSecundario = NULL,
                .poluicaoSecundario = -1.0, .combustivelSecundario = -1.0, .pesoSecundario = -1, .posicaoVetSecundario = -1};
    }

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
                               Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{

    //Escolhe um veiculo
    //cout<<"mvIntraRotaSwap\n\n";

    string mvStr = "mvIntraRotaSwap";

    int veiculoEscolhido;

    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);

    else
        veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    while(true)
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

            /* //Verifica se escolheu o veiculo ficticil
            if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
            {

                veiculoEscolhido = 0;
            }*/

            veiculo = solucao->vetorVeiculos[veiculoEscolhido];
        }

    }

    //Escolhe cliente

    int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 2);
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto ptrEscolhido = *clienteEscolhido;

    bool inicio = false;
    int peso = veiculo->carga;
    int pesoAux;


    peso = veiculo->carga;
    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0; //Antes cliente escolhido ou clienteIt
    double combustivelAux, poluicaoAux, combustivelBest, poluicaoBest = HUGE_VALF; //Rota completa
    int posicaoVetBest;

    int posicao = 0;
    bool inicioRota = true;
    bool passouEscolhido = false;
    Solucao::ClienteRota *nextClienteRota = NULL;

    vetClienteRotaAux[0] = **veiculo->listaClientes.begin();

    Solucao::ClienteRota *clientePtr;
    string seguencia = veiculo->printRota();

    //Guarda o proximo cliente depois de escolhido.
    auto proxEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido + 1);
    auto nextIt = veiculo->listaClientes.begin();



    for(auto clienteIt = std::next(veiculo->listaClientes.begin(), 1); clienteIt != veiculo->listaClientes.end();)
    {
        clientePtr = *clienteIt;

        //Verifica se chegou ao deposito
        if(clientePtr->cliente == 0)
            break;


        //Atualiza o próximo cliente
        if(!passouEscolhido)
        {
            nextIt = clienteIt;
            nextIt++;
            nextClienteRota = *nextIt;
        }

        if(clientePtr->cliente == (*clienteEscolhido)->cliente)
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

        if(!passouEscolhido)
        {
            //Criar o arco posicao - escolhido

            vetClienteRotaAux[posicao+1].cliente = (*clienteEscolhido)->cliente;

            if(Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, veiculo->tipo))
            {
                //Atualiza peso, combustivel, poluicao
                pesoAux = peso;
                pesoAux -= instancia->vetorClientes[vetClienteRotaAux[posicao+1].cliente].demanda;

                combustivelAux = combustivelRotaParcial + vetClienteRotaAux[posicao+1].combustivel;
                poluicaoAux = poluicaoRotaParcial + vetClienteRotaAux[posicao+1].poluicao;

                //Verifica combustivel
                if(VerificaSolucao::verificaCombustivel(combustivelAux, veiculo->tipo, instancia))
                {
                    //Calcular rota a partir de next e substituir candidato por clienteIt

                    auto resultado = calculaFimRota(instancia, veiculo, nextIt, pesoAux, vetClienteRotaAux, posicao + 1,
                                                    poluicaoAux, combustivelAux, (*clienteEscolhido)->cliente, mvStr,
                                                    clientePtr->cliente, veiculo->carga, 0);

                    if(resultado.viavel)
                    {
                        //Se a nova solucao é melhor do que a solucao atual, atualiza solucao
                        if(resultado.poluicao < poluicaoBest)
                        {

                            copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest,resultado, &posicaoVetBest);

                            if(pertubacao)
                                break;
                        }
                    }

                }
            }

            //Adicionar clienteIt ao vetClienteRotaAux.
            vetClienteRotaAux[posicao+1] = *clientePtr;

            peso -= instancia->vetorClientes[vetClienteRotaAux[posicao+1].cliente].demanda;
            combustivelRotaParcial += vetClienteRotaAux[posicao+1].combustivel;
            poluicaoRotaParcial += vetClienteRotaAux[posicao+1].poluicao;

            posicao += 1;
            clienteIt++;

            continue;

        }
        else
        {

            //Criar arco entre posicao - clientePtr. Calcular o fim da rota substituindo, clientePtr por clienteEscolhido. Posicao não é atualizado.
            vetClienteRotaAux[posicao+1].cliente = clientePtr->cliente;

            if(Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, peso, veiculo->tipo))
            {
                //Atualiza variaveis

                combustivelAux = combustivelRotaParcial + vetClienteRotaAux[posicao+1].combustivel;
                poluicaoAux = poluicaoRotaParcial + vetClienteRotaAux[posicao+1].poluicao;
                pesoAux = peso - instancia->vetorClientes[vetClienteRotaAux[posicao+1].cliente].demanda;

                //cout<<"Depois clienteEscolhido.\n";

                //Calcula o fim da rota
                ResultadosRota resultadosRota = calculaFimRota(instancia, veiculo, proxEscolhido, pesoAux,
                                                               vetClienteRotaAux, posicao + 1,
                                                               poluicaoAux, combustivelAux, clientePtr->cliente,
                                                               mvStr, ptrEscolhido->cliente, veiculo->carga, 0);

                //Verifica viabilidade
                if(resultadosRota.viavel)
                {   //cout<<"Viavel caso 2\n";
                    if(resultadosRota.poluicao < poluicaoBest)
                    {
                        posicaoVetBest = resultadosRota.posicaoVet;

                        copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest,
                                     resultadosRota, &posicaoVetBest);

                        if(pertubacao)
                            break;
                    }
                }


            }

            clienteIt++;

        }

    }

    if(poluicaoBest == HUGE_VALF)
        return {.viavel = false};
    else
    {
        return {.poluicao = poluicaoBest, .combustivel = combustivelBest, .peso = veiculo->carga, .viavel = true, .posicaoVet = posicaoVetBest, .veiculo = veiculo, .veiculoSecundario = NULL,
                .poluicaoSecundario = -1.0, .combustivelSecundario = -1.0, .pesoSecundario = -1, .posicaoVetSecundario = -1};
    }

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
 */

ResultadosRota Movimentos::mvInterRotasShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                                                Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest, bool pertubacao)

{

    string mvStr =  "mvInterRotasShift";

    int veiculoEscolhido1;      //Quarda o veiculo que irá receber um cliente
    int veiculoEscolhido2;      //Veiculo que retira um cliente

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

    while(true)
    {
        if((veiculoEscolhido1 != veiculoEscolhido2) && (veiculo1->listaClientes.size() > 2) && (veiculo2->listaClientes.size() > 2))
            break;
        else
        {
            while(((veiculo2->listaClientes.size() <= 2)))
            {

                veiculoEscolhido2 += 1;
                if (solucao->veiculoFicticil)
                    veiculoEscolhido2 %= (solucao->vetorVeiculos.size() - 1);
                else
                    veiculoEscolhido2 %= solucao->vetorVeiculos.size();

                veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];
            }


            while((veiculoEscolhido1 == veiculoEscolhido2) || veiculo1->listaClientes.size() <= 2)
            {
                veiculoEscolhido1 += 1;
                if (solucao->veiculoFicticil)
                    veiculoEscolhido1 %= (solucao->vetorVeiculos.size() - 1);
                else
                    veiculoEscolhido1 %= solucao->vetorVeiculos.size();

                veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
            }
        }

    }


    //Escolhe um cliente do veiculo2
    u_int32_t posicaoClienteEscolhido =  rand_u32() ;
    u_int32_t tam = (veiculo2->listaClientes.size() - 2);

    posicaoClienteEscolhido = posicaoClienteEscolhido % tam;

    posicaoClienteEscolhido += 1;

    auto clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
    auto escolhido = *clienteEscolhido;
    bool inicioRota = true;

    const int PosicaoClienteOriginal = posicaoClienteEscolhido;

    if((veiculo1->carga + instancia->vetorClientes[escolhido->cliente].demanda) > instancia->vetorVeiculos[veiculo1->tipo].capacidade)
    {
        posicaoClienteEscolhido += 1;
        posicaoClienteEscolhido = 1 + posicaoClienteEscolhido % (veiculo2->listaClientes.size() - 2);
        clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
        escolhido = *clienteEscolhido;

        while((veiculo1->carga + instancia->vetorClientes[escolhido->cliente].demanda) > instancia->vetorVeiculos[veiculo1->tipo].capacidade)
        {

            posicaoClienteEscolhido += 1;
            posicaoClienteEscolhido = 1 + posicaoClienteEscolhido % (veiculo2->listaClientes.size() - 2);
            clienteEscolhido = std::next(veiculo2->listaClientes.begin(), posicaoClienteEscolhido);
            escolhido = *clienteEscolhido;

            if(posicaoClienteEscolhido == PosicaoClienteOriginal)
            {


                return {.viavel = false};
            }
        }
    }

    //Variaveis de combustivel, poluicao
    double combustivelRotaParcialVeic1 = 0.0, poluicaoRotaParcialVeic1 = 0.0;
    double combustivelAuxVeic1, poluicaoAuxVeic1, combustivelBestVeic1, poluicaoBestVeic1 = HUGE_VALF; //Rota completa

    double combustivelBestVeic2, poluicaoBestVeic2;

    //Ajusta o peso dos veiculo
    int pesoVeic1 = veiculo1->carga + instancia->vetorClientes[escolhido->cliente].demanda;
    int pesoVeic2 = veiculo2->carga - instancia->vetorClientes[escolhido->cliente].demanda;
    int  pesoAux;

    const int PesoVeiculo1 = pesoVeic1;
    const int PesoVeiculo2 = pesoVeic2;

    vetClienteRotaAux[0] = **veiculo1->listaClientes.begin();
    int posicao = 0;
    Solucao::ClienteRota *clientePtr;
    Solucao::ClienteRota *nextClienteRota;



    string strVeiculo1 = veiculo1->printRota();
    string strVeiculo2 = veiculo2->printRota();
    int posicaoBestVeic1;


    for(auto clienteIt = std::next(veiculo1->listaClientes.begin(), 1); clienteIt != veiculo1->listaClientes.end();)
    {

        clientePtr = *clienteIt;

        if(clientePtr->cliente == 8)
            teste::breakPoint();

        //Verifica se chegou ao deposito
        if (!inicioRota && clientePtr->cliente == 0)
            break;

        if (inicioRota)
            inicioRota = false;

        //Atualiza o próximo cliente
        auto nextIt = clienteIt;
        nextIt++;
        nextClienteRota = *nextIt;

        const Solucao::ClienteRota clienteRotaPosicao = vetClienteRotaAux[posicao];

        vetClienteRotaAux[posicao + 1].cliente = (*clienteEscolhido)->cliente;

        //Calcular rota entre posicao e posicao + 1
        if(Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, pesoVeic1, veiculo1->tipo))
        {
            combustivelAuxVeic1 = combustivelRotaParcialVeic1;
            poluicaoAuxVeic1 = poluicaoRotaParcialVeic1;

            auto cliente = vetClienteRotaAux[posicao+1];

            combustivelAuxVeic1 += vetClienteRotaAux[posicao+1].combustivel;
            poluicaoAuxVeic1 += vetClienteRotaAux[posicao+1].poluicao;

            //Verifica combustivel
            if(VerificaSolucao::verificaCombustivel(combustivelAuxVeic1, veiculo1->tipo, instancia))
            {
                //calcula o resto da rota

                pesoAux = pesoVeic1;
                pesoAux -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;
                auto prox = clienteIt;
                auto ptr = *prox;

                auto resultado = calculaFimRota(instancia, veiculo1, prox, pesoAux, vetClienteRotaAux, posicao + 1,
                                                poluicaoAuxVeic1, combustivelAuxVeic1, -1, mvStr, -1, PesoVeiculo1,
                                                1);

                if (resultado.viavel)
                {
                    posicaoBestVeic1 = resultado.posicaoVet;

                    //Verifica se é melhor do que a melhor solucao
                    if (resultado.poluicao < poluicaoBestVeic1)
                    {
                        //Copia solucao para best

                        auto bestPtr = &vetClienteRotaBest[0];
                        auto auxPtr = &vetClienteRotaAux[0];

                        //Copia rota
                        for (int i = 0; i < resultado.posicaoVet + 1; ++i)
                        {
                            *bestPtr = *auxPtr;


                            bestPtr++;
                            auxPtr++;
                        }

                        poluicaoBestVeic1 = resultado.poluicao;
                        combustivelBestVeic1 = resultado.combustivel;

                        if (pertubacao)
                            break;

                    }
                }

            }
        }

        //Adicionar arco vetCliente[posicao] - clienteIt(posicao+1)

        vetClienteRotaAux[posicao+1] = **clienteIt;


        //Recalcula poluicao, combustivel relativo as cargas
        vetClienteRotaAux[posicao+1].poluicao = vetClienteRotaAux[posicao+1].poluicaoRota + VerificaSolucao::poluicaoCarga(instancia, veiculo1->tipo, pesoVeic1, instancia->matrizDistancias[vetClienteRotaAux[posicao].cliente][vetClienteRotaAux[posicao+1].cliente]);
        vetClienteRotaAux[posicao+1].combustivel = vetClienteRotaAux[posicao+1].combustivelRota + VerificaSolucao::combustivelCarga(instancia, veiculo1->tipo, pesoVeic1, instancia->matrizDistancias[vetClienteRotaAux[posicao].cliente][vetClienteRotaAux[posicao+1].cliente]);

        poluicaoRotaParcialVeic1 += vetClienteRotaAux[posicao+1].poluicao;
        combustivelRotaParcialVeic1 += vetClienteRotaAux[posicao+1].combustivel;

        if(!VerificaSolucao::verificaCombustivel(combustivelRotaParcialVeic1, veiculo1->tipo, instancia))
            break;

        pesoVeic1 -= instancia->vetorClientes[vetClienteRotaAux[posicao+1].cliente].demanda;

        posicao++;
        clienteIt++;

    }



    if(poluicaoBestVeic1 == HUGE_VALF)
        return {.viavel = false};

    string resultadoVeiculo1 = "";

    for (int i = 0; i < posicaoBestVeic1 + 1; ++i)
    {
        resultadoVeiculo1 += std::to_string(vetClienteRotaBest[i].cliente) + ' ';
    }

    posicao = 0;
    ResultadosRota resultadoVeic2;
    double poluicaoRotaVeic2 = 0.0, combustivelRotaVeic2 = 0.0;
    vetClienteRotaSecundBest[0] = *(*veiculo2->listaClientes.begin());

    string resultadoVeiculo2 = "";



    //Recalcular veiculo2 sem escolhido. Até escolhido, recalcular poluicao/combustivel relativo a carga, depois recalcular rota.
    for(auto clienteIt = std::next(veiculo2->listaClientes.begin(),1); clienteIt != veiculo2->listaClientes.end();)
    {
        clientePtr = *clienteIt;

        if(clientePtr->cliente == (*clienteEscolhido)->cliente)
        {

            clienteIt++;
            //cout<<vetClienteRotaSecundBest[0].cliente<<"\n\n";
            resultadoVeic2 = calculaFimRota(instancia, veiculo2, clienteIt, pesoVeic2, vetClienteRotaSecundBest,
                                            posicao, poluicaoRotaVeic2, combustivelRotaVeic2, -1, mvStr, -1,
                                            PesoVeiculo2, -1);

            for(int i = 0; i < resultadoVeic2.posicaoVet+1; ++i)
                resultadoVeiculo2 += std::to_string(vetClienteRotaSecundBest[i].cliente) + ' ';
            break;

        }

        vetClienteRotaSecundBest[posicao+1] = *clientePtr;
        int clieinte1, cliente2;
        clieinte1 = vetClienteRotaSecundBest[posicao].cliente;
        cliente2 = vetClienteRotaSecundBest[posicao+1].cliente;


        //recalcula poluicao/combustivel das cargas
        vetClienteRotaSecundBest[posicao+1].poluicao += vetClienteRotaSecundBest[posicao+1].poluicaoRota + VerificaSolucao::poluicaoCarga(instancia, veiculo2->tipo, pesoVeic2, instancia->matrizDistancias[clieinte1][cliente2]);
        vetClienteRotaSecundBest[posicao+1].combustivel += vetClienteRotaSecundBest[posicao+1].combustivelRota + VerificaSolucao::combustivelCarga(instancia, veiculo2->tipo, pesoVeic2, instancia->matrizDistancias[clieinte1][cliente2]);

        poluicaoRotaVeic2 += vetClienteRotaSecundBest[posicao+1].poluicao;
        combustivelRotaVeic2 += vetClienteRotaSecundBest[posicao+1].combustivel;

        if(VerificaSolucao::verificaCombustivel(combustivelRotaVeic2, veiculo2->tipo, instancia))
        {
            resultadoVeic2.viavel = false;
            break;
        }

        pesoVeic2 -= instancia->vetorClientes[cliente2].demanda;
        posicao += 1;
        clienteIt++;

    }


    if(!resultadoVeic2.viavel)
        return {.viavel = false};



    ResultadosRota resultados;

    resultados.viavel = true;

    resultados.combustivel = combustivelBestVeic1;
    resultados.poluicao = poluicaoBestVeic1;
    resultados.veiculo = veiculo1;
    resultados.posicaoVet = posicaoBestVeic1;

    int peso = instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;
    resultados.peso = PesoVeiculo1;

    resultados.poluicaoSecundario = resultadoVeic2.poluicao;
    resultados.combustivelSecundario = resultadoVeic2.combustivel;
    resultados.veiculoSecundario = veiculo2;
    resultados.pesoSecundario = PesoVeiculo2;
    resultados.posicaoVetSecundario = resultadoVeic2.posicaoVet;

    return resultados;



}
/*
 *
 */
ResultadosRota Movimentos::mvInterRotasSwap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                            Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest,Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao)
{

    int veiculoEscolhido1;
    int veiculoEscolhido2;

    string mvStr =  "mvInterRotasSwap";

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

    while(true)
    {
        if((veiculoEscolhido1 != veiculoEscolhido2) && (veiculo1->listaClientes.size() > 2) && (veiculo2->listaClientes.size() > 2))
            break;
        else
        {
            while(((veiculo2->listaClientes.size() <= 2)))
            {

                veiculoEscolhido2 += 1;
                if (solucao->veiculoFicticil)
                    veiculoEscolhido2 %= (solucao->vetorVeiculos.size() - 1);
                else
                    veiculoEscolhido2 %= solucao->vetorVeiculos.size();

                veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];
            }


            while((veiculoEscolhido1 == veiculoEscolhido2) || veiculo1->listaClientes.size() <= 2)
            {
                veiculoEscolhido1 += 1;
                if (solucao->veiculoFicticil)
                    veiculoEscolhido1 %= (solucao->vetorVeiculos.size() - 1);
                else
                    veiculoEscolhido1 %= solucao->vetorVeiculos.size();

                veiculo1 = solucao->vetorVeiculos[veiculoEscolhido1];
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
    int pesoVeic1T;
    int pesoVeic2T;
    int  pesoAux;
    vetClienteRotaAux[0] = **veiculo1->listaClientes.begin();

    //Copia veiculo2 para vetClienteRotaSecundAux até cliente escolhido
    int k = 0;
    for(auto it = veiculo2->listaClientes.begin();it != clienteEscolhido; ++it,++k)
        vetClienteRotaSecundAux[k] = **it;

    int posicao = 0;
    Solucao::ClienteRota *clientePtr;
    string strVeiculo1 = veiculo1->printRota();
    string strVeiculo2 = veiculo2->printRota();
    bool inicioRota = true;
    const int demandaEscolhido = instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;

    for(auto clienteIt = std::next(veiculo1->listaClientes.begin(), 1); clienteIt != veiculo1->listaClientes.end();)
    {

        clientePtr = *clienteIt;

        if (clientePtr->cliente == 8)
            teste::breakPoint();

        //Verifica se chegou ao deposito
        if (!inicioRota && clientePtr->cliente == 0)
            break;

        if (inicioRota)
            inicioRota = false;

        //Verificar pesos

        //Calcula novo peso do veiculo1
        pesoVeic1T = veiculo1->carga - instancia->vetorClientes[clientePtr->cliente].demanda;
        pesoVeic1T += demandaEscolhido;

        //Verificar peso
        if(pesoVeic1T < instancia->vetorVeiculos[veiculo1->tipo].capacidade)
        {
            //Calcula novo peso do veiculo2
            pesoVeic2T = veiculo2->carga - demandaEscolhido;
            pesoVeic2T += instancia->vetorClientes[clientePtr->cliente].demanda;

            //Verificar peso
            if(pesoVeic2T < instancia->vetorVeiculos[veiculo2->tipo].capacidade)
            {
                //Para o veiculo1: Recalcular poluicao/combustivel, criar arco estre posicao e escolhido e calcular o fim da rota.
                //Para o veiculo2: Recalcular poluicao/combustivel das cargas até escolhido. Calcular rota com clienteIt.

                vetClienteRotaAux[posicao + 1].cliente = (*clienteEscolhido)->cliente;
                combustivelAuxVeic1 = poluicaoAuxVeic1 = 0.0;

                int cliente1, cliente2;
                pesoAux = pesoVeic1T;

                if(recalculaCombustivelPoluicaoCargas(veiculo1, &poluicaoAuxVeic1, &combustivelAuxVeic1, &pesoAux, instancia, vetClienteRotaAux, posicao))
                {
                    auto next = clienteIt;
                    next++;

                    ResultadosRota resultadosRotaVeic1{.viavel = false}, resultadosRotaVeic2{.viavel=false};

                    //Adiciona o novo cliente
                    if(Construtivo::determinaHorario(&vetClienteRotaAux[posicao], &vetClienteRotaAux[posicao+1], instancia, pesoAux, veiculo1->tipo))
                    {
                        poluicaoAuxVeic1 += vetClienteRotaAux[posicao+1].poluicao;
                        combustivelAuxVeic1 += vetClienteRotaAux[posicao+1].combustivel;
                        pesoAux -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;

                        if(VerificaSolucao::verificaCombustivel(combustivelAuxVeic1, veiculo1->tipo, instancia))
                            resultadosRotaVeic1 = calculaFimRota(instancia, veiculo1, next, pesoAux, vetClienteRotaAux,
                                                                 posicao + 1, poluicaoAuxVeic1, combustivelAuxVeic1, -1,
                                                                 mvStr, -1, pesoVeic1T, 0);


                    }

                    if(resultadosRotaVeic1.viavel)
                    {
                        //Adicionar clienteIt ao veiculo2

                        pesoAux = pesoVeic2T;
                        int posicaoVeic2 = posicaoClienteEscolhido - 1;

                        //Recalcular combustivel/carga do veiculo2 com a nova carga

                        combustivelAuxVeic2 = poluicaoAuxVeic2 = 0.0;

                        if(recalculaCombustivelPoluicaoCargas(veiculo2, &poluicaoAuxVeic2, &combustivelAuxVeic2, &pesoAux, instancia, vetClienteRotaSecundAux, posicaoVeic2))
                        {
                            //Adicioinar clienteIt ao veiculo2

                            vetClienteRotaSecundAux[posicaoVeic2+1].cliente = clientePtr->cliente;

                            if(Construtivo::determinaHorario(&vetClienteRotaSecundAux[posicaoVeic2], &vetClienteRotaSecundAux[posicaoVeic2+1], instancia, pesoAux, veiculo2->tipo))
                            {
                                //Atualiza variaveis

                                combustivelAuxVeic2 += vetClienteRotaSecundAux[posicaoVeic2+1].combustivel;
                                poluicaoAuxVeic2 += vetClienteRotaSecundAux[posicaoVeic2+1].poluicao;

                                pesoAux -= instancia->vetorClientes[clientePtr->cliente].demanda;

                                if(VerificaSolucao::verificaCombustivel(combustivelAuxVeic2, veiculo2->tipo, instancia))
                                {
                                    auto nexClienteEscolhido = clienteEscolhido;
                                    nexClienteEscolhido++;

                                    resultadosRotaVeic2 = calculaFimRota(instancia, veiculo2, nexClienteEscolhido,
                                                                         pesoAux, vetClienteRotaSecundAux,
                                                                         posicaoVeic2 + 1, poluicaoAuxVeic2,
                                                                         combustivelAuxVeic2, -1, mvStr, -1,
                                                                         pesoVeic2T, 0);
                                }

                            }

                        }

                        if(resultadosRotaVeic2.viavel)
                        {   //cout<<"Gerou solucao\n";
                            //Verificar se a solucao atual é melhor que a melhor solucao

                            double poluicaoNovaSolucao = resultadosRotaVeic1.poluicao + resultadosRotaVeic2.poluicao;

                            if(poluicaoNovaSolucao < (poluicaoBestVeic1 + poluicaoBestVeic2))
                            {
                                //Atualizar nova solucao

                                pesoBestVeic1 = pesoVeic1T;
                                pesoBestVeic2 = pesoVeic2T;

                                copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBestVeic1, &combustivelBestVeic1, resultadosRotaVeic1, &posicaoBestVeic1);
                                copiaSolucao(vetClienteRotaSecundBest, vetClienteRotaSecundAux, &poluicaoBestVeic2, &combustivelBestVeic2, resultadosRotaVeic2, &posicaoBestVeic2);

                                if(pertubacao)
                                    break;

                            }

                        }
                    }

                }


            }
        }

        //Escrever clienteIt em vetClienteAux

        vetClienteRotaAux[posicao+1] = *clientePtr;
        posicao++;
        clienteIt++;

    }

    if(poluicaoBestVeic1 == HUGE_VALF)
        return {.viavel = false};

    ResultadosRota resultados;

    resultados.viavel = true;

    resultados.combustivel = combustivelBestVeic1;
    resultados.poluicao = poluicaoBestVeic1;
    resultados.veiculo = veiculo1;
    resultados.posicaoVet = posicaoBestVeic1;

    resultados.peso = pesoBestVeic1;

    resultados.poluicaoSecundario = poluicaoBestVeic2;
    resultados.combustivelSecundario = combustivelBestVeic2;
    resultados.veiculoSecundario = veiculo2;
    resultados.pesoSecundario = pesoBestVeic2;
    resultados.posicaoVetSecundario = posicaoBestVeic2;

    return resultados;

}

bool Movimentos::recalculaCombustivelPoluicaoCargas(Solucao::Veiculo *veiculo, double *poluicao, double *combustivel, int *pesoTotal, const Instancia::Instancia *const instancia,
                                        Solucao::ClienteRota *vetClienteRotaAux, int posicao)
{
    int cliente1, cliente2;

    //Recalcular poluicao/combustivel das cargas
    for(int i = 1; i <= posicao; ++i)
    {
        cliente1 = vetClienteRotaAux[i-1].cliente;
        cliente2 = vetClienteRotaAux[i].cliente;

        double dist = instancia->matrizDistancias[cliente1][cliente2];

        vetClienteRotaAux[i].poluicao = vetClienteRotaAux[i].poluicaoRota + VerificaSolucao::poluicaoCarga(instancia, veiculo->tipo, *pesoTotal, dist);
        vetClienteRotaAux[i].combustivel = vetClienteRotaAux[i].combustivelRota + VerificaSolucao::combustivelCarga(instancia, veiculo->tipo, *pesoTotal, dist);

        *combustivel += vetClienteRotaAux[i].combustivel;
        *poluicao += vetClienteRotaAux[i].poluicao;

        *pesoTotal -= instancia->vetorClientes[cliente2].demanda;
    }

    if(VerificaSolucao::verificaCombustivel(*combustivel, veiculo, instancia))
        return true;
    else
        return false;


}

void Movimentos::copiaSolucao(Solucao::ClienteRota *bestPtr, Solucao::ClienteRota *auxPtr, double *poluicaoBest, double *combustivelBest, ResultadosRota resultado, int *posicao)
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

    if(posicao)
        *posicao = resultado.posicaoVet;

}

ResultadosRota Movimentos::recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                         Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin)
{

    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto clienteAlvo = clienteEscolhido;

    if(posicaoAlvo > 1)
        clienteAlvo = std::next(veiculo->listaClientes.begin(), posicaoAlvo);

    double poluicao , combustivel;
    poluicao = combustivel = 0.0;


    //Copiar lista para vetClienteRota, até posicaoAlvo. Se clienteEscolhido for achado, deve-se calcular a rota sem ele.
    for(auto it = std::next(veiculo->listaClientes.begin(), begin); it != veiculo->listaClientes.end(); )
    {


        if((*it)->cliente != (*clienteEscolhido)->cliente && (posicaoAlvo < 0 || (*it)->cliente != (*clienteAlvo)->cliente))
        {
            //Copia it para o vetor e atualiza as variaveis.
            vetClienteRotaAux[posicaoVet] = **it;

            //Recalcular poluicao e combustivel das rotas.

            auto proximo = it;
            proximo++;

            vetClienteRotaAux[posicaoVet].poluicao = vetClienteRotaAux[posicaoVet].poluicaoRota + VerificaSolucao::poluicaoCarga(instancia, veiculo->tipo, peso, instancia->matrizDistancias[(**it).cliente][(**proximo).cliente]);
            vetClienteRotaAux[posicaoVet].combustivel = vetClienteRotaAux[posicaoVet].combustivelRota + VerificaSolucao::combustivelCarga(instancia, veiculo->tipo, peso, instancia->matrizDistancias[(**it).cliente][(**proximo).cliente]);

            //Atualiza variaveis
            peso -= instancia->vetorClientes[(**it).cliente].demanda;
            poluicao += vetClienteRotaAux[posicaoVet].poluicao;
            combustivel += vetClienteRotaAux[posicaoVet].combustivel;


        }
        else if((posicaoAlvo > 0) && (*it)->cliente == (*clienteAlvo)->cliente)
        {
            //Somente insere it no vetor.
            vetClienteRotaAux[posicaoVet] = **it;
            peso -= instancia->vetorClientes[(**it).cliente].demanda;

            break;
        }
        else
        {
            //it é igual a  clienteEscolhido

            auto itAntes = it;
            itAntes--;
            it++;
            peso -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;

            vetClienteRotaAux[posicaoVet+1] = **it;

            //Rota entre o cliente anterior de it e o cliente posterior de it
            if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet+1],instancia, peso, veiculo->tipo))
            {
                ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                return resultado;
            }

            //Atualiza valores
            poluicao += vetClienteRotaAux[posicaoVet+1].poluicao;
            combustivel+= vetClienteRotaAux[posicaoVet+1].combustivel;
            peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet+1].cliente].demanda;

            posicaoVet += 1;

            // Cliente alvo está depois de clienteEscolhido. Recalcular rotas até cliente alvo.
            while(true)
            {
                it++;

                vetClienteRotaAux[posicaoVet+1] = **it;
                if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet+1],instancia, peso, veiculo->tipo))
                {
                    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                //Atualiza valores
                poluicao += vetClienteRotaAux[posicaoVet+1].poluicao;
                combustivel+= vetClienteRotaAux[posicaoVet+1].combustivel;
                peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet].cliente].demanda;
                posicaoVet += 1;

                //Verifical combustivel
                if((combustivel - instancia->vetorVeiculos[veiculo->tipo].combustivel) >= -0.001)
                {
                    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                //Verifica se encontrou cliente alvo ou o deposito
                if((*it)->cliente == (*clienteAlvo)->cliente || (*it)->cliente == 0)
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
ResultadosRota Movimentos::calculaFimRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                          auto proximoClienteIt, int peso,
                                          Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, double poluicao,
                                          double combustivel,
                                          const int clienteEscolhido, std::string mvStr, const int substituto,
                                          const int pesoTotal,
                                          const int maisclientes)
{


    //Calcular rota do ultimo cliente em vetCliente
    if(clienteEscolhido == 0)
        throw  exceptionEscolhido;
    for(auto itCliente = proximoClienteIt; itCliente != veiculo->listaClientes.end(); )
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

    /*auto veiculoPtr = new Solucao::Veiculo(veiculo->tipo);



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
        cout<<"Func pai: "<<mvStr<<'\n';
        throw exceptionVeiculo;
    }

    delete veiculoPtr;*/

    //Peso não se aplica.
    return {.poluicao = poluicao, .combustivel = combustivel,  .peso = -1, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};


}


//**************************************************************************************************************************************************
//**************************************************************************************************************************************************


Movimentos::ResultadosRota Movimentos::inverteRota(auto itInicio, auto itFim, Solucao::ClienteRota *vetClienteRotaAux, int posicao, int peso, double poluicao,
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
Movimentos::ResultadosRota Movimentos::mv_2optSwapIntraRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{

    int veiculoEscolhido;      //guarda o veiculo

    string mvStr =  "mv_2opt_intraRota";
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

    //cout<<"Escolhido "<<escolhido->cliente<<'\n';

    double combustivelRotaParcial = 0.0, poluicaoRotaParcial = 0.0, combustivelAux, poluicaoAux;
    double combustivelBest, poluicaoBest = HUGE_VALF;
    int posicaoBestVeic, posicaoAux, pesoBest;

    int pesoVeic = veiculo->carga;
    int pesoAux;

    vetClienteRotaAux[0] = **veiculo->listaClientes.begin();
    int posicao = 0;
    int indexListaClientes = 1;
    bool clientePassouEscolhido = false;
    auto next = veiculo->listaClientes.begin();

    for(auto clienteIt = std::next(veiculo->listaClientes.begin(),1); clienteIt != veiculo->listaClientes.end(); )
    {

        next = clienteIt;

        if((*next)->cliente == 0)
            break;

        next++;

        if((*next)->cliente == 0)
            break;

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
        Movimentos::ResultadosRota resultadosRota = Movimentos::inverteRota(inicio, fim, vetClienteRotaAux, posicao, pesoVeic, poluicaoRotaParcial,
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
            resultadosRota = Movimentos::calculaFimRota(instancia, veiculo, fim, pesoAux, vetClienteRotaAux, posicaoAux,
                                                        poluicaoAux, combustivelAux, -1, mvStr, -1, veiculo->carga,
                                                        0);

            if(resultadosRota.viavel)
            {
                if(resultadosRota.poluicao < poluicaoBest)
                {
                    pesoBest = pesoAux;

                    Movimentos::copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota, &posicaoBestVeic);

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

    Movimentos::ResultadosRota resultadosRota;

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
Movimentos::ResultadosRota Movimentos::mv_2optSwapInterRotas(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                                             Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao)
{
    string mvStr = "2optInterRotas";

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
        {
            veiculoEscolhido2 = veiculoEscolhido2 % (solucao->vetorVeiculos.size() - 1);
        }
        else
        {
            veiculoEscolhido2 = veiculoEscolhido2 % solucao->vetorVeiculos.size();
        }

        veiculo2 = solucao->vetorVeiculos[veiculoEscolhido2];
        int tam = veiculo2->listaClientes.size();
        string s = veiculo2->printRota();
        int a = 2;
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

    int posicaoBestVeic = 0, pesoBest, pesoBestSec;
    int posicaoBestSec = 0;

    int pesoVeic1Parcial = 0;

    auto it = veiculo1->listaClientes.begin();
    vetClienteRotaAux[0] = **it;
    it++;
    vetClienteRotaAux[1] = **it;

    pesoVeic1Parcial += instancia->vetorClientes[(*it)->cliente].demanda;

    int posicao = 1;
    int k = 0;

    const int tipoVeic1 = veiculo1->tipo;
    const int tipoVeic2 = veiculo2->tipo;

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
        if((pesoVeic1DepClienteIt + pesoClientesVeic2Parcial) < instancia->vetorVeiculos[tipoVeic2].capacidade)
        {
            //Calcula os novos pesos

            int pesoNovoVeiculo2 = pesoVeic1DepClienteIt + pesoClientesVeic2Parcial;
            int pesoNovoVeiculo1 = (pesoClientesVeic2AposEscolhido) + pesoVeic1Parcial;

            //Verifica peso Veiculo1

            if(pesoNovoVeiculo1 < instancia->vetorVeiculos[tipoVeic1].capacidade)
            {

                combustivelAux = 0.0;
                poluicaoAux = 0.0;
                pesoAux = pesoNovoVeiculo1;

                //Calculo veiculo1: Recalcular poluicao/combustivel relativo as cargas até clienteIt
                if(Movimentos::recalculaCombustivelPoluicaoCargas(veiculo1, &poluicaoAux, &combustivelAux, &pesoAux, instancia, vetClienteRotaAux, posicao))
                {

                    auto itAux = clienteEscolhido;
                    itAux++;

                    //cout<<"viculo1\n";

                    //Calcula fim da rota adicionando clientes do veiculo2


                    Movimentos::ResultadosRota resultadosRotaVeic1 = Movimentos::calculaFimRota_2OptInter(instancia, veiculo1, veiculo2, itAux, pesoAux, vetClienteRotaAux, posicao,
                                                                                                          poluicaoAux, combustivelAux, -1, mvStr, -1, pesoNovoVeiculo1, 0);

                    if(resultadosRotaVeic1.viavel)
                    {

                        combustivelAux = 0.0;
                        poluicaoAux = 0.0;
                        pesoAux = pesoNovoVeiculo2;

                        //Calculo veiculo2: Recalcular poluicao/combustivel relativo as cargas até clienteEscolhido
                        if(Movimentos::recalculaCombustivelPoluicaoCargas(veiculo2, &poluicaoAux, &combustivelAux, &pesoAux, instancia, vetClienteRotaSecundAux, posicaoClienteEscolhido))
                        {
                            itAux = clienteIt;
                            itAux++;

                            //cout<<"viculo2\n";

                            //Calcula fim da rota adicionando clientes do veiculo1

                            Movimentos::ResultadosRota resultadosRotaVeic2 = Movimentos::calculaFimRota_2OptInter( instancia, veiculo2, veiculo1, itAux, pesoAux, vetClienteRotaSecundAux,
                                                                                                                   posicaoClienteEscolhido, poluicaoAux, combustivelAux, -1, mvStr, -1, pesoNovoVeiculo2, 0);

                            if(resultadosRotaVeic2.viavel)
                            {

                                if((resultadosRotaVeic1.poluicao + resultadosRotaVeic2.poluicao) < (poluicaoBest + poluicaoBestSec))
                                {
                                    //Atualiza a melhor solucao

                                    pesoBest = pesoNovoVeiculo1;
                                    posicaoBestVeic = resultadosRotaVeic1.posicaoVet;

                                    Movimentos::copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRotaVeic1, NULL);

                                    pesoBestSec = pesoNovoVeiculo2;
                                    posicaoBestSec = resultadosRotaVeic2.posicaoVet;

                                    Movimentos::copiaSolucao(vetClienteRotaSecundBest, vetClienteRotaSecundAux, &poluicaoBestSec, &combustivelBestSec, resultadosRotaVeic2, NULL);



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

    string veic1 = "";

    for(int i = 0; i <= posicaoBestVeic; ++i)
        veic1 += std::to_string(vetClienteRotaBest[i].cliente) + ' ';

    string veic2 = "";

    for(int i = 0; i <= posicaoBestSec; ++i)
        veic2 += std::to_string(vetClienteRotaSecundBest[i].cliente) + ' ';

    Movimentos::ResultadosRota resultadosRota;

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

Movimentos::ResultadosRota Movimentos::mvIntraRotaInverteRota(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,Solucao::ClienteRota *vetClienteRotaBest,
                                                              Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{
    string mvStr = "inverteRota";

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

        Movimentos::ResultadosRota resultadosRota = inverteRota(veiculo->listaClientes.begin(), itFim, vetClienteRotaAux, 0, veiculo->carga, 0, 0, instancia, veiculo->tipo);

        if(resultadosRota.viavel)
        {


            if(bestVeiculo)
            {
                if((veiculo->poluicao - resultadosRota.poluicao) > (bestVeiculo->poluicao  - poluicaoBest))
                {
                    //cout << "Viavel.\n";
                    bestVeiculo = veiculo;
                    pesoBest = veiculo->carga;

                    Movimentos::copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota, &posicaoBest);

                    if ((resultadosRota.poluicao < veiculo->poluicao) || pertubacao)
                        break;
                }
            }
            else
            {
                bestVeiculo = veiculo;
                pesoBest = veiculo->carga;

                Movimentos::copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest, resultadosRota, &posicaoBest);

                if ((resultadosRota.poluicao < veiculo->poluicao) || pertubacao)
                    break;
            }

        }
    }

    if(bestVeiculo == NULL)
        return {.viavel = NULL};

    Movimentos::ResultadosRota resultadosRota;

    resultadosRota.viavel = true;
    resultadosRota.veiculo = bestVeiculo;
    resultadosRota.poluicao = poluicaoBest;
    resultadosRota.combustivel = combustivelBest;
    resultadosRota.peso = pesoBest;
    resultadosRota.posicaoVet = posicaoBest;
    resultadosRota.veiculoSecundario = NULL;

    return resultadosRota;
}

Movimentos::ResultadosRota Movimentos::mvTrocarVeiculos(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                                        Solucao::ClienteRota *vetClienteRotaAux, Solucao::ClienteRota *vetClienteRotaSecundBest,Solucao::ClienteRota *vetClienteRotaSecundAux, bool pertubacao)
{
    string mvStr = "trocaVeiculo";

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

    Movimentos::ResultadosRota resultadosRota{.viavel = false};
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

            if(veiculo1->carga < instancia->vetorVeiculos[novoTipo].capacidade)
            {

                vetClienteRotaBest[0] = **solucao->vetorVeiculos[novoTipo]->listaClientes.begin();

                auto it = veiculo1->listaClientes.begin(); //0
                it++;

                resultadosRota = Movimentos::calculaFimRota_2OptInter(instancia, solucao->vetorVeiculos[novoTipo],
                                                                      veiculo1, it, veiculo1->carga, vetClienteRotaBest,
                                                                      0, 0, 0, -1, mvStr, -1,
                                                                      veiculo1->carga, 0);
            }
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

        Movimentos::ResultadosRota resultadosRota2 = Movimentos::calculaFimRota_2OptInter(instancia,
                                                                                          solucao->vetorVeiculos[novoTipoVeic2],
                                                                                          veiculo2, it, veiculo2->carga,
                                                                                          vetClienteRotaSecundBest, 0,
                                                                                          0, 0, -1,
                                                                                          mvStr, -1,
                                                                                          veiculo2->carga, 0);

        if(resultadosRota2.viavel)
        {
            if(((resultadosRota.poluicao + resultadosRota2.poluicao) < (veiculo1->poluicao + veiculo2->poluicao)) || pertubacao)
            {
                Movimentos::ResultadosRota resultados;

                string s = veiculo1->getRota();
                string r1;

                double saida1 = (**veiculo1->listaClientes.begin()).tempoSaida;
                double saida2 = vetClienteRotaBest->tempoSaida;

                for(auto ptr = vetClienteRotaBest; ptr != &vetClienteRotaBest[resultadosRota.posicaoVet+1]; ptr++)
                    r1 += std::to_string(ptr->cliente) + ' ';

                string s2 = veiculo2->getRota();
                string r2;

                for(auto ptr = vetClienteRotaSecundBest; ptr != &vetClienteRotaSecundBest[resultadosRota2.posicaoVet+1]; ptr++)
                    r2 += std::to_string(ptr->cliente) + ' ';

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

ResultadosRota
Movimentos::calculaFimRota_2OptInter(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                     Solucao::Veiculo *veiculo2, auto proximoClienteIt, int peso,
                                     Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, double poluicao,
                                     double combustivel, const int clienteEscolhido, std::string mvStr,
                                     const int substituto, const int pesoTotal, const int maisclientes)
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
            Movimentos::ResultadosRota resultados = {.viavel=false};
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
        int aux = vetClienteRotaAux[i].cliente;
        auto cliente = new Solucao::ClienteRota;
        *cliente = vetClienteRotaAux[i];
        veiculoPtr->listaClientes.push_back(cliente);

    }

    /*auto clientePtr = *std::next(veiculoPtr->listaClientes.begin(), 1);


    veiculoPtr->poluicao = poluicao;
    veiculoPtr->combustivel = combustivel;
    veiculoPtr->carga = pesoTotal;

    if(!VerificaSolucao::verificaVeiculo(veiculoPtr, instancia))
    {
        cout<<"Carga: "<<veiculoPtr->carga<<"\nCapacidade: "<<instancia->vetorVeiculos[veiculo->tipo].capacidade<<'\n';
        delete veiculoPtr;
        cout<<"Func pai: "<<mvStr<<'\n';
        throw exceptionVeiculo;
    }

    delete veiculoPtr;*/

    //Peso não se aplica.
    return {.poluicao = poluicao, .combustivel = combustivel,  .peso = -1, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};
}

Movimentos::ResultadosRota Movimentos::aplicaMovimento(int movimento, const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
                                                       Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao, Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux)
{

    switch(movimento)
    {
        case 0:
            return Movimentos::mvIntraRotaShift(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, pertubacao);
            break;

        case 1:
            return Movimentos::mvIntraRotaSwap(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, pertubacao);
            break;

        case 2:
            return Movimentos::mvInterRotasShift(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, vetClienteRotaSecundBest, pertubacao);
            break;
        case 3:
            return Movimentos::mvInterRotasSwap(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, vetClienteRotaSecundBest, vetClienteRotaSecundAux, pertubacao);
            break;

        case 4:
            return Movimentos::mv_2optSwapIntraRota(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, pertubacao);
            break;

        case 5:
            return Movimentos::mv_2optSwapInterRotas(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, vetClienteRotaSecundBest, vetClienteRotaSecundAux, pertubacao);
            break;

        case 6:
            return Movimentos::mvIntraRotaInverteRota(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, pertubacao);
            break;

        case 7:
            return Movimentos::mvTrocarVeiculos(instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, vetClienteRotaSecundBest, vetClienteRotaSecundAux, pertubacao);
            break;
    }

    exit(-1);
    return {.viavel=false};

}

void Movimentos::atualizaSolucao(ResultadosRota resultado, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaSecundBest)
{


    //Calcula a diferença
    int tam  = resultado.veiculo->listaClientes.size();
    int diferenca = (resultado.posicaoVet + 1) - tam;
    bool positivo = true;

    if(diferenca < 0)
        positivo = false;

    //Adiciona ou remove clientes da lista
    while(diferenca != 0)
    {

        if(positivo)
        {
            auto cliente = new Solucao::ClienteRota;
            resultado.veiculo->listaClientes.push_back(cliente);
            diferenca--;
        }
        else
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
        int cliente = vetClienteRotaBest[l].cliente;
        l += 1;
    }


    //Atualiza combustivel/poluiao/peso

    solucao->poluicao -= resultado.veiculo->poluicao;
    resultado.veiculo->combustivel = resultado.combustivel;
    resultado.veiculo->poluicao = resultado.poluicao;
    resultado.veiculo->carga = resultado.peso;
    solucao->poluicao += resultado.veiculo->poluicao;

    if(resultado.veiculoSecundario)
    {
        //Calcula a diferença
        diferenca = (resultado.posicaoVetSecundario + 1) - resultado.veiculoSecundario->listaClientes.size();

        positivo = true;

        if(diferenca < 0)
            positivo = false;

        //Adiciona ou remove clientes da lista
        while(diferenca != 0)
        {

            if(positivo)
            {
                auto cliente = new Solucao::ClienteRota;
                resultado.veiculoSecundario->listaClientes.push_back(cliente);
                diferenca--;
            }
            else
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
            int cliente = vetClienteRotaSecundBest[l].cliente;
            l += 1;
        }

        //Atualiza combustivel/poluiao/peso

        solucao->poluicao -= resultado.veiculoSecundario->poluicao;
        resultado.veiculoSecundario->combustivel = resultado.combustivelSecundario;
        resultado.veiculoSecundario->poluicao = resultado.poluicaoSecundario;
        resultado.veiculoSecundario->carga = resultado.pesoSecundario;
        solucao->poluicao += resultado.veiculoSecundario->poluicao;

    }
}
