//
// Created by igor on 08/01/2021.
//

#include "Pertubacao.h"
#include "mersenne-twister.h"

using namespace Pertubacao;

/**
 *
 * A pertubação ira escolher 2 rotas distintas aleatoriamente e para cada rota escolhe um cliente e realiza a troca entre as rotas.
 * A viabilidade não inpede o movimento de ocorrer. A troca é realizada k vezes, com k > 1.
 *
 * Para viabilizar a rota são retirados os ultimos clientes da rota ate obter uma rota viavel. Os clientes que foram retirados são incluidos no veiculo victicil,
 * o vnd ira tentar viabilizar a solucao
 *
 * @param solucao Solucao a ser pertubada
 * @param k Numero de vezes que o swap sera realizado
 * @return  retorna true se conseguil realizar a pertubacao
 */

bool Pertubacao::pertubacao_k_swap( Instancia::Instancia *instancia, Solucao::Solucao *solucao, const int k)
{

    bool vetRotaEscolhida[instancia->numVeiculos];

    for(int i = 0; i < instancia->numVeiculos; ++i)
        vetRotaEscolhida[i] = 0;

    int rota[2];
    rota[0] = rota[1] = -1;

    //Realiza o swap k vezes
    for(int kI; kI < k; ++kI)
    {

        int ultimoVeiculo = -1;

        //Escolhe as duas rotas
        for(int i = 0; i < 2; ++i)
        {
            int indiceInicial = rand_u32() % instancia->numVeiculos;
            rota[i] = indiceInicial;


            //Verifica se rota é não vazia e se é diferente da ultima rota escolhida
            if(solucao->vetorVeiculos[indiceInicial]->listaClientes.size() <= 2 || indiceInicial == ultimoVeiculo)
            {
                rota[i] = (rota[i] + 1) % instancia->numVeiculos;

                while(solucao->vetorVeiculos[rota[i]]->listaClientes.size() <= 2 || rota[i] == ultimoVeiculo)
                {
                    rota[i] = (rota[i] + 1) % instancia->numVeiculos;
                    if(rota[i] == indiceInicial)
                    {
                        std::cout<<"Erro\nArquivo: Pertubacao.cpp\nLinha: "<<__LINE__<<"\nMotivo: Rotas nao encontradas, condicao impossivel\n";
                        exit(-1);
                    }
                }
            }

            ultimoVeiculo = rota[i];

        }

        //Escolher os clientes das rotas
        for(int i = 0; i < 2; ++i)
        {

        }

    }

}