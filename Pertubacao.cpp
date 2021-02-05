//
// Created by igor on 08/01/2021.
//

#include "Pertubacao.h"
#include "mersenne-twister.h"
#include "Movimentos_Paradas.h"

using namespace Pertubacao;

typedef std::list<Solucao::ClienteRota*>::iterator IteratorListClienteRota;

/**
 *
 * A pertubação ira escolher 2 rotas distintas aleatoriamente e para cada rota escolhe um cliente e realiza a troca entre as rotas.
 * A viabilidade não inpede o movimento de ocorrer. A troca é realizada k vezes, com k > 1.
 *
 * Para viabilizar a rota são retirados os ultimos clientes da rota ate obter uma rota viavel. Os clientes que foram retirados são incluidos no veiculo victicil,
 * o vnd ira tentar viabilizar a solucao
 *
 * @param solucao a ser pertubada
 * @param k Numero de vezes que o swap sera realizado
 * @return  retorna true se conseguil realizar a pertubacao com o veiculo viavel
 */

bool Pertubacao::pertubacao_k_swap(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, const int k,
                                   Solucao::ClienteRota *vetClienteRota, Solucao::ClienteRota *vetClienteRotaAux,
                                   double *vetLimiteTempo, PertubacaoInviabilidade *inviabilidadeEstatisticas)
{

    bool vetRotaEscolhida[instancia->numVeiculos];

    for(int i = 0; i < instancia->numVeiculos; ++i)
        vetRotaEscolhida[i] = 0;

    int rota[2];
    rota[0] = rota[1] = -1;

    IteratorListClienteRota vetIterator[2];

    //Realiza o swap k vezes
    for(int kI; kI < k; ++kI)
    {

        int ultimoVeiculo = -1;

        //Escolhe as duas rotas
        for (int i = 0; i < 2; ++i)
        {
            int indiceInicial = rand_u32() % instancia->numVeiculos;
            rota[i] = indiceInicial;


            //Verifica se rota é não vazia e se é diferente da ultima rota escolhida
            if (solucao->vetorVeiculos[indiceInicial]->listaClientes.size() <= 2 || indiceInicial == ultimoVeiculo)
            {
                rota[i] = (rota[i] + 1) % instancia->numVeiculos;

                while (solucao->vetorVeiculos[rota[i]]->listaClientes.size() <= 2 || rota[i] == ultimoVeiculo)
                {
                    rota[i] = (rota[i] + 1) % instancia->numVeiculos;
                    if (rota[i] == indiceInicial)
                    {
                        std::cout << "Erro\nArquivo: Pertubacao.cpp\nLinha: " << __LINE__
                                  << "\nMotivo: Rotas nao encontradas, condicao impossivel\n";
                        exit(-1);
                    }
                }
            }

            vetRotaEscolhida[rota[i]] = 1;
            ultimoVeiculo = rota[i];

        }


        //Escolher os clientes das rotas
        for (int i = 0; i < 2; ++i)
        {

            Solucao::Veiculo *veiculo = solucao->vetorVeiculos[rota[i]];

            vetIterator[i] = veiculo->listaClientes.begin();

            int possicao = 1 + (rand_u32() % (veiculo->listaClientes.size() - 2));


            std::advance(vetIterator[i], possicao);


            veiculo->carga -= instancia->vetorClientes[(*vetIterator[i])->cliente].demanda;
        }

        //Troca os clientes das rotas
        Solucao::ClienteRota clienteRotaAux;
        clienteRotaAux.swap((*vetIterator[0]));
        (*vetIterator[0])->swap((*vetIterator[1]));
        (*vetIterator[1])->swap(&clienteRotaAux);

        {
            Solucao::Veiculo *veiculo = solucao->vetorVeiculos[rota[0]];



            veiculo = solucao->vetorVeiculos[rota[1]];


        }



        if (!solucao->veiculoFicticil)
        {
            solucao->veiculoFicticil = 1;
            solucao->vetorVeiculos.push_back(new Solucao::Veiculo(2));

        }

        for(int i = 0; i < 2; ++i)
        {
            Solucao::Veiculo *veiculo = solucao->vetorVeiculos[rota[i]];
            veiculo->carga += instancia->vetorClientes[(*vetIterator[i])->cliente].demanda;

        }

    }

    //Cria  as rotas
    for(int i = 0; i < instancia->numVeiculos; ++i)
    {

            if(!vetRotaEscolhida[i])
                continue;

            Solucao::Veiculo *veiculo = solucao->vetorVeiculos[i];



            int j = 0;
            for(auto cliente:veiculo->listaClientes)
            {
                vetClienteRota[j].cliente = cliente->cliente;
                ++j;
            }



            auto cliente = veiculo->listaClientes.end();
            Solucao::ClienteRota *clienteRota;


            //Funcao deleta o ultimo cliente do veiculo e atualiza a carga
            auto funcDeletaUltimoCliente = [&]
            {

                //Pega o ultimo cliente do veiculo
                cliente = veiculo->listaClientes.end();   // null
                --cliente;                                     //0 ->  0 - i ... k - (0)
                --cliente;                                     //k


                //Atualiza o peso do veiculo
                clienteRota = *cliente;
                veiculo->carga -= instancia->vetorClientes[clienteRota->cliente].demanda;

                auto itAux = cliente;
                //--itAux;



                //Apaga o cliente do veiculo
                veiculo->listaClientes.erase(itAux);
                vetClienteRota[veiculo->listaClientes.size()-1].cliente = 0;




                //Insere o cliente no veiculo ficticil
                cliente = solucao->vetorVeiculos[instancia->numVeiculos]->listaClientes.begin();
                ++cliente;
                solucao->vetorVeiculos[instancia->numVeiculos]->listaClientes.insert(cliente, clienteRota);
                solucao->vetorVeiculos[instancia->numVeiculos]->carga += instancia->vetorClientes[clienteRota->cliente].demanda;

                if(veiculo->listaClientes.size() <= 2)
                {
                    std::cout<<"Erro\nArquivo: Pertubacao.cpp\nLinha: "<<__LINE__<<"\nMotivo: Veiculo vazio. Condicao impossivel ??\n";
                }


            };

            bool aux = true;

            while(veiculo->carga > instancia->vetorVeiculos[veiculo->tipo].capacidade)
            {
                funcDeletaUltimoCliente();



                if(inviabilidadeEstatisticas)
                    inviabilidadeEstatisticas->peso += 1;

            }

            //Tenta gerar uma rota
            while(!Movimentos_Paradas::criaRota(instancia, vetClienteRota, veiculo->listaClientes.size(), veiculo->carga, veiculo->tipo, &veiculo->combustivel,
                                                &veiculo->poluicao, NULL, NULL, vetLimiteTempo, vetClienteRotaAux, inviabilidadeEstatisticas))
            {
                funcDeletaUltimoCliente();

            }



    }

    solucao->poluicao = 0.0;

    for(auto veiculoIt:solucao->vetorVeiculos)
    {
        if(veiculoIt->tipo != 2)
            solucao->poluicao += veiculoIt->poluicao;
    }


    //Verificar se o veiculo ficticil eh vazio
    if(solucao->vetorVeiculos[instancia->numVeiculos]->listaClientes.size() <= 2)
    {
        //Veiculo vazio
        delete solucao->vetorVeiculos[instancia->numVeiculos];
        solucao->vetorVeiculos.pop_back();
        solucao->veiculoFicticil = 0;

        return true;
    }
    else
    {

        solucao->poluicaoPenalidades = instancia->penalizacao.at(instancia->numClientes-1) * (solucao->vetorVeiculos[instancia->numVeiculos]->listaClientes.size() - 2);
        return false;

    }

}