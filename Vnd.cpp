//
// Created by igor on 13/04/2020.
// 2.400

//2600

// 2700

#include "Vnd.h"
#include "mersenne-twister.h"
using namespace Vnd;


void Vnd::vnd(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest,
         Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao, Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux)
{

    static int vetMovimentos[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    for(int i = 0; i < 8; ++i)
    {
        int mv = rand_u32() % 8;

        for(int j = 0; j < i; )
        {
            if(vetMovimentos[j] == mv)
            {
                mv = (mv+1) % 8;
                j = 0;
                continue;
            }

            ++j;
        }

        vetMovimentos[i] = mv;

    }


    Movimentos::ResultadosRota resultadosRota;

    int posicao = 0;

    while(posicao < 8)
    {
        //cout<<"VND\n";
        resultadosRota = Movimentos::aplicaMovimento(vetMovimentos[posicao], instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, false, vetClienteRotaSecundBest, vetClienteRotaSecundAux);

        if(resultadosRota.viavel)
        {
            if(avaliaSolucao(solucao, resultadosRota))
            {

                Movimentos::atualizaSolucao(resultadosRota, solucao, vetClienteRotaBest, vetClienteRotaSecundBest);
                posicao = 0;
            }
            else
                posicao++;
        }
        else
            posicao++;
    }

}

bool Vnd::avaliaSolucao(Solucao::Solucao *solucao, Movimentos::ResultadosRota resultadosRota)
{
    if(resultadosRota.veiculoSecundario)
        return (resultadosRota.poluicao + resultadosRota.poluicaoSecundario) < (resultadosRota.veiculo->poluicao + resultadosRota.veiculoSecundario->poluicao);

    else
        return (resultadosRota.poluicao) < (resultadosRota.veiculo->poluicao);
}

