//
// Created by igor on 10/02/2020.
//

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"

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

class ExceptionVeiculo: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, veiculo incorreto.\n";
    }
} exceptionVeiculo;

class ExceptionPeso: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: calculaFimRota. \nMotivo: Peso passado incorreto\n";
    }
} exceptionPeso;

class Exception: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: calculaFimRota. \nMotivo: clienteEscolhido eh igual a zero\n";
    }
} exceptionEscolhido;

ResultadosRota Movimentos::mvIntraRotaShift(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
                                            Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux,
                                            bool pertubacao)
{
    //Escolhe um veiculo

    int veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
        veiculoEscolhido = rand_u32() % (solucao->vetorVeiculos.size() - 1);

    auto *veiculo = solucao->vetorVeiculos[veiculoEscolhido];

    while(true)
    {
        if(veiculo->listaClientes.size() > 2)
            break;
        else
        {
            veiculoEscolhido += 1;
            veiculoEscolhido %= solucao->vetorVeiculos.size();

            if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
            {

                veiculoEscolhido = 0;
            }

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
    double combustivelAux, poluicaoAux, combustivelBest, poluicaoBest = HUGE_VALF; //Rota completa

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
            if( instancia->vetorVeiculos[veiculo->tipo].combustivel - combustivelAux >= -0.001)
            {


                //calcula o resto da rota
                pesoAux = peso;
                pesoAux -= instancia->vetorClientes[(*clienteEscolhido)->cliente].demanda;
                auto prox = clienteIt;
                prox++;

                //cout<<"\n\nposicao + 1: "<<posicao+1<<" Cliente: "<<vetClienteRotaAux[posicao+1].cliente<<'\n';

                auto resultado = calculaFimRota(instancia, veiculo, prox, pesoAux, vetClienteRotaAux, posicao + 1,
                                                poluicaoAux, combustivelAux, (*clienteEscolhido)->cliente);

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

        return {.poluicao = poluicaoBest, .combustivel = combustivelBest, .peso = veiculo->carga, .viavel = true, .posicaoVet = -1, .veiculo = veiculo, .veiculoSecundario = NULL,
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

                    auto resultado = calculaFimRota(instancia, veiculo, nextIt, pesoAux, vetClienteRotaAux, posicao+1, poluicaoAux, combustivelAux, (*clienteEscolhido)->cliente, clientePtr->cliente);

                    if(resultado.viavel)
                    {
                        //Se a nova solucao é melhor do que a solucao atual, atualiza solucao
                        if(resultado.poluicao < poluicaoBest)
                        {

                            copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest,
                                         resultado, 0);

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
                ResultadosRota resultadosRota = calculaFimRota(instancia, veiculo, proxEscolhido, pesoAux, vetClienteRotaAux, posicao+1,
                                                               poluicaoAux, combustivelAux, clientePtr->cliente, ptrEscolhido->cliente);

                //Verifica viabilidade
                if(resultadosRota.viavel)
                {   //cout<<"Viavel caso 2\n";
                    if(resultadosRota.poluicao < poluicaoBest)
                    {
                        copiaSolucao(vetClienteRotaBest, vetClienteRotaAux, &poluicaoBest, &combustivelBest,
                                     resultadosRota, 0);

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
        return {.poluicao = poluicaoBest, .combustivel = combustivelBest, .peso = veiculo->carga, .viavel = true, .posicaoVet = -1, .veiculo = veiculo, .veiculoSecundario = NULL,
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

    //cout<<"mvInterRotasShift\n\n";

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
        if((veiculoEscolhido1 != veiculoEscolhido2) && (veiculo1->listaClientes.size() > 2) && (veiculo1->listaClientes.size() > 2))
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
    int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo2->listaClientes.size() - 2);
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
                                                poluicaoAuxVeic1, combustivelAuxVeic1, -1, -1, PesoVeiculo1, 1);

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
            resultadoVeic2 = calculaFimRota(instancia, veiculo2, clienteIt, pesoVeic2, vetClienteRotaSecundBest, posicao, poluicaoRotaVeic2, combustivelRotaVeic2, -1, -1, PesoVeiculo2, -1);

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
        if((veiculoEscolhido1 != veiculoEscolhido2) && (veiculo1->listaClientes.size() > 2) && (veiculo1->listaClientes.size() > 2))
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
                            resultadosRotaVeic1 = calculaFimRota(instancia, veiculo1, next, pesoAux, vetClienteRotaAux, posicao + 1, poluicaoAuxVeic1, combustivelAuxVeic1, -1, -1, pesoVeic1T);


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

                                    resultadosRotaVeic2 = calculaFimRota(instancia, veiculo2, nexClienteEscolhido, pesoAux, vetClienteRotaSecundAux, posicaoVeic2+1, poluicaoAuxVeic2,
                                                                        combustivelAuxVeic2, -1, -1, pesoVeic2T);
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
    resultados.posicaoVetSecundario = pesoBestVeic2;

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
    for (int i = 0; i < resultado.posicaoVet + 1; ++i)
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
                                          auto proximoClienteIt, int peso, Solucao::ClienteRota *vetClienteRotaAux,
                                          int posicaoVet, double poluicao, double combustivel,
                                          const int clienteEscolhido, const int substituto, const int pesoTotal, const int maisclientes)
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