//
// Created by igor on 01/11/2020.
//

#ifndef HGVRSP_ALVO_H
#define HGVRSP_ALVO_H
#include "Instancia.h"
#define NumAlvos 3

namespace Alvo
{

    class Alvo
    {

    public:

        const int n;
        double *vetAlvos;
        double *vetTime;
        const Instancia::TimeType start;

        Alvo(double alvo, Instancia::TimeType _start, int n) : start(_start), n(n)
        {

            if(!(n <=3 && n>=0))
            {
                cout<<"n errado na classe Alvo\n";
                exit(-1);
            }

            vetTime = new double[n];
            vetAlvos = new double[n];

            double p = 0.3;
            int i = 0;

            for(; (p >= 0.0 && i < n); p-=0.15, ++i)
            {
                vetAlvos[i] = alvo + p*alvo;
                vetTime[i] = -1.0;

                //cout<<"Alvo: "<<vetAlvos[i]<<" Time: "<<vetTime[i]<<'\n';

            }




        }

        bool antingilTodosAlvos()
        {
            bool alvos = true;

            for(int i = 0; (i < n)&&alvos; ++i)
            {
                alvos = alvos && (vetTime[i] > 0);
            }

            return alvos;
        }



        void novaSolucao(double poluicao)
        {

            int atualizacao = 0;

            for(int i = 0; i < n; ++i)
            {

                if(vetTime[i] == -1)
                {
                    if(poluicao <= vetAlvos[i])
                    {
                        atualizacao = 1;
                        break;
                    }
                }
            }


            if(atualizacao)
            {
                Instancia::TimeType end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> time = (end - start);


                for(int i = 0; i < n; ++i)
                {
                    if(vetTime[i] == -1)
                    {
                        if(poluicao <= vetAlvos[i])
                        {
                            vetTime[i] = time.count();
                        }

                    }
                }
            }


        }
    };

}


#endif //HGVRSP_ALVO_H
