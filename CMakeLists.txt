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
set(CMAKE_CXX_FLAGS " -m64")

#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS "-g")

#cmake .. -DCMAKE_BUILD_TYPE=asan

set(CMAKE_CXX_FLAGS_TSAN  "-fsanitize=thread -g -O1" CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds." FORCE)

set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1" CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds." FORCE)

set(CMAKE_CXX_FLAGS_LSAN "-fsanitize=leak -fno-omit-frame-pointer -g -O1" CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds." FORCE)

set(CMAKE_CXX_FLAGS_MSAN "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2" CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds." FORCE)

set(CMAKE_CXX_FLAGS_UBSAN "-fsanitize=undefined" CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds." FORCE)


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
