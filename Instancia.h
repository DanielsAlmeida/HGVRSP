//
// Created by igor on 10/09/19.
//

#ifndef HGVRSP_INSTANCIA_H
#define HGVRSP_INSTANCIA_H

typedef struct
{
    int inicio;
    int fim;

}Periodo;

typedef struct
{
    int cliente;
    int demanda;
    int tempoServico;
    int inicioJanela;
    int fimJanela;

}Cliente;

typedef struct
{
    int capacidade;
    int combustivel;
    double pVeiculo;  //Acrescimo de combustivel por carga
    double cVeiculo;  //Taxa conversao combustivel/co2
    int inicioJanela;
    int fimJanela;
}Veiculo;

class Instancia
{

public:

    Periodo *vetorPeriodos;
    Cliente *vetorClientes;
    Veiculo vetorVeiculos[2] = {{10001, 150, 2.2384e-005, 2.66374, 0, 540},{15000, 200, 1.79961e-005, 2.66374, 30, 540}};





};

#endif //HGVRSP_INSTANCIA_H
