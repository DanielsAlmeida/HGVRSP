//
// Created by igor on 13/04/2020.

#include <chrono>
#include "Vnd.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
using namespace Vnd;


void Vnd::vnd(const Instancia::Instancia *const instancia, Solucao::Solucao *solucao,
              Solucao::ClienteRota *vetClienteRotaBest,
              Solucao::ClienteRota *vetClienteRotaAux, bool pertubacao, Solucao::ClienteRota *vetClienteRotaSecundBest,
              Solucao::ClienteRota *vetClienteRotaSecundAux, int interacao, EstatisticaMv *vetEstatisticaMv,
              double *vetLimiteTempo, Modelo::Modelo *modelo, HashRotas::HashRotas *hashRotas)
{



    static int vetMovimentos[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    //static int vetMovimentos[1] = {5};

    const int Num = 8;
    int inicio = 0;

    if(solucao->veiculoFicticil)
    {

        inicio = 2;
        vetMovimentos[0] = 2;
        vetMovimentos[1] = 3;
    }

    for(int i = inicio; i < Num; ++i)
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

            auto c_start = std::chrono::high_resolution_clock::now();



            resultadosRota = Movimentos::aplicaMovimento(vetMovimentos[posicao], instancia, solucao, vetClienteRotaBest,
                                                         vetClienteRotaAux, false, vetClienteRotaSecundBest,
                                                         vetClienteRotaSecundAux, vetLimiteTempo, modelo, hashRotas);


            auto c_end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli>  tempoCpu = (c_end - c_start);

            vetEstatisticaMv[vetMovimentos[posicao]].numTempo += 1;
            vetEstatisticaMv[vetMovimentos[posicao]].tempo += tempoCpu.count();

            if (resultadosRota.viavel)
            {
                if (avaliaSolucao(solucao, resultadosRota))
                {
                    if(!solucao->veiculoFicticil)
                        atualizaEstatisticaMv(&vetEstatisticaMv[vetMovimentos[posicao]], solucao, resultadosRota);

                    Movimentos::atualizaSolucao(resultadosRota, solucao, vetClienteRotaBest, vetClienteRotaSecundBest, instancia, vetMovimentos[posicao]);
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
    {
        if(resultadosRota.veiculoSecundario->tipo != 2)
            return (resultadosRota.poluicao + resultadosRota.poluicaoSecundario) < (resultadosRota.veiculo->poluicao + resultadosRota.veiculoSecundario->poluicao);
        else
            return true;
    }

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

