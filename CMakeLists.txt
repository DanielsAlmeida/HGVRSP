cmake_minimum_required(VERSION 3.13)
project(HGVRSP)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# declara header gurobi directory
set(GUROBI_INCLUDE_DIRECTORY $ENV{GUROBI_HOME}/include)

# adicionar header gurobi
include_directories(${GUROBI_INCLUDE_DIRECTORY})

# declara library gurobi directory
set(GUROBI_LIB_DIRECTORY $ENV{LD_LIBRARY_PATH})

# adicionar library gurobi
link_directories(${GUROBI_LIB_DIRECTORY})

set (CMAKE_CXX_FLAGS " -std=c++2a")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS " -m64")

#set(CMAKE_CXX_FLAGS "-fsanitize=address")
#set(CMAKE_CXX_FLAGS "-g")

set (CMAKE_CXX_FLAGS "-I $HOME/Documentos/Programas/boost_1_72_0")

set(    SOURCE_FILES
        ${GUROBI_INCLUDE_DIRECTORY}/gurobi_c++.h
        main.cpp
        Instancia.cpp
        Instancia.h
        Solucao.cpp
        Solucao.h
        VerificaSolucao.cpp
        VerificaSolucao.h
        Construtivo.cpp
        Construtivo.h
        mersenne-twister.h
        mersenne-twister.cpp
        Movimentos.cpp
        Movimentos.h
        Movimentos2.cpp
        Exception.h
        ViabilizaSolucao.cpp
        ViabilizaSolucao.h
        Vnd.cpp
        Vnd.h
        Movimentos_Paradas.cpp
        Movimentos_Paradas.h
        Constantes.cpp
        Constantes.h
        Modelo.cpp
        Modelo.h
        HashRotas.cpp
        HashRotas.h)

add_executable(HGVRSP ${SOURCE_FILES})
        
target_link_libraries (HGVRSP gurobi_c++)
target_link_libraries (HGVRSP gurobi90)
#target_link_libraries (HGVRSP gurobi81)

#target_link_libraries(HGVRSP ${GUROBI_LIBRARY})
