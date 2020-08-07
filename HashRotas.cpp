//
// Created by igor on 06/08/2020.
//

#include "HashRotas.h"
//using namespace HashRotas;

HashRotas::HashRotas::HashRotas(int numClientes)
{
    numClientes -= 1;


    switch (numClientes)
    {
        case 10:
            tamTabela = 211;

        case 15:
            tamTabela = 307;

        case 20:
            tamTabela = 401;

        case 25:
            tamTabela = 503;

        case 50:
            tamTabela = 1009;

        case 75:
            tamTabela = 1511;

        case 100:
            tamTabela = 2003;
    }

    tabelaHash = new std::list<HashNo*>[tamTabela];

    if(tabelaHash == NULL)
    {
        std::cout<<"Nao foi possivel alocar a tabela hash\n";
        exit(-1);
    }
}

u_int32_t HashRotas::HashRotas::getHash(Solucao::Veiculo *veiculo)
{
    u_int32_t hash = 5381;

    for(auto cliente : veiculo->listaClientes)
    {
        if(cliente->cliente != 0)
            hash = ((hash << 5) + hash) + cliente->cliente;
    }

    hash = ((hash << 5) + hash) + veiculo->tipo;

    return hash;
}

u_int32_t HashRotas::HashRotas::getHash(Solucao::ClienteRota *clienteRota, const int tam, const int tipo)
{

    u_int32_t hash = 5381;
    Solucao::ClienteRota *cliente = &clienteRota[1];

    for(int i = 1; i < tam-1; ++i, ++cliente)
    {
        hash = ((hash << 5) + hash) + cliente->cliente;
    }

    hash = ((hash << 5) + hash) + tipo;

    return hash;

}

bool HashRotas::HashRotas::insereVeiculo(Solucao::Veiculo *veiculo)
{

    //Verifica veiculo vazio
    if(veiculo->listaClientes.size() == 2)
        return false;


    u_int32_t hash = getHash(veiculo);
    hash = hash % tamTabela;


    std::list<HashNo*> *lista = &tabelaHash[hash];


    if(!lista->empty())
    {
        //Percorre a lista
        for(auto hashNo : *lista)
        {


            if((hashNo->tam == veiculo->listaClientes.size()) && (hashNo->tipo == veiculo->tipo))
            {


                bool encontrou = true;

                //Percorre os clientes

                Solucao::ClienteRota *veiculo_p = hashNo->veiculo;

                for(auto cliente : veiculo->listaClientes)
                {


                    if(veiculo_p->cliente != cliente->cliente)
                    {
                        encontrou = false;
                        break;
                    }


                    ++veiculo_p;
                }

                if(encontrou)
                    return false;

            }

        }
    }

    //Veiculo não foi encontrado na lista

    //Insere o veiculo


    //Cria um novo no
    HashNo *hashNo = new HashNo;

    hashNo->tam = veiculo->listaClientes.size();
    hashNo->veiculo = new Solucao::ClienteRota[hashNo->tam];
    hashNo->carga = veiculo->carga;
    hashNo->tipo = veiculo->tipo;
    hashNo->poluicao = veiculo->poluicao;
    hashNo->combustivel = veiculo->combustivel;

    Solucao::ClienteRota *veiculo_p = hashNo->veiculo;

    for(auto cliente : veiculo->listaClientes)
    {
        veiculo_p->swap(cliente);
        ++veiculo_p;
    }

    lista->push_back(hashNo);

    return true;

}

HashRotas::HashNo* HashRotas::HashRotas::getVeiculo(Solucao::ClienteRota *clienteRota, const int tam, const int tipo)
{

    //Verifica veiculo vazio
    if(tam <= 2)
        return NULL;

    u_int32_t hash = getHash(clienteRota, tam, tipo);

    hash = hash % tamTabela;

    std::list<HashNo*> *lista = &tabelaHash[hash];

    if(lista->empty())
        return NULL;

    for(auto hashNo : *lista)
    {
        if((hashNo->tam == tam) && (hashNo->tipo == tipo))
        {
            bool encontrou = true;

            //Percorre os clientes

            Solucao::ClienteRota *veiculo_p = hashNo->veiculo;
            Solucao::ClienteRota *cliente = clienteRota;

            for(int i = 0; i < tam; ++i)
            {
                if(veiculo_p->cliente != cliente->cliente)
                {
                    encontrou = false;
                    break;
                }

                ++veiculo_p;
                ++cliente;
            }

            if(encontrou)
                return hashNo;

        }
    }

    //Percorreu todos os veiculos e nao encontrou
    return NULL;

}

HashRotas::HashNo* HashRotas::HashRotas::getVeiculo(Solucao::Veiculo *veiculo)
{
    //Verifica veiculo vazio
    if(veiculo->listaClientes.size() <= 2)
        return NULL;

    u_int32_t hash = getHash(veiculo);

    hash = hash % tamTabela;

    std::list<HashNo*> *lista = &tabelaHash[hash];

    if(lista->empty())
        return NULL;

    for(auto hashNo : *lista)
    {
        if((hashNo->tam == veiculo->listaClientes.size()) && (hashNo->tipo == veiculo->tipo))
        {
            bool encontrou = true;

            //Percorre os clientes

            Solucao::ClienteRota *veiculo_p = hashNo->veiculo;


            for(auto cliente : veiculo->listaClientes)
            {
                if(veiculo_p->cliente != cliente->cliente)
                {
                    encontrou = false;
                    break;
                }

                ++veiculo_p;

            }

            if(encontrou)
                return hashNo;

        }
    }

    //Percorreu todos os veiculos e nao encontrou
    return NULL;
}

void HashRotas::HashRotas::estatisticasHash(float *tamanhoMedio_, int *maior_)
{
    u_int32_t tamanhoMedio= 0;
    int maior = tabelaHash[0].size();

    for(int i = 0; i < tamTabela; ++i)
    {
        tamanhoMedio += tabelaHash[i].size();

        if(tabelaHash[i].size() > maior)
            maior = tabelaHash[i].size();
    }

    *tamanhoMedio_ = tamanhoMedio/float(tamTabela);
    *maior_ = maior;
}

HashRotas::HashRotas::~HashRotas()
{
    std::list<HashNo*> *list;

    //deleta HashNo
    for(int i = 0; i < tamTabela; ++i)
    {
        list = &tabelaHash[i];

        if(!list->empty())
        {

            for(auto hashNo : *list)
                delete hashNo;
        }
    }

    delete []tabelaHash;

}

