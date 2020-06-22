//
// Created by igor on 13/04/2020.

#include "Vnd.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
using namespace Vnd;


void Vnd::vnd(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
              Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao,
              Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux,
              int interacao, EstatisticaMv *vetEstatisticaMv, double *vetLimiteTempo)
{

    return;

    static int vetMovimentos[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    //static int vetMovimentos[1] = {7};

    const int Num = 8;

    for(int i = 0; i < Num; ++i)
    {
        int mv = rand_u32() % Num;

        for(int j = 0; j < i; )
        {
            if(vetMovimentos[j] == mv)
            {
                mv = (mv+1) % Num;
                j = 0;
                continue;
            }

            ++j;
        }

        vetMovimentos[i] = mv;
    }


    Movimentos::ResultadosRota resultadosRota;

    int posicao = 0;

    while(posicao < Num)
    {

            resultadosRota = Movimentos::aplicaMovimento(vetMovimentos[posicao], instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, false, vetClienteRotaSecundBest,
                    vetClienteRotaSecundAux, vetLimiteTempo);

            if (resultadosRota.viavel)
            {
                if (avaliaSolucao(solucao, resultadosRota))
                {

                    atualizaEstatisticaMv(&vetEstatisticaMv[vetMovimentos[posicao]], solucao, resultadosRota);
                    Movimentos::atualizaSolucao(resultadosRota, solucao, vetClienteRotaBest, vetClienteRotaSecundBest);
                    posicao = 0;
                } else
                    posicao++;
            } else
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

void Vnd::atualizaEstatisticaMv(EstatisticaMv *estatisticaMv, Solucao::Solucao *solucao, Movimentos::ResultadosRota resultadosRota)
{

    double poluicao = solucao->poluicao;
    poluicao -= resultadosRota.veiculo->poluicao;
    poluicao += resultadosRota.poluicao;

    if(resultadosRota.veiculoSecundario)
    {

        poluicao -= resultadosRota.veiculoSecundario->poluicao;
        poluicao += resultadosRota.poluicaoSecundario;

    }

    estatisticaMv->num += 1;
    estatisticaMv->poluicao += poluicao - solucao->poluicao;
    estatisticaMv->gap += 100.0 * ((poluicao - solucao->poluicao)/solucao->poluicao);
}

