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

        int cliente;                    //id do cliente
        bool percorrePeriodo[5];       //true se percorre periodo de i a j, false caso contrário.
        double tempoChegada;
        double tempoSaida;
        double poluicao;                // Poluição somente de i para j.
        double combustivel;             // Combustível somente de i para j.



    }ClienteRota;

    class Veiculo
    {

    public:

        std::list<ClienteRota *> listaClientes; //Lista de clientes
        double combustivel;                      //Quantidade gasta de combustível
        double poluicao;
        int carga;                            //Quantidade de produtos
        int  tipo;                            //tipo == 0 => tipo 1; tipo == 1 => tipo 2;

        Veiculo(int  tipo)
        {
            poluicao = 0.0;
            combustivel = 0.0;
            carga = 0;
            this->tipo = tipo;

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
