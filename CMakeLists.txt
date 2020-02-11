cmake_minimum_required(VERSION 3.13)
project(HGVRSP)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

add_executable(HGVRSP main.cpp Instancia.cpp Instancia.h Solucao.cpp Solucao.h VerificaSolucao.cpp VerificaSolucao.h Construtivo.cpp Construtivo.h mersenne-twister.h mersenne-twister.cpp Movimentos.cpp Movimentos.h)