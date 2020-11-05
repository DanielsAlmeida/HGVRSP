//
// Created by igor on 25/09/2020.
//

#include "Modelo2Rotas.h"
#include "mersenne-twister.h"

using namespace Modelo2Rotas;

void Modelo2Rotas::geraRotas_comb_2Rotas(Solucao::Solucao *solucao, Modelo::Modelo *modelo, Solucao::ClienteRota *vetClienteRota, Solucao::ClienteRota *vetClienteRota2,
                                         const Instancia::Instancia *const instancia, HashRotas::HashRotas *hashRotas, int *vetRotasAux, int **matRotas, int *vetRotasAux2)
{

    //Zera a matriz matRotas
    for(int i = 0; i < instancia->numVeiculos; ++i)
    {
        for(int j = 0; j < instancia->numVeiculos; ++j)
            matRotas[i][j] = 0;


    }

    const int NumMaximoRotasGrupo = ((instancia->numVeiculos) * (instancia->numVeiculos - 1))/2;
    const int  NumMaximoRotasMip =  (10 < NumMaximoRotasGrupo) ? 10 : NumMaximoRotasGrupo;


    //Variaveis
    bool existePares = true;
    int indice0, indice1;
    double combustivel0, combustivel1;
    double poluico0, poluicao1;
    int peso0, peso1, tam0, tam1;
    int tipo0, tipo1;
    double poluicaoAntes;
    double gap;
    Solucao::Veiculo *ptr_veiculo = NULL;
    int tam;
    int numRotas = 0;

    while(existePares && numRotas < NumMaximoRotasMip)
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
            while((solucao->vetorVeiculos[indice]->listaClientes.size() <= 2) || ((k == 1) && ((indice0 == indice) || (matRotas[indice][indice0]))))
                indice = (indice + 1) % instancia->numVeiculos;



            int indiceOrig = indice;

            do
            {

                bool rotaUsada = true;

                if((solucao->vetorVeiculos[indice]->listaClientes.size() > 2))
                {

                    bool condicao = true;

                    if(indice0 >= 0)
                        condicao = !((matRotas[indice0][indice])&&(k==1));

                    if(condicao || (k == 0))
                    {

                        if(k == 0)
                        {
                            //Percorre todos os veiculos
                            for (int j = 0; j < instancia->numVeiculos; ++j)
                            {

                                //Verifica se j eh diferente de indice
                                if (j != indice)
                                {
                                    if (matRotas[indice][j] == 0)
                                    {
                                        rotaUsada = false;
                                        //indice0 = indice1 = -1;
                                        break;
                                    }
                                }
                            }
                        }
                        else
                            rotaUsada = false;
                    }



                    //Verifica se a rota ja foi usada
                    if (rotaUsada)
                        indice = (indice + 1) % instancia->numVeiculos;
                    else
                        break;
                }
                else
                    indice = (indice + 1) % instancia->numVeiculos;

                if(!rotaUsada)
                    break;


            }while (indice != indiceOrig);



            //Percorreu todas as rotas
            if(indice == -1)
            {
                indice0 = indice1 = -1;
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



        //Verifica se encontrou as rotas
        if(indice0 < 0 || indice1 < 0)
            break;



        existePares = true;

        //Copia as rotas para os vetores e chama o mip para duas rotas

        int indice = indice0;
        Solucao::ClienteRota *ptr_clienteRota = vetClienteRota;

        for(int i = 0; i < 2; ++i)
        {



            for(auto itRota : solucao->vetorVeiculos[indice]->listaClientes)
            {
                ptr_clienteRota->swap(itRota);
                ++ptr_clienteRota;

            }

            indice = indice1;
            ptr_clienteRota = vetClienteRota2;

        }

        ptr_clienteRota = NULL;

        //Quarda tipo, tam, poluicao, combustivel e peso das rotas

        tipo0 = solucao->vetorVeiculos[indice0]->tipo;
        tam0 = solucao->vetorVeiculos[indice0]->listaClientes.size();
        poluico0 =  solucao->vetorVeiculos[indice0]->poluicao;
        combustivel0 = solucao->vetorVeiculos[indice0]->combustivel;
        peso0 = solucao->vetorVeiculos[indice0]->carga;

        tipo1 = solucao->vetorVeiculos[indice1]->tipo;
        tam1 = solucao->vetorVeiculos[indice1]->listaClientes.size();
        poluicao1 =  solucao->vetorVeiculos[indice1]->poluicao;
        combustivel1 = solucao->vetorVeiculos[indice1]->combustivel;
        peso1 = solucao->vetorVeiculos[indice1]->carga;

        poluicaoAntes = poluico0 + poluicao1;

        try
        {

            //Otimza as duas rotas
            modelo->criaRota(vetClienteRota, &tam0, tipo0, &peso0, instancia, &poluico0, &combustivel0, 3, vetRotasAux,
                             vetClienteRota2, &tam1, tipo1, &peso1, &poluicao1, &combustivel1, vetRotasAux2, true);

        }
        catch (GRBException e)
        {
            std::cout<<"code: "<<e.getErrorCode()<<"\nMessage: "<<e.getMessage()<<"\n";
            std::cout<<"Rota1: "<<solucao->vetorVeiculos[indice0]->getRota()<<"\nRota2: "<<solucao->vetorVeiculos[indice1]->getRota()<<"\n\n";

            std::cout<<"vet1: ";
            for(int i = 0; i < tam0; ++i)
                std::cout<<vetClienteRota[i].cliente<<' ';


            std::cout<<"\nvet2: ";
            for(int i = 0; i < tam1; ++i)
                std::cout<<vetClienteRota2[i].cliente<<' ';

            std::cout<<"\n\n";

            exit(-1);

        }

        if(vetClienteRota[tam0-1].cliente != 0 || vetClienteRota[0].cliente != 0 || vetClienteRota2[tam1-1].cliente != 0 || vetClienteRota2[0].cliente != 0)
        {
            std::cout<<"Erro arquivo: Modelo2Rotas.cpp \nLinha"<<__LINE__<<"\n\n";
            std::cout<<"Motivo: rotas nao comecao ou terminao no deposito\n\n";
            exit(-1);
        }


        gap = (((poluico0+poluicao1)-poluicaoAntes)/poluicaoAntes) * 100.0;

        existePares = true;

        //Verifica se  o gap é significativo
        if(gap > -1e-4)
        {
            //if(matRotas[indice0][indice1] == 2 || matRotas[indice0][indice1]==2)
              //  existePares = false;
            matRotas[indice0][indice1] = 2;
            matRotas[indice1][indice0] = 2;

            //std::cout<<"nao ouve melhora. Par: "<<indice0<<' '<<indice1<<'\n';
        }
        else
        {



            indice = indice0;

            for(int k = 0; k < 2; ++k)
            {
                for (int j = 0; j < instancia->numVeiculos; ++j)
                {
                    if(j != indice)
                        matRotas[j][indice] = 0;

                }

                for(int j = 0; j < instancia->numVeiculos; ++j)
                {
                    if(j != indice)
                        matRotas[indice][j] = 0;
                }

                if(k == 0)
                    indice = indice1;

            }

            matRotas[indice0][indice1] = 1;
            matRotas[indice1][indice0] = 1;

            //std::cout<<"Melhora. Par: "<<indice0<<' '<<indice1<<'\n';

            //Escrever as rotas na solucao

            hashRotas->insereVeiculo(vetClienteRota, poluico0, combustivel0, tam0, solucao->vetorVeiculos[indice0]->tipo, peso0);
            hashRotas->insereVeiculo(vetClienteRota2, poluicao1, combustivel1, tam1, solucao->vetorVeiculos[indice1]->tipo, peso1);

            for (int i = 0; i < 2; ++i)
            {
                if (i == 0)
                {
                    ptr_clienteRota = vetClienteRota;
                    ptr_veiculo = solucao->vetorVeiculos[indice0];
                    tam = tam0;

                } else
                {

                    ptr_clienteRota = vetClienteRota2;
                    ptr_veiculo = solucao->vetorVeiculos[indice1];
                    tam = tam1;
                }

                //Verificar se é necessário adicionar ou remover

                //Se diferenca é < 0 : remover da lista |diferenca| a lista. Se é > 0 : adicionar |diferenca| a lista. Se é 0, nao faz nada.
                int diferenca =  tam - ptr_veiculo->listaClientes.size();
                int size = ptr_veiculo->listaClientes.size();

                if (diferenca < 0)
                {
                    while (diferenca != 0)
                    {
                        Solucao::ClienteRota *ptrCliente = *ptr_veiculo->listaClientes.begin();

                        delete ptrCliente;
                        ptr_veiculo->listaClientes.pop_front();

                        ++diferenca;
                    }
                } else if (diferenca > 0)
                {

                    while (diferenca != 0)
                    {
                        Solucao::ClienteRota *ptrCliente = new Solucao::ClienteRota;
                        ptr_veiculo->listaClientes.push_front(ptrCliente);

                        --diferenca;
                    }
                }

                int tamAux = 0;

                if(ptr_veiculo->listaClientes.size() != tam)
                    std::cout<<"tam != lista.size()\n\n"<<"tam: "<<tam<<"\nsize orig: "<<size<<"\nsize novo: "<<ptr_veiculo->listaClientes.size()<<'\n';

                //Copia a solucao para o veiculo
                for (auto it = ptr_veiculo->listaClientes.begin(); it != ptr_veiculo->listaClientes.end(); ++it, ++tamAux)
                {
                    (*it)->swap(&ptr_clienteRota[tamAux]);
                }


            }

            //Atualiza poluicao e combustivel
            solucao->poluicao += -solucao->vetorVeiculos[indice0]->poluicao - solucao->vetorVeiculos[indice1]->poluicao;
            solucao->poluicao += poluico0 + poluicao1;

            solucao->vetorVeiculos[indice0]->poluicao = poluico0;
            solucao->vetorVeiculos[indice1]->poluicao = poluicao1;

            solucao->vetorVeiculos[indice0]->combustivel = combustivel0;
            solucao->vetorVeiculos[indice1]->combustivel = combustivel1;

            solucao->vetorVeiculos[indice1]->carga = peso1;
            solucao->vetorVeiculos[indice0]->carga = peso0;


        }

        ++numRotas;
    }

}
