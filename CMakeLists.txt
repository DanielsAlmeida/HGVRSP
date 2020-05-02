cmake_minimum_required(VERSION 3.13)
project(HGVRSP)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set (CMAKE_CXX_FLAGS "-O3")
set (CMAKE_CXX_FLAGS "-I $HOME/Documentos/Programas/boost_1_72_0")
set(CMAKE_CXX_FLAGS "-fconcepts")

add_executable(HGVRSP main.cpp Instancia.cpp Instancia.h Solucao.cpp Solucao.h VerificaSolucao.cpp VerificaSolucao.h Construtivo.cpp Construtivo.h mersenne-twister.h mersenne-twister.cpp Movimentos.cpp Movimentos.h Movimentos2.cpp Exception.h ViabilizaSolucao.cpp ViabilizaSolucao.h Vnd.cpp Vnd.h Movimentos_Paradas.cpp Movimentos_Paradas.h)
