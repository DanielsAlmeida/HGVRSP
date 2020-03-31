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
        bool inicio = true;
        for(auto ptr = vetClienteRotaBest; ; ++ptr)
        {


            if(inicio)
                inicio = false;
            else
            {
                if(ptr->cliente == 0)
                {

                    break;
                }
            }

        }
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
                                          const int clienteEscolhido, const int substituto)
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

    auto veiculoPtr = new Solucao::Veiculo(*veiculo);

    int i = 0;

    for(auto it : veiculoPtr->listaClientes)
    {
        *it = vetClienteRotaAux[i];
        i += 1;
    }

    veiculoPtr->poluicao = poluicao;
    veiculoPtr->combustivel = combustivel;
    if(!VerificaSolucao::verificaVeiculo(veiculoPtr, instancia))
    {
        delete veiculoPtr;
        throw exceptionVeiculo;
    }

    delete veiculoPtr;

    //Peso não se aplica.
    return {.poluicao = poluicao, .combustivel = combustivel,  .peso = -1, .viavel = true, .posicaoVet = posicaoVet, .veiculoSecundario = NULL};


}