//
// Created by igor on 24/09/19.
//

#ifndef HGVRSP_SOLUCAO_H
#define HGVRSP_SOLUCAO_H

#include <list>
#include <vector>
#include <string>
#include <iostream>



struct PertubacaoInviabilidade
{
    int peso;
    int janelaTempo;
    int combustivel;

    PertubacaoInviabilidade()
    {
        peso = 0;
        janelaTempo = 0;
        combustivel = 0;
    }
};

namespace Solucao
{

    class ClienteRota
    {

    public:

        int cliente;                    //id do cliente
        bool percorrePeriodo[5];       //true se percorre periodo de i a j, false caso contrário.
        long double tempoPorPeriodo[5];
        long double distanciaPorPeriodo[5];
        long double tempoChegada;
        long double tempoSaida;
        long double poluicao;                // Poluição somente de i para j.
        long double combustivel;             // Combustível somente de i para j.
        long double combustivelRota;
        long double poluicaoRota;
        long double produtoCarga;
        long double distanciaAteCliente;
        bool rotaMip;


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
            distanciaAteCliente = outro.distanciaAteCliente;
            rotaMip = outro.rotaMip;

            for(int i = 0; i < 5; ++i)
            {
                percorrePeriodo[i] = outro.percorrePeriodo[i];
                tempoPorPeriodo[i] = outro.tempoPorPeriodo[i];
                distanciaPorPeriodo[i] = outro.distanciaPorPeriodo[i];
            }
        }

        void operator = (const ClienteRota &outro)
        {


            cliente = outro.cliente;
            tempoChegada = outro.tempoChegada;
            tempoSaida = outro.tempoSaida;
            poluicao = outro.poluicao;
            combustivel = outro.combustivel;
            poluicaoRota = outro.poluicaoRota;
            produtoCarga = outro.produtoCarga;
            combustivelRota = outro.combustivelRota;
            distanciaAteCliente = outro.distanciaAteCliente;
            rotaMip = outro.rotaMip;

            for(int i = 0; i < 5; ++i)
            {
                percorrePeriodo[i] = outro.percorrePeriodo[i];
                tempoPorPeriodo[i] = outro.tempoPorPeriodo[i];
                distanciaPorPeriodo[i] = outro.distanciaPorPeriodo[i];
            }
        }

        void swap(ClienteRota *outro)
        {
            cliente = outro->cliente;
            tempoChegada = outro->tempoChegada;
            tempoSaida = outro->tempoSaida;
            poluicao = outro->poluicao;
            combustivel = outro->combustivel;
            poluicaoRota = outro->poluicaoRota;
            produtoCarga = outro->produtoCarga;
            combustivelRota = outro->combustivelRota;
            distanciaAteCliente = outro->distanciaAteCliente;
            rotaMip = outro->rotaMip;

            for(int i = 0; i < 5; ++i)
            {
                percorrePeriodo[i] = outro->percorrePeriodo[i];
                tempoPorPeriodo[i] = outro->tempoPorPeriodo[i];
                distanciaPorPeriodo[i] = outro->distanciaPorPeriodo[i];
            }
        }



    };

    class Veiculo
    {

    private: std::list<ClienteRota *>::iterator itListaCliente;

    public:

        std::list<ClienteRota *> listaClientes; //Lista de clientes
        double combustivel;                      //Quantidade gasta de combustível
        double poluicao;
        int carga;                            //Quantidade de produtos
        int  tipo;                            //tipo == 0 => tipo 1; tipo == 1 => tipo 2;
        int id;

        Veiculo(int  tipo, int _id)
        {
            poluicao = 0.0;
            combustivel = 0.0;
            carga = 0;
            this->tipo = tipo;
            id = _id;

            inicialisaVeiculo(tipo);

        }

        Veiculo(int  tipo)
        {
            poluicao = 0.0;
            combustivel = 0.0;
            carga = 0;
            this->tipo = tipo;
            id = -1;

            inicialisaVeiculo(tipo);

        }

        double getDistancia()
        {
            itListaCliente = listaClientes.end();
            itListaCliente--;

            return (*itListaCliente)->distanciaAteCliente;
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
            id = outro.id;

            ClienteRota *clientePtr = 0;

            for(auto it : outro.listaClientes)
            {
                clientePtr = new ClienteRota(*it);

                listaClientes.push_back(clientePtr);
            }

        }

        Veiculo(const Veiculo *outro)
        {

            combustivel = outro->combustivel;
            poluicao = outro->poluicao;
            carga = outro->carga;
            tipo = outro->tipo;
            id = outro->id;

            ClienteRota *clientePtr = 0;

            for(auto it : outro->listaClientes)
            {
                clientePtr = new ClienteRota(*it);

                listaClientes.push_back(clientePtr);
            }
        }

        std::string getRota(void)
        {
            std::string saida;

            //saida = "********************************TIPO-"+std::to_string(tipo)+"********************************************\n";

            for(auto cliente : listaClientes)
            {

                saida += std::to_string(cliente->cliente) + ' ';
/*                saida += "TempoChegada: " + std::to_string(cliente->tempoChegada) + "  ";
                saida += "TempoSaida: " + std::to_string(cliente->tempoSaida) + "\n";
                saida += "PoluicaoRota: " + std::to_string(cliente->poluicaoRota) + "  ";
                saida += "Poluicao: " + std::to_string(cliente->poluicao) + "\n";
                saida += "CombustivelRota: " + std::to_string(cliente->combustivelRota) + "  ";
                saida += "Combustivel: " + std::to_string(cliente->combustivel) + "\n\n";*/

            }

            //saida += "****************************************************************************\n";

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

        static bool compare(const Veiculo *veiculo0 , const Veiculo *veiculo1)
        {
            return veiculo0->poluicao < veiculo1->poluicao;
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
        int solucoesViabilizadas;
        double poluicaoPenalidades;
        double tempoConstrutivo;
        double tempoVnd;
        double tempoViabilizador;
        bool rotasMip;

        PertubacaoInviabilidade *inviabilidadeEstatatisticas;

        int totalInteracoesILS;
        int solucoesInviaveis;

        Solucao()
        {

        }

        Solucao(int numVeiculos);
        Solucao(Solucao *outro);

        double getPoluicaoTotal()
        {
            return poluicao + poluicaoPenalidades;
        }

        static bool compare(const Solucao *solucao0, const Solucao *solucao1)
        {
            return solucao0->poluicao < solucao1->poluicao;
        }

        ~Solucao();


    };

}

#endif //HGVRSP_SOLUCAO_H
