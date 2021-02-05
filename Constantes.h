//
// Created by igor on 07/05/2020.
//

#ifndef HGVRSP_CONSTANTES_H
#define HGVRSP_CONSTANTES_H

extern const int Heuristica_0; //Tempo -> poluicao
extern const int Heuristica_1; //Tempo -> poluicao -> distancia_deposito
extern const int Heuristica_2; //Distancia
extern const int Heuristica_3; //Distancia -> poluicao
extern const int Heuristica_4; // Tempo -> Distancia -> Poluicao
extern const int Heuristica_5; // Distancia ->Tempo -> Poluicao
extern const int Heuristica_6; // Tempo -> Poluicao -> Distancia

extern const int Heuristica_Cliente_0; // Tempo
extern const int Heuristica_Cliente_1; // Poluicao
extern const int Heuristica_Cliente_2; // distancia_deposito
extern const int Heuristica_Cliente_3; // Distancia

extern const int MaxTamVetClientesMatrix;

extern const int OpcaoGrasp, OpcaoIls, OpcaoGraspMip, OpcaoIlsMip, OpcaoGraspIls, OpcaoGraspIlsMip, OpcaoGraspComIlsMip;



#endif //HGVRSP_CONSTANTES_H
