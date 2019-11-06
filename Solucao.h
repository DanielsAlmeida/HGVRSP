//
// Created by igor on 24/09/19.
//

#ifndef HGVRSP_SOLUCAO_H
#define HGVRSP_SOLUCAO_H

#include <list>
#include <vector>

namespace Solucao
{

    typedef struct
    {

    public:

        int cliente;                  //id do cliente
        //float distanciaPeriodo[5];  //Distancia em que o veiculo percorreu em cada periodo para chegar no cliente
        double tempoChegada;
        double tempoSaida;
        double poluicao, combustivel; // Poluicao e combustivel parcial do veiculo.



    }ClienteRota;

    class Veiculo
    {

    public:

        std::list<ClienteRota *> listaClientes; //Lista de clientes
        double combustivel;                      //Quantidade gasta de combustível
        double poluicao;
        int carga;                            //Quantidade de produtos

        Veiculo()
        {
            poluicao = combustivel = 0.0;
            carga = 0;

            inicialisaVeiculo();

        }

        ~Veiculo()
        {
            ClienteRota *clienteAux;

            while(!listaClientes.empty())
            {
                clienteAux = listaClientes.front();
                listaClientes.pop_front();

                delete clienteAux;

            }
        }

    private:
        void inicialisaVeiculo();


    };

    class Solucao
    {

    public:
        std::vector<Veiculo*> vetorVeiculos;
        double poluicao;

        Solucao(int numVeiculos);

        ~Solucao();


    };

}

#endif //HGVRSP_SOLUCAO_H
