cmake_minimum_required(VERSION 3.13)
project(GERA_SOLUCAO)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set(CMAKE_CXX_FLAGS "-O3")

add_executable(GERA_SOLUCAO main.cpp Instancia.cpp Instancia.h VerificaSolucao.cpp VerificaSolucao.h mersenne-twister.h mersenne-twister.cpp Solucao.cpp Solucao.h )
