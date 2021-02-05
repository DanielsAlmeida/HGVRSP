//
// Created by igor on 30/11/2020.
//

#ifndef HGVRSP_ESTATISTICASQUALIDADE_H
#define HGVRSP_ESTATISTICASQUALIDADE_H

#include <cmath>
#include <chrono>

using namespace std;
using namespace chrono;

namespace EstatisticasQualidadeN
{
    class EstatisticasQualidade
    {
    public:

        double poluicao;
        double tempo;
        u_int64_t interacao;
        double poluicaoCorrente = -1.0;
        bool mip;

        EstatisticasQualidade(double p, u_int64_t i, bool m, high_resolution_clock::time_point c_start, double p2, bool viavel)
        {
            poluicao = p;

            if(viavel)
                poluicaoCorrente = p2;
            else
                poluicaoCorrente = 0;

            interacao = i;

            auto end = high_resolution_clock::now();
            static std::chrono::duration<double> tempoCpu;
            tempoCpu = (end - c_start);

//            cout<<c_start.time_since_epoch().count() <<"\t" <<end.time_since_epoch().count()<<"\t"<<tempoCpu.count()<<"\n";

            tempo = tempoCpu.count();

            mip = m;
        }

        EstatisticasQualidade(double p, u_int64_t i, bool m, high_resolution_clock::time_point c_start)
        {
            poluicao = p;
            interacao = i;

            auto end = high_resolution_clock::now();
            static std::chrono::duration<double> tempoCpu;
            tempoCpu = (end - c_start);

//            cout<<c_start.time_since_epoch().count() <<"\t" <<end.time_since_epoch().count()<<"\t"<<tempoCpu.count()<<"\n";

            tempo = tempoCpu.count();

            mip = m;
        }

        EstatisticasQualidade(){}
    };



}

#endif //HGVRSP_ESTATISTICASQUALIDADE_H
