//
// Created by igor on 24/09/19.
//

#ifndef HGVRSP_SOLUCAO_H
#define HGVRSP_SOLUCAO_H

#include <list>
#include <vector>
#include <string>
#include <iostream>

namespace Solucao
{

    class ClienteRota
    {

    public:

        int cliente;                    //id do cliente
        bool percorrePeriodo[5];       //true se percorre periodo de i a j, false caso contrário.
        double tempoChegada;
        double tempoSaida;
        double poluicao;                // Poluição somente de i para j.
        double combustivel;             // Combustível somente de i para j.
        double combustivelRota;
        double poluicaoRota;
        double produtoCarga;

        ClienteRota(){}
        ClienteRota(const ClienteRota &outro)
        {
            cliente = outro.cliente;
            tempoChegada = outro.tempoChegada;
            tempoSaida = outro.tempoSaida;
            poluicao = outro.poluicao;
            combustivel = outro.combustivel;
            poluicaoRota = outro.poluicaoRota;
            produtoCarga = outro.produtoCarga;
            combustivelRota = outro.combustivelRota;

            for(int i = 0; i < 5; ++i)
                percorrePeriodo[i] = outro.percorrePeriodo[i];
        }

    };

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

            inicialisaVeiculo(tipo);

        }

        std::string printRota()
        {
            std::string s = "";

            for(auto it : listaClientes)
                s += std::to_string(it->cliente) + ' ';

            return s;
        }

        Veiculo(const Veiculo &outro)
        {
            combustivel = outro.combustivel;
            poluicao = outro.poluicao;
            carga = outro.carga;
            tipo = outro.tipo;
            ClienteRota *clientePtr = 0;

            for(auto it : outro.listaClientes)
            {
                clientePtr = new ClienteRota(*it);

                listaClientes.push_back(clientePtr);
            }

        }

        std::string getRota(void)
        {
            std::string saida;

            saida = "********************************TIPO-"+std::to_string(tipo)+"********************************************\n";

            for(auto cliente : listaClientes)
            {

                saida += "Cliente: " + std::to_string(cliente->cliente) + "\n";
                saida += "TempoChegada: " + std::to_string(cliente->tempoChegada) + "  ";
                saida += "TempoSaida: " + std::to_string(cliente->tempoSaida) + "\n";
                saida += "PoluicaoRota: " + std::to_string(cliente->poluicaoRota) + "  ";
                saida += "Poluicao: " + std::to_string(cliente->poluicao) + "\n";
                saida += "CombustivelRota: " + std::to_string(cliente->combustivelRota) + "  ";
                saida += "Combustivel: " + std::to_string(cliente->combustivel) + "\n\n";

            }

            saida += "****************************************************************************\n";

            return saida;
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
        void inicialisaVeiculo(int tipo);


    };

    class Solucao
    {

    public:
        std::vector<Veiculo*> vetorVeiculos;
        double poluicao;
        bool veiculoFicticil;
        int ultimaAtualizacao;
        int numSolucoesInv;
        double poluicaoPenalidades;

        Solucao(int numVeiculos);

        ~Solucao();


    };

}

#endif //HGVRSP_SOLUCAO_H
