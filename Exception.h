//
// Created by igor on 09/04/2020.
//

#ifndef HGVRSP_EXCEPTION_H
#define HGVRSP_EXCEPTION_H

#include <exception>

class ExceptionVeiculo: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, veiculo incorreto.\n";
    }
};

class ExceptionPeso: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: calculaFimRota. \nMotivo: Peso passado incorreto\n";
    }
} ;
class ExceptionEscolhido: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: calculaFimRota. \nMotivo: clienteEscolhido eh igual a zero\n";
    }
};

class ExceptioViabilidadeMip: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro, fuc: Modelo::geraRotasOtimas. \nMotivo: resultado do modelo deveria ser viavel.\n";
    }
};

extern ExceptionVeiculo exceptionVeiculo;
extern ExceptionPeso exceptionPeso;
extern ExceptionEscolhido exceptionEscolhido;
extern ExceptioViabilidadeMip exceptionViabilidadeMip;

#endif //HGVRSP_EXCEPTION_H
