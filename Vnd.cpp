//
// Created by igor on 13/04/2020.
// 2.400

//2600

// 2700

#include "Vnd.h"
#include "Construtivo.h"
#include "Movimentos_Paradas.h"
#include "mersenne-twister.h"
using namespace Vnd;


void Vnd::vnd(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao, Solucao::ClienteRota *vetClienteRotaBest, Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao,
             Solucao::ClienteRota *vetClienteRotaSecundBest, Solucao::ClienteRota *vetClienteRotaSecundAux, int interacao, EstatisticaMv *vetEstatisticaMv)
{
    
    return;

    static int vetMovimentos[8] = {0, 1, 2, 3, 4, 5, 6, 7};//, 8};
    //static int vetMovimentos[2] = {8, 5};

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


        if(vetMovimentos[posicao] != 8)
        {
            resultadosRota = Movimentos::aplicaMovimento(vetMovimentos[posicao], instancia, solucao, vetClienteRotaBest, vetClienteRotaAux, false,
                                                         vetClienteRotaSecundBest,vetClienteRotaSecundAux);

            if (resultadosRota.viavel)
            {
                if (avaliaSolucao(solucao, resultadosRota))
                {

                    atualizaEstatisticaMv(&vetEstatisticaMv[posicao], solucao, resultadosRota);
                    Movimentos::atualizaSolucao(resultadosRota, solucao, vetClienteRotaBest, vetClienteRotaSecundBest);
                    posicao = 0;
                } else
                    posicao++;
            } else
                posicao++;

        }
        else
        {
            double poluicao = solucao->poluicao;
            if(Movimentos_Paradas::mvPercorreRotaParadas(instancia, solucao, vetClienteRotaSecundBest))
            {

                vetEstatisticaMv[posicao].num += 1;
                vetEstatisticaMv[posicao].poluicao += solucao->poluicao - poluicao;
                vetEstatisticaMv[posicao].gap += 100.0 * ((solucao->poluicao - poluicao)/poluicao);
                posicao = 0;
            }
            else
                posicao++;
        }
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

