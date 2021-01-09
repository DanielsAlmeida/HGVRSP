//
// Created by igor on 20/10/2020.
//

#include "Ils.h"
#include "Modelo2Rotas.h"
#include "Constantes.h"
#include "Construtivo.h"
#include "mersenne-twister.h"
using namespace Ils;
#include "Alvo.h"

#define NumInteracoes 1000
#define Debug false

void Ils::ils(const Instancia::Instancia *const instancia, Solucao::Solucao **solucao, const u_int64_t numInteracoesMax,
              const u_int64_t numInteracoesMaxSemMelhora, const double tempoLimite, const int opcao,
              Solucao::ClienteRota **vetVetorClienteRota, HashRotas::HashRotas *hashRotas, int **vetGuardaRota,
              Vnd::EstatisticaMv *vetEstatistica, double *vetLimiteTempo, int **matRotas,
              Modelo_1_rota::Modelo *modelo1Rota,
              Modelo::Modelo *modelo, double *tempoModelo2Rotas, u_int64_t *interacoesIls,
              u_int64_t *ultimaAtualizacaoIls,
              Construtivo::Candidato *vetorCandidatos, double *vetorParametors,
              Solucao::ClienteRota **matrixClienteBest,
              Movimentos_Paradas::TempoCriaRota *tempoCriaRota, Construtivo::GuardaCandInteracoes *vetCandInteracoes,
              const double alvo, Alvo::Alvo *alvoTempo, list<EstatisticasQualidade> &listaEstQual)
{


    if((*solucao)->veiculoFicticil && !vetorCandidatos)
    {

        cout<<"Ils recebeu uma solucao invialvel ou um ponteiro NULL !!\n";

        exit(-1);

    }



    if(!(*solucao)->veiculoFicticil)
    {   double poluicao = (*solucao)->poluicao;

        if(alvoTempo)
            alvoTempo->novaSolucao(poluicao);
    }



    if(alvoTempo && (alvoTempo->antingilTodosAlvos()))
    {

        return;

    }

    #if Debug
    cout<<"ILS\n";
    #endif


    Solucao::Solucao *solucaoCorrente = new Solucao::Solucao(*solucao);

    static const float vetAlfas[9] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    *interacoesIls = 0;
    *ultimaAtualizacaoIls = -1;

    auto tempoStartIls = std::chrono::high_resolution_clock::now();
    auto tempoEndIls   = std::chrono::high_resolution_clock::now();

    auto timeIls = std::chrono::duration_cast<std::chrono::seconds>(tempoEndIls - tempoStartIls);

    double poluicaoBestHeuristica;

    if(vetorCandidatos)
        poluicaoBestHeuristica = (*solucao)->poluicao;
    else
        poluicaoBestHeuristica = calculaSolucaoHeuristica(instancia, *solucao,hashRotas, vetVetorClienteRota[0], vetVetorClienteRota[1], vetLimiteTempo);


    int numChamadasMip = 0;
    *ultimaAtualizacaoIls = 0;

    list<Solucao::Solucao*> listaSolucao;


    while(((alvoTempo&&!alvoTempo->antingilTodosAlvos()) || alvoTempo==NULL ) && (timeIls.count() <= tempoLimite) && (( (*interacoesIls > 1400) &&
         (*interacoesIls - *ultimaAtualizacaoIls) < numInteracoesMaxSemMelhora) || (*interacoesIls <= 1400)) && (*interacoesIls < numInteracoesMax))
    {


        if(*interacoesIls == NumInteracoes && !(*solucao)->veiculoFicticil && opcao == OpcaoIlsMip)
        {

            double poluicao = (*solucao)->poluicao;

            Modelo_1_rota::geraRotasOtimas(*solucao, modelo1Rota, vetVetorClienteRota[0], instancia, hashRotas,vetGuardaRota[0]);


            auto start = std::chrono::high_resolution_clock::now();

            Modelo2Rotas::geraRotas_comb_2Rotas(*solucao, modelo, vetVetorClienteRota[0], vetVetorClienteRota[1],
                                                instancia, hashRotas, vetGuardaRota[0], matRotas, vetGuardaRota[1]);


            auto end = std::chrono::high_resolution_clock::now();

            auto tempo_ = std::chrono::duration_cast<std::chrono::seconds>(end - start);
            *tempoModelo2Rotas += tempo_.count();

            if((*solucao)->poluicao < poluicao)
            {
                *ultimaAtualizacaoIls = *interacoesIls;
                listaEstQual.push_back(EstatisticasQualidade((*solucao)->poluicao, *interacoesIls, true, tempoStartIls, (*solucao)->poluicao));

            }

            if(alvoTempo && !((*solucao)->veiculoFicticil))
                alvoTempo->novaSolucao((*solucao)->poluicao);

#           if Debug
            cout<<" MIP best 1000\n";

#           endif
        }



        //Receta a solucao
        if(((opcao == OpcaoIlsMip && *interacoesIls > NumInteracoes && (*interacoesIls - *ultimaAtualizacaoIls) == 700) ||
           ((opcao == OpcaoGraspIlsMip || opcao == OpcaoGraspComIlsMip) && (*interacoesIls - *ultimaAtualizacaoIls) == 150)) && !(*solucao)->veiculoFicticil)
        {


            delete solucaoCorrente;
            solucaoCorrente = new Solucao::Solucao(*solucao);

#if         Debug
            cout<<"Recetando a solucao\n";

#endif

        }


        if((solucaoCorrente->veiculoFicticil) && vetorCandidatos)
        {
            delete solucaoCorrente;
            solucaoCorrente = Construtivo::geraSolucao(instancia, vetAlfas[rand_u32()%5], vetVetorClienteRota[0],
                                                       vetVetorClienteRota[1], nullptr, false, vetorCandidatos,
                                                       {2,1}, vetorParametors, matrixClienteBest, tempoCriaRota, vetCandInteracoes, vetLimiteTempo);

        }

        for(auto veiculo : solucaoCorrente->vetorVeiculos)
            (*veiculo->listaClientes.begin())->rotaMip = false;


        Movimentos::ResultadosRota resultadosRota =  Movimentos::mv_2optSwapInterRotas(instancia, solucaoCorrente, vetVetorClienteRota[0] , vetVetorClienteRota[1],
                                                                                       vetVetorClienteRota[2], vetVetorClienteRota[3], true, true, vetLimiteTempo,
                                                                                       NULL, NULL, vetGuardaRota[0], vetGuardaRota[1]); //5



/*
        Movimentos::ResultadosRota resultadosRota = Movimentos::mvInterRotasShift(instancia, solucaoCorrente, vetVetorClienteRota[0] , vetVetorClienteRota[1],
                                                                                  vetVetorClienteRota[2], true, true, vetLimiteTempo,
                                                                                  modelo, hashRotas, vetGuardaRota[0], vetGuardaRota[1]);
*/
        if(resultadosRota.viavel)
        {
           // cout<<" v \n";
            Movimentos::atualizaSolucao(resultadosRota, solucaoCorrente, vetVetorClienteRota[0], vetVetorClienteRota[2], instancia, -1);

        }

        //fase de busca local rvnd

        //if((*interacoesIls - *ultimaAtualizacaoIls) != numInteracoesMaxSemMelhora-2)
        {

            Vnd::vnd(instancia, solucaoCorrente, vetVetorClienteRota[0], vetVetorClienteRota[1], false,
                     vetVetorClienteRota[2], vetVetorClienteRota[3], 0,
                     vetEstatistica, vetLimiteTempo, NULL, hashRotas, vetGuardaRota[0], vetGuardaRota[1], 5);

        }



        //MIP uma rota

        bool chamadaMip = false;
        bool deletaSolucaoCorrente = true;

        //if((*interacoesIls >= 100) && (opcao == OpcaoGraspIlsMip || opcao == OpcaoIlsMip || opcao == OpcaoGraspComIlsMip) && !solucaoCorrente->veiculoFicticil)
        if((((opcao == OpcaoIlsMip && *interacoesIls > NumInteracoes && (*interacoesIls - *ultimaAtualizacaoIls) >= 300) || ((opcao == OpcaoGraspIlsMip || opcao == OpcaoGraspComIlsMip) &&
                numChamadasMip < numInteracoesMaxSemMelhora && (*interacoesIls - *ultimaAtualizacaoIls) >= 50)) || (solucaoCorrente->poluicao < (*solucao)->poluicao &&  *interacoesIls > NumInteracoes)) && !solucaoCorrente->veiculoFicticil && opcao == OpcaoIlsMip)
        {

            static bool mip = false;






            double  poluicaoHeuriAux = calculaSolucaoHeuristica(instancia, solucaoCorrente,hashRotas, vetVetorClienteRota[0], vetVetorClienteRota[1], vetLimiteTempo);

            double gap = (solucaoCorrente->poluicao - (*solucao)->poluicao)/(*solucao)->poluicao;

            if(gap <= 0.15)
            {


                #if Debug

               // cout<<"MIP i "<<*interacoesIls<<" gap <= 15 \n";


                #endif

                const static int p = 100 - 40;


                int valA = rand_u32() % 100;

                if ((p <= valA) || ((*solucao)->veiculoFicticil) ||
                    (gap *100 <= -0.1))
                {

                    if (gap < -0.01)
                    {
                        ++numChamadasMip;
                        Modelo_1_rota::geraRotasOtimas(solucaoCorrente, modelo1Rota, vetVetorClienteRota[0], instancia,
                                                       hashRotas, vetGuardaRota[0]);


                        auto start = std::chrono::high_resolution_clock::now();

                        Modelo2Rotas::geraRotas_comb_2Rotas(solucaoCorrente, modelo, vetVetorClienteRota[0],
                                                            vetVetorClienteRota[1],
                                                            instancia, hashRotas, vetGuardaRota[0], matRotas,
                                                            vetGuardaRota[1]);


                        auto end = std::chrono::high_resolution_clock::now();

                        auto tempo_ = std::chrono::duration_cast<std::chrono::seconds>(end - start);
                        *tempoModelo2Rotas += tempo_.count();
                        chamadaMip = true;

                        if (!listaSolucao.empty())
                        {
                            for (auto it:listaSolucao)
                                delete it;

                            listaSolucao.clear();
                        }
                    } else
                    {
                        deletaSolucaoCorrente = false;
                        listaSolucao.push_back(solucaoCorrente);
                    }

                    mip = false;
                    #if Debug

                    //cout<<"MIP i "<<*interacoesIls<<"\n";

                    if(solucaoCorrente->poluicao - (*solucao)->poluicao < -0.001 || poluicaoHeuriAux - poluicaoBestHeuristica  < -0.001)
                    {
                        //cout<<"Atualizacao Entrou MIP\n";
                    }

                    #endif


                }
            }

            if(!chamadaMip && ((*interacoesIls - *ultimaAtualizacaoIls) % 200  == 0) && !listaSolucao.empty() && (*interacoesIls - *ultimaAtualizacaoIls)>300)
            {
                    deletaSolucaoCorrente = true;

                    auto itBest = listaSolucao.begin();

                    int tam =  ceil(0.1*listaSolucao.size());

                    int escolhido  = rand_u32() % tam;



                    listaSolucao.sort(Solucao::Solucao::compare);

                    //cout<<"Bloco, gap das solucoes: \n";

                    //for(auto it:listaSolucao)
                    //    cout<<(it->poluicao - (*solucao)->poluicao)/(*solucao)->poluicao * 100.0<<" ";

                    //cout<<" \n\n ";

                    auto end = listaSolucao.end();
                    --end;



                    int i  = 0;
                    for(itBest = listaSolucao.begin(); i != escolhido; ++itBest,++i);

                    itBest = listaSolucao.begin();




//                    listaSolucao.remove(*itBest);
                    auto ptrAux = *itBest;
                    listaSolucao.erase(itBest);



                    for(auto it:listaSolucao)
                    {
                        if(it == solucaoCorrente)
                            solucaoCorrente = NULL;

                        delete it;
                    }

                    if(solucaoCorrente)
                        delete solucaoCorrente;

                    solucaoCorrente = ptrAux;
                    listaSolucao.clear();

                    ++numChamadasMip;

                    Modelo_1_rota::geraRotasOtimas(solucaoCorrente, modelo1Rota, vetVetorClienteRota[0], instancia, hashRotas,vetGuardaRota[0]);



                    auto start = std::chrono::high_resolution_clock::now();

                    Modelo2Rotas::geraRotas_comb_2Rotas(solucaoCorrente, modelo, vetVetorClienteRota[0], vetVetorClienteRota[1],
                                                        instancia, hashRotas, vetGuardaRota[0], matRotas, vetGuardaRota[1]);



                    auto endTime = std::chrono::high_resolution_clock::now();

                    auto tempo_ = std::chrono::duration_cast<std::chrono::seconds>(endTime - start);
                    *tempoModelo2Rotas += tempo_.count();





            }


        }

        if((((solucaoCorrente->poluicao - (*solucao)->poluicao)/(*solucao)->poluicao)*100  < -0.1) || ((*solucao)->veiculoFicticil == true && !solucaoCorrente->veiculoFicticil))
        {



            listaSolucao.clear();


            delete *solucao;
            *solucao = new Solucao::Solucao(solucaoCorrente);


            *ultimaAtualizacaoIls = *interacoesIls;


            if(alvoTempo&&!((*solucao)->veiculoFicticil))
                alvoTempo->novaSolucao((*solucao)->poluicao);

            listaEstQual.push_back(EstatisticasQualidade((*solucao)->poluicao, *interacoesIls, chamadaMip, tempoStartIls, solucaoCorrente->poluicao));

#           if Debug

            cout<<"Atualizacao interacao : "<<*ultimaAtualizacaoIls<<'\n';

#           endif

        }
        else
        {   

            listaEstQual.push_back(EstatisticasQualidade((*solucao)->poluicao, *interacoesIls, chamadaMip, tempoStartIls, solucaoCorrente->poluicao));

            if(!deletaSolucaoCorrente)
                solucaoCorrente = new Solucao::Solucao(solucaoCorrente);
        }

        tempoEndIls   = std::chrono::high_resolution_clock::now();
        timeIls = std::chrono::duration_cast<std::chrono::seconds>(tempoEndIls - tempoStartIls);


        if((interacoesIls - ultimaAtualizacaoIls) == 50 )
        {
            delete solucaoCorrente;
            solucaoCorrente = new Solucao::Solucao(*solucao);

        }


        ++(*interacoesIls);

    }

    if(!listaSolucao.empty())
    {
        for(auto it:listaSolucao)
            delete it;


        listaSolucao.clear();
    }

    if(opcao==OpcaoGraspComIlsMip && numChamadasMip == numInteracoesMaxSemMelhora && ((alvoTempo && !alvoTempo->antingilTodosAlvos()) || alvoTempo == NULL))
    {




        Modelo_1_rota::geraRotasOtimas(solucaoCorrente, modelo1Rota, vetVetorClienteRota[0], instancia, hashRotas,vetGuardaRota[0]);


        auto start = std::chrono::high_resolution_clock::now();

        Modelo2Rotas::geraRotas_comb_2Rotas(solucaoCorrente, modelo, vetVetorClienteRota[0], vetVetorClienteRota[1],
                                            instancia, hashRotas, vetGuardaRota[0], matRotas, vetGuardaRota[1]);


        auto end = std::chrono::high_resolution_clock::now();

        auto tempo_ = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        *tempoModelo2Rotas += tempo_.count();

        if(solucaoCorrente->poluicao - (*solucao)->poluicao  < -0.001)
        {
            delete *solucao;
            *solucao = solucaoCorrente;
            solucaoCorrente = NULL;

            listaEstQual.push_back(EstatisticasQualidade((*solucao)->poluicao, *interacoesIls, true, tempoStartIls));
        }

    }


    delete solucaoCorrente;

    (*solucao)->ultimaAtualizacao = *ultimaAtualizacaoIls;
}
double Ils::calculaSolucaoHeuristica(const Instancia::Instancia *instancia, Solucao::Solucao *solucao, HashRotas::HashRotas *hashRotas,
                                     Solucao::ClienteRota *vetorClienteBest, Solucao::ClienteRota *vetorAux, double *vetorLimiteTempo)
{

    return solucao->poluicao;

    double poluicaoHeuriAux = 0.0;
    HashRotas::HashNo *hashNo;
    double aux, aux2;

    for (auto veiclulo : solucao->vetorVeiculos)
    {





        {
            if(veiclulo->listaClientes.size() <= 2)
            {
                continue;
            }

            //Recupera o veiculo na hash
            hashNo = hashRotas->getVeiculo(veiclulo);

            if (!hashNo)
            {
                poluicaoHeuriAux += veiclulo->poluicao;
            }
            else
            {

                if (hashNo->poluicaoH > 0.0)
                    poluicaoHeuriAux += hashNo->poluicaoH;
                else
                {
                    //Copia a rota para vetor

                    Solucao::ClienteRota *clienteRota_ptr = vetorClienteBest;

                    for (auto cliente : veiclulo->listaClientes)
                    {
                        clienteRota_ptr->cliente = cliente->cliente;

                        ++clienteRota_ptr;
                    }

                    bool resultado = Movimentos_Paradas::criaRota(instancia, vetorClienteBest,
                                                                  veiclulo->listaClientes.size(),
                                                                  veiclulo->carga, veiclulo->tipo, &aux,
                                                                  &aux2, NULL, NULL, vetorLimiteTempo,
                                                                  vetorAux);

                    if (!resultado)
                    {

                        hashNo->poluicaoH = veiclulo->poluicao + 0.15 * veiclulo->poluicao;

                    } else
                        hashNo->poluicaoH = aux2;

                    poluicaoHeuriAux += hashNo->poluicaoH;
                }

            }
        }

    }

    return poluicaoHeuriAux;

}
