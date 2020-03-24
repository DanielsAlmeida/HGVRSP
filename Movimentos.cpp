//
// Created by igor on 10/02/2020.
//

#include "Movimentos.h"
#include "Construtivo.h"
#include "mersenne-twister.h"

using namespace Movimentos;

bool Movimentos::mvIntraRotasReinsertion(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao)
{


    //Escolhe um veiculo

    int veiculoEscolhido = rand_u32() % solucao->vetorVeiculos.size();

    //Vevifica se a solução é inviavel
    if(solucao->veiculoFicticil && veiculoEscolhido == solucao->vetorVeiculos.size() - 1)
        veiculoEscolhido = 0;

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
                veiculoEscolhido = 0;

            veiculo = solucao->vetorVeiculos[veiculoEscolhido];
        }

    }


    int posicaoClienteEscolhido = 1 + rand_u32() % (veiculo->listaClientes.size() - 1);
    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);


    bool inicio = false;
    int peso = veiculo->carga;
    int posicaoVetorLivre = -1;


    for(auto it = veiculo->listaClientes.begin(); it != veiculo->listaClientes.end(); ++it)
    {
        if((*it)->cliente == (*clienteEscolhido)->cliente)
            continue;

        if(posicaoVetorLivre == -1)
        {


            if(Construtivo::determinaHorario(&vetClienteRotaAux[0], &vetClienteRotaAux[1], instancia, peso, veiculo->tipo))
            {
                auto itAux = it;
                itAux++;

                int posicao = 2;

                peso -= instancia->vetorClientes[(**clienteEscolhido).cliente].demanda;
                bool viavel = true;
                double poluicao = 0.0;
                double combustivel = instancia->vetorVeiculos[veiculo->tipo].combustivel;

                while(itAux != veiculo->listaClientes.end())
                {
                    vetClienteRotaAux[posicao] = **itAux;

                    if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicao-1], &vetClienteRotaAux[posicao], instancia,peso, veiculo->tipo))
                    {
                        viavel = false;
                        break;
                    }

                    peso -= instancia->vetorClientes[(**itAux).cliente].demanda;
                    combustivel -= vetClienteRotaAux[posicao].combustivel;
                    poluicao += vetClienteRotaAux[posicao].poluicao;
                    itAux++;
                    posicao += 1;

                    if(combustivel <= 0.0)
                    {
                        viavel = false;
                        break;
                    }
                }

                if(!viavel)
                {
                    continue;
                }
                else
                {

                }

            }

        }


    }


}

// 0 - 1 - 2 - 3 - 4 - 0


ResultadosRota Movimentos::recalculaRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo, int posicaoClienteEscolhido, int posicaoAlvo, int peso,
                                         Solucao::ClienteRota *vetClienteRotaAux, int posicaoVet, int begin)
{

    auto clienteEscolhido = std::next(veiculo->listaClientes.begin(), posicaoClienteEscolhido);
    auto clienteAlvo = std::next(veiculo->listaClientes.begin(), posicaoAlvo);

    double poluicao , combustivel;
    poluicao = combustivel = 0.0;


    //Copiar lista para vetClienteRota, até posicaoAlvo. Se clienteEscolhido for achado, deve-se calcular a rota sem ele.
    for(auto it = std::next(veiculo->listaClientes.begin(), begin); it != veiculo->listaClientes.end(); )
    {


        if((*it)->cliente != (*clienteEscolhido)->cliente && (*it)->cliente != (*clienteAlvo)->cliente)
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
        else if((*it)->cliente == (*clienteAlvo)->cliente)
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
            poluicao += vetClienteRotaAux[posicaoVet].poluicao;
            combustivel+= vetClienteRotaAux[posicaoVet].combustivel;
            peso -= instancia->vetorClientes[vetClienteRotaAux[posicaoVet+1].cliente].demanda;

            posicaoVet += 1;

            // Cliente alvo está depois de clienteEscolhido. Recalcular rotas até cliente alvo.
            while(true)
            {
                it++;

                //Se chegou no final, erro, porque cliente alvo nao esta lista.
                if(it == (*veiculo).listaClientes.end())
                    throw exceptionEndList;

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
                if(combustivel > instancia->vetorVeiculos[veiculo->tipo].combustivel)
                {
                    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = false, .posicaoVet = posicaoVet};
                    return resultado;
                }

                //Verifica se encontrou cliente alvo.
                if((*it)->cliente == (*clienteAlvo)->cliente)
                {
                    posicaoVet += 1;
                    break;
                }


            }

        }

        posicaoVet += 1;
        ++it;
    }

    ResultadosRota resultado = {.poluicao = poluicao, .combustivel = combustivel, .peso = peso, .viavel = true, .posicaoVet = posicaoVet};
    return resultado;

}

// 0-1-2-3-4-5-0
//0-1-3-2- => 0-1-3-2-4-5-0

//Calcula rota ate o final.
ResultadosRota Movimentos::calculaFimRota(const Instancia::Instancia *const instancia, Solucao::Veiculo *veiculo,
                                            int posicaoProximoCliente, int peso, Solucao::ClienteRota *vetClienteRotaAux,
                                            int posicaoVet, double poluicao, double combustivel)
{

    posicaoVet -= 1;

    //Calcular rota do ultimo cliente em vetCliente

    for(auto itCliente = std::next(veiculo->listaClientes.begin(), posicaoProximoCliente); itCliente != veiculo->listaClientes.end(); ++itCliente)
    {
        vetClienteRotaAux[posicaoVet + 1] = **itCliente;

        //Calcular rota entre posicaoVet e posicaoVet + 1
        if(!Construtivo::determinaHorario(&vetClienteRotaAux[posicaoVet], &vetClienteRotaAux[posicaoVet+1], instancia, peso, veiculo->tipo))
        {
            ResultadosRota resultados = {.viavel=false};
            return resultados;
        }

        //Atualiza combustivel e poluicao.
        peso -= instancia->vetorClientes[(*itCliente)->cliente].demanda;
        combustivel += vetClienteRotaAux[posicaoVet+1].combustivel;
        poluicao += vetClienteRotaAux[posicaoVet+1].poluicao;

        //Verifica peso
        if(peso < 0)
        {
            throw exceptionPeso;
        }

        //Verifica combustivel
        if(combustivel > instancia->vetorVeiculos[veiculo->tipo].combustivel)
        {
            ResultadosRota resultados = {.viavel = false};
            return resultados;
        }

        posicaoVet += 1;
    }

    //Peso não se aplica.
    ResultadosRota resultados = {.viavel = true, .combustivel = combustivel, .posicaoVet = posicaoVet, .poluicao = poluicao, .peso = -1};
    return resultados;

}
