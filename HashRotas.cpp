//
// Created by igor on 06/08/2020.
//

#include "HashRotas.h"
//using namespace HashRotas;

HashRotas::HashRotas::HashRotas(int numClientes)
{
    numClientes -= 1;
    numRotas = 0;

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

    tabelaHash = new std::vector<HashNo*>[tamTabela];

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

bool HashRotas::HashRotas::insereVeiculo(Solucao::ClienteRota *clienteRotaOriginal, Solucao::ClienteRota *clienteBest,
                                         double poluicaoBest, double combustivelBest, const int tam, const bool tipo,
                                         const int carga)
{

    //Verifica veiculo vazio
    if(tam == 2)
        return false;


    u_int32_t hashRotaOriginal = getHash(clienteRotaOriginal, tam, tipo);
    hashRotaOriginal = hashRotaOriginal % tamTabela;

    bool iguais = true;


    for(int i = 0; i < tam; ++i)
    {
        if(clienteRotaOriginal[i].cliente != clienteBest[i].cliente)
        {
            iguais = false;
            break;
        }
    }

    u_int32_t hashRotaBest;
    HashNo *hashNoBest = NULL;



    hashRotaBest = getHash(clienteBest, tam, tipo);
    hashRotaBest = hashRotaBest % tamTabela;

    std::vector<HashNo*> *lista = &tabelaHash[hashRotaBest];


        if(!lista->empty())
        {
            //Percorre a lista
            for(auto hashNo : *lista)
            {


                if((hashNo->tam == tam) && (hashNo->tipo == tipo) && (hashNo->carga == carga))
                {


                    bool encontrou = true;

                    //Percorre os clientes

                    Solucao::ClienteRota *veiculo_p = hashNo->veiculo;

                    for(int i = 0; i < tam; ++i)
                    {


                        if(veiculo_p->cliente != clienteBest[i].cliente)
                        {
                            encontrou = false;
                            break;
                        }


                        ++veiculo_p;
                    }

                    if(encontrou)
                    {
                        if(iguais)
                            return false;

                        hashNoBest = hashNo;
                        break;


                    }

                }

            }
        }

    if(!hashNoBest)
    {
        hashNoBest = new HashNo;
        hashNoBest->tam = tam;
        hashNoBest->veiculo = new Solucao::ClienteRota[hashNoBest->tam];
        hashNoBest->carga = carga;
        hashNoBest->tipo = tipo;
        hashNoBest->best = NULL;
        hashNoBest->poluicaoH = -1.0;

        numRotas += 1;

        hashNoBest->poluicao = poluicaoBest;
        hashNoBest->combustivel = combustivelBest;

        Solucao::ClienteRota *veiculo_p = hashNoBest->veiculo;
        Solucao::ClienteRota *veiculo_V = clienteBest;
        for (int i = 0; i < tam; ++i)
        {
            veiculo_p->swap(veiculo_V);

            ++veiculo_p;
            ++veiculo_V;
        }

        lista->push_back(hashNoBest);

        if(iguais)
            return true;
    }
    else
    {
        if(iguais)
            return false;
    }

    //Encontrou best



    lista = &tabelaHash[hashRotaOriginal];
    HashNo *hashNoVeic = NULL;

    if(!lista->empty())
    {
        //Percorre a lista
        for(auto hashNo : *lista)
        {


            if((hashNo->tam == tam) && (hashNo->tipo == tipo) && (hashNo->carga == carga))
            {


                bool encontrou = true;

                //Percorre os clientes

                Solucao::ClienteRota *veiculo_p = hashNo->veiculo;
                Solucao::ClienteRota *ptr_veiculoOrig = clienteRotaOriginal;

                for(int i = 0; i < tam; ++i)
                {


                    if(veiculo_p->cliente != ptr_veiculoOrig->cliente)
                    {
                        encontrou = false;
                        break;
                    }


                    ++veiculo_p;
                    ++ptr_veiculoOrig;
                }

                if(encontrou)
                {

                    hashNoVeic = hashNo;
                    break;


                }


            }

        }


    }


    if(!hashNoVeic)
    {
        hashNoVeic = new HashNo;
        hashNoVeic->tipo = tipo;
        hashNoVeic->carga = carga;
        hashNoVeic->tam = tam;
        hashNoVeic->veiculo = new Solucao::ClienteRota[hashNoVeic->tam];
        hashNoVeic->poluicaoH = -1.0;

        Solucao::ClienteRota *veiculo_p = hashNoVeic->veiculo;
        Solucao::ClienteRota *ptr_veiculoOrig = clienteRotaOriginal;

        for(int i = 0; i < tam; ++i)
        {

            veiculo_p->cliente = ptr_veiculoOrig->cliente;

            ++veiculo_p;
            ++ptr_veiculoOrig;
        }


        lista->push_back(hashNoVeic);
    }

    if(hashNoVeic && hashNoBest)
    {
        hashNoVeic->best = hashNoBest;
        return true;
    }
    else
    {
        std::cout<<"Erro, condicao impossivel.\nArquivo: HashRotas.cpp\nFuncao: insereVeiculo\nLinha: "<<__LINE__<<'\n';

        if(!hashNoVeic)
            std::cout<<"hashNoVeic eh NULL\n";

        if(!hashNoBest)
            std::cout<<"hashNoBest eh NULL\n";

        exit(-1);
    }

    return true;

}

bool HashRotas::HashRotas::insereVeiculo(Solucao::ClienteRota *vetClienteRota, double poluicaoBest, double combustivelBest, const int tam, const bool tipo, const int carga)
{
    u_int32_t hashRotaBest;
    HashNo *hashNoVeic = NULL;

    hashRotaBest = getHash( vetClienteRota, tam, tipo);
    hashRotaBest = hashRotaBest % tamTabela;

    std::vector<HashNo*> *lista = &tabelaHash[hashRotaBest];


    if(!lista->empty())
    {
        //Percorre a lista
        for(auto hashNo : *lista)
        {


            if((hashNo->tam == tam) && (hashNo->tipo == tipo) && (hashNo->carga == carga))
            {


                bool encontrou = true;

                //Percorre os clientes

                Solucao::ClienteRota *veiculo_p = hashNo->veiculo;

                for(int i = 0; i < tam; ++i)
                {


                    if(veiculo_p->cliente != vetClienteRota[i].cliente)
                    {
                        encontrou = false;
                        break;
                    }


                    ++veiculo_p;
                }

                if(encontrou)
                {
                    //Veiculo_p ja esta na hash

                    return false;


                }
                else
                {


                    hashNoVeic = hashNo;
                    break;
                }

            }

        }
    }


    if(!hashNoVeic)
    {
        hashNoVeic = new HashNo;

        hashNoVeic->tipo = tipo;
        hashNoVeic->carga = carga;
        hashNoVeic->poluicao = poluicaoBest;
        hashNoVeic->combustivel = combustivelBest;
        hashNoVeic->tam = tam;
        hashNoVeic->veiculo = new Solucao::ClienteRota[hashNoVeic->tam];
        hashNoVeic->poluicaoH = -1.0;
        hashNoVeic->best = NULL;

        Solucao::ClienteRota *veiculo_p = hashNoVeic->veiculo;
        Solucao::ClienteRota *ptr_veiculoOrig = vetClienteRota;

        for(int i = 0; i < tam; ++i)
        {

            veiculo_p->swap(ptr_veiculoOrig);

            ++veiculo_p;
            ++ptr_veiculoOrig;
        }


        lista->push_back(hashNoVeic);
    }

    return true;
}

bool HashRotas::HashRotas::getVeiculo(Solucao::ClienteRota *clienteRota, const int tam, const int tipo, double *poluicao, double *combustivel)
{

    //Verifica veiculo vazio
    if(tam <= 2)
        return NULL;

    u_int32_t hash = getHash(clienteRota, tam, tipo);


    hash = hash % tamTabela;

    std::vector<HashNo*> *lista = &tabelaHash[hash];

    if(lista->empty())
        return false;

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
            {

                while(hashNo->best)
                    hashNo = hashNo->best;

                veiculo_p = hashNo->veiculo;
                cliente = clienteRota;

                for(int i = 0; i < tam; ++i)
                {
                    cliente->swap(veiculo_p);

                    ++cliente;
                    ++veiculo_p;
                }

                *combustivel = hashNo->combustivel;
                *poluicao = hashNo->poluicao;

                return hashNo;
            }

        }
    }

    //Percorreu todos os veiculos e nao encontrou
    return false;

}

HashRotas::HashNo* HashRotas::HashRotas::getVeiculo(Solucao::Veiculo *veiculo)
{
    //Verifica veiculo vazio
    if(veiculo->listaClientes.size() <= 2)
        return NULL;

    u_int32_t hash = getHash(veiculo);

    hash = hash % tamTabela;

    std::vector<HashNo*> *lista = &tabelaHash[hash];

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
            {
                while(hashNo->best)
                    hashNo = hashNo->best;

                return hashNo;
            }

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


    std::vector<HashNo*> *list;

    //deleta HashNo
    for(int i = 0; i < tamTabela; ++i)
    {
        list = &tabelaHash[i];

        if(!list->empty())
        {

            for(auto hashNo : *list)
            {
                delete []hashNo->veiculo;
                delete hashNo;
            }
        }
    }

    delete []tabelaHash;



}

