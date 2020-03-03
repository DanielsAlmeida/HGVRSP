//
// Created by igor on 10/09/19.
//

#include "Instancia.h"
#include <string>
#include <fstream>

using namespace std;
//using namespace Instancia;

Instancia::Instancia::Instancia(std::string arquivo)
{

    std::string clientes;

    demandaTotal = 0;

    int index = (arquivo.length() - 1);

    for(;index > 0; --index)
    {
        if(arquivo[index] == '/')
        {   index +=1;
            break;
        }

    }

    index += 3;

    for(;arquivo[index] != 'x'; ++index)
    {
        clientes = clientes + arquivo[index];
    }
    numClientes = std::atoi(clientes.c_str());
    numClientes +=1; //Acrescenta deposito

    std::ifstream file;
    file.open(arquivo, std::ios::out);

    penalizacao.insert({15, 1500});
    penalizacao.insert({10, 1000});
    penalizacao.insert({20, 2000});
    penalizacao.insert({25, 2000});
    penalizacao.insert({50, 3000});
    penalizacao.insert({75, 4000});
    penalizacao.insert({100, 3000});

    if(file.is_open())
    {

        vetorClientes = new Cliente[numClientes];

        string lixo;
        //Divisão de tempo
        for(int i = 0; i < 8; ++i)
            getline(file, lixo);

        int cliente, demanda, tempoServico, inicioJanela, fimJanela;
        numVeiculos = 0;

        for(int i = 0; i < numClientes;++i)
        {

            file>>cliente>> demanda>> tempoServico>> inicioJanela>> fimJanela;

            vetorClientes[i].cliente = cliente;
            vetorClientes[i].demanda = demanda;
            vetorClientes[i].tempoServico = tempoServico/60.0;
            vetorClientes[i].inicioJanela = inicioJanela/60.0;
            vetorClientes[i].fimJanela = fimJanela/60.0;

            demandaTotal += demanda;

        }

        getline(file, lixo);
        getline(file, lixo);
        getline(file, lixo);

        getline(file, lixo);


        numVeiculos = int(ceil((numClientes-1)/5.0));

        while(lixo[0] != ';')
        {
            getline(file, lixo);
        }


        if((numVeiculos % 2) == 0)
        {
            veiculosTipo1 = numVeiculos/2;
            veiculosTipo2 = numVeiculos/2;
        }
        else
        {
            veiculosTipo1 = numVeiculos/2 + 1;
            veiculosTipo2 = numVeiculos/2;
        }


        for(int i = 0; i < 4; ++i)
            getline(file, lixo);

        //Aloca matriz de distancias

        matrizDistancias = new double* [numClientes];

        int k;

        for(int i = 0; i < numClientes; ++i)
        {
            matrizDistancias[i] = new double[numClientes];

            for(k = 0; k < numClientes; ++k)
            {
                matrizDistancias[i][k] = 0.0;
            }

        }

        //Leitura das distancias

        long posicao = file.tellg();
        getline(file, lixo);


        int i,j;
        float distancia;
        int p = 0;
        while(lixo[0] != ';')
        {
            file.seekg(posicao);
            file>>i>>j>>distancia;

            matrizDistancias[i][j] = distancia;
            posicao = file.tellg();
            getline(file, lixo, '\n');

            if(lixo.size()==1)
                getline(file, lixo);
        }

        //Leitura das velocidades

        //Aloca matriz
        matrizVelocidade = new double**[numClientes];

        for(int i = 0; i < numClientes; ++i)
        {
            matrizVelocidade[i] = new double*[numClientes];
        }

        for(int i = 0; i < numClientes; ++i)
        {
            for(int j = 0; j < numClientes; ++j)
            {
                matrizVelocidade[i][j] = new double[5];
            }
        }

        int  velocidade;
        getline(file, lixo);

        posicao = file.tellg();
        getline(file, lixo);

        //Leitura dos valores
        while(lixo[0] != ';')
        {
            file.seekg(posicao);
            file>>i>>j>>k>>velocidade;

            matrizVelocidade[i][j][k-1] = velocidade;

            posicao = file.tellg();
            getline(file, lixo, '\n');

            if(lixo.size()==1)
            {
                getline(file, lixo);


            }


        }

        getline(file, lixo);

        //Matriz de co2


        matrizCo2 = new double***[numClientes];

        for(i = 0; i < numClientes; ++i)
        {
            matrizCo2[i] = new double**[numClientes];
        }

        //Periodos

        for(i = 0; i < numClientes; ++i)
        {
            for( j = 0; j< numClientes; ++j)
            {
                matrizCo2[i][j] = new double*[5];
            }
        }

        for(i = 0; i < numClientes; ++i)
        {
            for(j = 0; j< numClientes; ++j)
            {
                for(int l = 0; l < 5; ++l)
                matrizCo2[i][j][l] = new double[2];
            }
        }

        double g;
        int r;

        posicao = file.tellg();
        getline(file, lixo);

        while(lixo[0] != ';')
        {
            file.seekg(posicao);
            file>>i>>j>>k>>r>>g;



            matrizCo2[i][j][k-1][r-1] = g;

            posicao = file.tellg();
            getline(file, lixo, '\n');

            if(lixo.size()==1)
            {
                getline(file, lixo);


            }


        }


        for(int i = 1; i < numClientes; ++i)
        {

            vetorClientes[i].distanciaDeposito = matrizDistancias[0][i];
            vetorClientes[i].periodoFimJanela = retornaPeriodo(vetorClientes[i].fimJanela);
        }

        file.close();

        distanciaDeposito = new double[numClientes];
        double maior;

        for(int i = 0; i < numClientes; ++i)
        {
            maior = matrizDistancias[i][0];

            if(matrizDistancias[0][i] > maior)
                maior = matrizDistancias[0][i];

            distanciaDeposito[i] = maior;
        }

    }
    else
    {
        std::cout<<"Nao foi possivel abrir o arquivo: "<<arquivo<<std::endl;
        exit(-1);
    }

}

Instancia::Instancia::~Instancia()
{
//***********************Libera memoria*************************************************


    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j< numClientes; ++j)
        {
            for(int l = 0; l < 5; ++l)
                delete []matrizCo2[i][j][l];
        }
    }

    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j< numClientes; ++j)
        {
            delete []matrizCo2[i][j];
        }
    }

    for(int i = 0; i < numClientes; ++i)
    {
        delete []matrizCo2[i];
    }

    delete matrizCo2;


    for(int i = 0; i < numClientes; ++i)
    {
        for(int j = 0; j < numClientes; ++j)
        {
            delete []matrizVelocidade[i][j];
        }
    }

    for(int i = 0; i < numClientes; ++i)
    {
        delete []matrizVelocidade[i];
    }

    delete []matrizVelocidade;

    delete []vetorClientes;

    for(int i = 0; i < numClientes; ++i)
    {
        delete []matrizDistancias[i];
    }

    delete []matrizDistancias;
    delete []distanciaDeposito;
}

int Instancia::Instancia::retornaPeriodo(float hora) const
{


    if(hora >= 0 && hora < 1.8)

        return 0;

    else if(hora >= 1.8 && hora < 3.6)
        return 1;

    else if(hora >= 3.6 && hora < 5.4)
        return 2;

    else if(hora >= 5.4 && hora < 7.2)
        return 3;

    else if(hora >= 7.2 && hora <= 9.67)
        return 4;
    else
        return -1;
}

void Instancia::Instancia::getClientes()
{

/*    for(int i = 1; i < numClientes; ++i)
    {

        cout<<vetorClientes[i].cliente<<": "<<vetorClientes[i].demanda<<'\n';
    }*/

}