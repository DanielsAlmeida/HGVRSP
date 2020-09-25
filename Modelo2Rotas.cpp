//
// Created by igor on 25/09/2020.
//

#include "Modelo2Rotas.h"
#include "mersenne-twister.h"

using namespace Modelo2Rotas;

void Modelo2Rotas::geraRotas_comb_2Rotas(Solucao::Solucao *solucao, Modelo::Modelo *modelo, Solucao::ClienteRota *vetClienteRota, Solucao::ClienteRota *vetClienteRota2,
                                         const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas, int *vetRotasAux, bool **matRotas)
{

    //Zera a matriz matRotas
    for(int i = 0; i < instancia->numVeiculos; ++i)
    {
        for(int j = 0; j < instancia->numVeiculos; ++j)
            matRotas[i][j] = 0;


    }

    bool existePares = true;
    int indice0, indice1;
    double combustivel0, combustivel1;
    double poluico0, poluicao1;
    int peso0, peso1;

    while(existePares)
    {
        existePares = false;

        indice0 = -1;
        indice1 = -2;

        //Escolhe duas rotas
        for(int k = 0; k < 2; ++k)
        {

            //Escolhe um indice
            int indice = rand_u32() % instancia->numVeiculos;

            //Verifica a rota eh maior que 2 e diferente de indice0
            while((solucao->vetorVeiculos[indice]->listaClientes.size() <= 2) || ((indice0 == indice) && (k == 1)))
                indice = (indice + 1) % instancia->numVeiculos;

            int indiceOrig = indice;

            do
            {
                bool rotaUsada = false;

                //Percorre todos os veiculos
                for(int j = 0; j < instancia->numVeiculos; ++j)
                {

                    //Verifica se
                    if(j != indice)
                    {
                        if(matRotas[j][indice])
                        {
                            rotaUsada = true;
                            break;
                        }
                    }
                }

                if(rotaUsada)
                    indice = (indice + 1) % instancia->numVeiculos;
                else
                    break;

            }while (indice != indiceOrig);

            if(indice == indiceOrig)
            {
                break;
            }
            else
            {
                if(k == 0)
                    indice0 = indice;
                else
                    indice1 = indice;
            }


        }




    }

}
