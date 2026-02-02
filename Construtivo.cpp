#include "Construtivo.h"
#include <cmath>
#include <tuple>
#include "mersenne-twister.h"
#include <unordered_map>
#include <chrono>
#include "Movimentos.h"
#include "ViabilizaSolucao.h"
#include "Movimentos_Paradas.h"
#include "Constantes.h"
#include "HashRotas.h"
#include "Modelo2Rotas.h"
#include "Ils.h"

#define DEBUG false
#define Debug false

using namespace Construtivo;
using namespace std;

void breakPoint()
{

}

class ExceptioMvShifit: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Erro. \nMotivo: Movimento Shifit esta erado.\n";
    }
} exceptioMvShifit;

vector<int> Construtivo::vetorInicioRotas(Solucao::Solucao *solucao, int p){
    
    vector<int> seq, depositosRota;
    int numdeposits = 0;

    depositosRota.clear();

    seq.clear();
    for (auto veiculo : solucao->vetorVeiculos) {
        for (auto cliente : veiculo->listaClientes) 
            seq.push_back(cliente->cliente);}

    cout << "Sequencia "<< p << ": ";
    for(const auto& element : seq)
        cout << element << " ";
    cout << endl;
    // Registra os depósitos na sequência 1
    for (size_t i = 0; i < seq.size(); ++i) 
        if (seq[i] == 0) {
            depositosRota.push_back(i);
            numdeposits++;
        }
    vector <int> inicioRotas;
    // cout << "\n ID depositos rota seq: ";
    // for(const auto& element : depositosRota)
    //     cout << element << " ";

    for (size_t i = 0; i < depositosRota.size(); i+=2) 
        inicioRotas.push_back(depositosRota[i]);

    // cout << "\n Numero depositos: " << numdeposits << endl;
    
    for(int i = 1; i < inicioRotas.size(); ++i){
        if(inicioRotas[i] == inicioRotas[i-1]+2)
            inicioRotas.erase(inicioRotas.begin() + i-1);
    }
    // cout << "\n ID inicios rotas seq: ";
    // for(const auto& element : inicioRotas)
    //     cout << element << " ";
    // cout << endl;

    return inicioRotas;
}

int Construtivo::extraiSequenciaClientes(
    Solucao::Solucao *sol,
    vector<int> &sequencia
) {
    int numdeposits = 0;
    sequencia.clear();
    for (auto veiculo : sol->vetorVeiculos) {
        for (auto cliente : veiculo->listaClientes) {
            if (cliente->cliente == 0)
                numdeposits++;
            else
                sequencia.push_back(cliente->cliente);
        }
    }
    return numdeposits;
}

void Construtivo::completaPopulacaoInicial(
    Solucao::Solucao **populacaoInicial,
    int tamanhoPopulacao,
    int metadePreenchida,
    const Instancia::Instancia *instancia,
    float alfa,
    Solucao::ClienteRota *vetorClienteBest,
    Solucao::ClienteRota *vetorClienteAux,
    string *sequenciaStr,
    Construtivo::Candidato *vetorCandidatos,
    boost::tuple<int,int>* heuristica,
    const double *const vetorParametros,
    Solucao::ClienteRota **matrixClienteBest,
    Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
    GuardaCandInteracoes *vetCandInteracoes,
    double *vetLimiteTempo,
    HashRotas::HashRotas *hashRotas
) {
    vector<int> seq1, seq2, novaSeq, inicioRotas1, inicioRotas2;

    for (int i = metadePreenchida; i < tamanhoPopulacao; ++i) {

        //    Seleção dos pais
        int p1, p2;
        do {
            p1 = rand() % metadePreenchida;
            p2 = rand() % metadePreenchida;
        } while (p1 == p2);

        int numdeposits1 = 0, numdeposits2 = 0;

        inicioRotas1.clear();
        inicioRotas2.clear();
        
        cout << endl;
        inicioRotas1 = vetorInicioRotas(populacaoInicial[p1], 1);
        inicioRotas2 = vetorInicioRotas(populacaoInicial[p2], 2);

        seq1.clear();
        for (auto veiculo : populacaoInicial[p1]->vetorVeiculos) 
            for (auto cliente : veiculo->listaClientes) 
                seq1.push_back(cliente->cliente);


        seq2.clear();
        for (auto veiculo : populacaoInicial[p2]->vetorVeiculos) 
            for (auto cliente : veiculo->listaClientes) 
                seq2.push_back(cliente->cliente);
    
        
        if (seq1.size() == 0 || seq2.size() == 0) {
            i--;
            continue;
        }

        // definir cortes baseado na quantidade de rotas
        
        bool bloco1P1 = rand() % 2;
        bool bloco2P1 = rand() % 2;
        bool bloco3P1 = rand() % 2;
        
        // Evita clone total
        if ((bloco1P1 && bloco2P1 && bloco3P1) || (!bloco1P1 && !bloco2P1 && !bloco3P1))
            bloco2P1 = !bloco2P1;
        
        float numRotas = (seq1.size()+seq2.size())/2;
        float numRotas1 = inicioRotas1.size();
        float numRotas2 = inicioRotas2.size();
        int c1, c2;
        int n1 = seq1.size(), n2 = seq2.size();

        if(numRotas == 1){
            
            c1 = static_cast<int>(0.3 * n1);
            c2 = static_cast<int>(0.7 * n1);

            novaSeq.clear();

            for (int j = 0; j < n1; ++j) {
                if (j < c1) 
                    novaSeq.push_back(bloco1P1 ? seq1[j] : seq2[j]);
                else if (j < c2) 
                    novaSeq.push_back(bloco2P1 ? seq1[j] : seq2[j]);
                else 
                    novaSeq.push_back(bloco3P1 ? seq1[j] : seq2[j]);
            }

        }
        else if(numRotas == 2){

            novaSeq.clear();

            for (int j = 0; j < n1; ++j) {
                if (j < inicioRotas1[1]) 
                    novaSeq.push_back(bloco1P1 ? seq1[j] : seq2[j]);
                else 
                    novaSeq.push_back(bloco2P1 ? seq1[j] : seq2[j]);
            }
        }
        else{
            int resultado[4], aux1, aux2;
            for(int i = 0; i<2;++i){
                aux1 = static_cast<int>(0.2 * n1);
                aux2 = static_cast<int>(0.4 * n1);
                if (aux1 > aux2) std::swap(aux1, aux2);
                resultado[i] = aux1 + rand() % (aux2 - aux1 + 1);

                aux1 = static_cast<int>(0.2 * n2);
                aux2 = static_cast<int>(0.4 * n2);
                if (aux1 > aux2) std::swap(aux1, aux2);
                resultado[i+2] = aux1 + rand() % (aux2 - aux1 + 1);
            }
            
            //pontos de corte para pai 1
            int c1_1 = resultado[0];
            int c2_1 = resultado[1]+c1_1;

            //pontos de corte para pai 2
            int c1_2 = resultado[2];
            int c2_2 = resultado[3]+c1_2;

            novaSeq.clear();

            novaSeq.clear();

            // Bloco 1
            if (bloco1P1) 
                for (int j = 0; j < c1_1; ++j)
                    novaSeq.push_back(seq1[j]);
            else
                for (int j = 0; j < c1_2; ++j)
                    novaSeq.push_back(seq2[j]);
            

            // Bloco 2
            if (bloco2P1)
                for (int j = c1_1; j < c2_1; ++j)
                    novaSeq.push_back(seq1[j]);
            else
                for (int j = c1_2; j < c2_2; ++j)
                    novaSeq.push_back(seq2[j]);
            

            // Bloco 3
            if (bloco3P1)
                for (int j = c2_1; j < n1; ++j)
                    novaSeq.push_back(seq1[j]);
             else
                for (int j = c2_2; j < n2; ++j)
                    novaSeq.push_back(seq2[j]);
            

            // for (int j = 0; j < n1; ++j) {
            //     if (j < c1_1) 
            //         novaSeq.push_back(bloco1P1 ? seq1[j] : seq2[j]);
            //     else if (j < c2_1) 
            //         novaSeq.push_back(bloco2P1 ? seq1[j] : seq2[j]);
            //     else 
            //         novaSeq.push_back(bloco3P1 ? seq1[j] : seq2[j]);
            // }


        }       

        cout << "\n\nPos cruzamento: ";
        for(const auto& element : novaSeq)
            cout << element << " ";
        cout << endl << endl;

        vector<bool> clienteUsado(instancia->numClientes + 1, false);
        vector<int> novaSeqSemDuplicatas;
        
        
        
        int clientesAtendidos = 0;

        for (int c : novaSeq) {
            if (c > 0 && c <= instancia->numClientes) {
                if (!clienteUsado[c]) {
                    clienteUsado[c] = true;
                    novaSeqSemDuplicatas.push_back(c);
                    clientesAtendidos++;
                }
            }
        }

        novaSeq.swap(novaSeqSemDuplicatas);

        // for(const auto& element : novaSeqSemDuplicatas)
        //     cout << element << " ";
        //     cout << endl;

        vector<int> clientesFaltantes;
        
        std::string sequenciaLocal;
        sequenciaLocal.reserve(instancia->numClientes * 3);

        for (int c = 1; c <= instancia->numClientes; ++c) {
            if (!clienteUsado[c]) {
                clientesFaltantes.push_back(c);
            }
        }


        for (int c : novaSeq) {
            sequenciaLocal += std::to_string(c);
            sequenciaLocal += ' ';
        }

        for (int c : clientesFaltantes) {
            sequenciaLocal += std::to_string(c);
            sequenciaLocal += ' ';
        }
        //    Reconstrução via GRASP
        Solucao::Solucao *novaSol = geraSolucao(
            instancia,
            alfa,
            vetorClienteBest,
            vetorClienteAux,
            &sequenciaLocal,
            // sequenciaStr,
            true,
            vetorCandidatos,
            *heuristica,
            vetorParametros,
            matrixClienteBest,
            tempoCriaRota,
            vetCandInteracoes,
            vetLimiteTempo
        );


    //    Se tiver usado veiculo ficticio, desconsidera sol e
    //    gera outra
        if (novaSol->veiculoFicticil) {
            delete novaSol;
            i--;
            continue;
        }

        populacaoInicial[i] = novaSol;
    }
// ordena candidatos baseado na poluicao
    std::sort(populacaoInicial, populacaoInicial+tamanhoPopulacao,[](const Solucao::Solucao* a, const Solucao::Solucao* b) {
              return a->poluicao < b->poluicao;
          });

    cout << "Menor poluicao: " << populacaoInicial[0]->poluicao;
}


Solucao::Solucao * Construtivo::grasp(const Instancia::Instancia *const instancia, float *vetorAlfa, int tamAlfa,
                                      const int numInteracoes,
                                      const int numIntAtualizarProb, bool log, stringstream *strLog,
                                      boost::tuple<int, int> *VetHeuristica,
                                      const int tamVetHeuristica, const double *const vetorParametros,
                                      Vnd::EstatisticaMv *vetEstatisticaMv,
                                      Solucao::ClienteRota **matrixClienteBest,
                                      Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
                                      GuardaCandInteracoes *vetCandInteracoes, double *vetLimiteTempo,
                                      Modelo::Modelo *modelo,
                                      Modelo_1_rota::Modelo *modelo1Rota, const Instancia::TimeType timeStart,
                                      double *ptr_tempoMip2rotas,
                                      u_int64_t *totalInteracoes, const int opcao, const double tempoMax,
                                      const double alvo, Alvo::Alvo *alvoTempos,
                                      list<EstatisticasQualidade> &listaEstQual)
{

    unordered_map<int, int> hash;

    Instancia::TimeType tempo_end;

    //Vetor guarda o resto da lista. São passados para construir a solução
    auto *vetorClienteBest = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetorClienteAux = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetClienteBestSecund = new Solucao::ClienteRota[instancia->numClientes+2];
    auto *vetClienteRotaSecundAux = new Solucao::ClienteRota[instancia->numClientes+2];
    int *guardaRota = new int[MaxTamVetClientesMatrix];
    int *guardaRota2 = new int[MaxTamVetClientesMatrix];
    int *vetRotasOpt = new int [instancia->numVeiculos];

    Solucao::ClienteRota* vetVetClienteRota[4] = {vetorClienteBest, vetorClienteAux, vetClienteBestSecund, vetClienteRotaSecundAux};
    int *vetGuardaRotas[2] = {guardaRota, guardaRota2};


    HashRotas::HashRotas hashRotas(instancia->numClientes);

    //Vetores para o reativo
    double *vetorProbabilidade = new double[tamAlfa];
    int *vetorFrequencia = new int[tamAlfa];
    double *solucaoAcumulada = new double[tamAlfa];
    double *vetorMedia = new double[tamAlfa];
    double *proporcao = new double[tamAlfa];
    Candidato *vetorCandidatos = new Candidato[instancia->numClientes];
    double somaProb, valAleatorio;
    int numSolInviaveis = 0;



    //Inicializa a melhor solução
    vetorFrequencia[0] = 1;

    int **matRotas = new int*[instancia->numVeiculos];

    for(int i = 0; i < instancia->numVeiculos; ++i)
        matRotas[i] = new int[instancia->numVeiculos];

    Solucao::Solucao *best = geraSolucao(instancia, vetorAlfa[0], vetorClienteBest, vetorClienteAux, nullptr, false,
                                         vetorCandidatos, VetHeuristica[0], vetorParametros, matrixClienteBest,
                                         tempoCriaRota, vetCandInteracoes, vetLimiteTempo);
    vetorProbabilidade[0] = 1.0/tamAlfa;
    solucaoAcumulada[0] = best->poluicao;
    vetorFrequencia[0] = 1;

    Solucao::Solucao *solucaoAux;
    int posicaoAlfa;
    u_int64_t ultimaAtualizacao = 0;
    double poluicaoUltima = -1.0;
    
    boost::tuple<int,int> heuristica(VetHeuristica[0]);
    int posicaoHeuristica = 0;
    u_int64_t ultimaAtualizacaoHeuristica = 0;

    //Inicializa os vetores de todos os alfas
    for(int i = 1; i < tamAlfa; ++i)
    {

        vetorProbabilidade[i] = 1.0/tamAlfa;
        vetorFrequencia[i] = 1;



        //Inicializa a solucaoAcumulada para o alfa
        solucaoAux = geraSolucao(instancia, vetorAlfa[i], vetorClienteBest, vetorClienteAux, nullptr, false,
                                 vetorCandidatos, heuristica, vetorParametros, matrixClienteBest, tempoCriaRota,
                                 vetCandInteracoes, vetLimiteTempo);
        solucaoAcumulada[i] = solucaoAux->poluicao + solucaoAux->poluicaoPenalidades;

        if(best->veiculoFicticil)
        {
            if(!solucaoAux->veiculoFicticil)
            {
                delete best;
                best = solucaoAux;
                solucaoAux = NULL;

                int num = int (ceil(best->poluicao));

                hash[num]++;

            }
            else
            {
                if((solucaoAux->poluicao + solucaoAux->poluicaoPenalidades) < (best->poluicao + best->poluicaoPenalidades))
                {
                    delete best;
                    best = solucaoAux;
                    solucaoAux = NULL;
                }
                else
                {
                    delete solucaoAux;
                    solucaoAux = NULL;
                }
            }
        }
        else
        {
            if(solucaoAux->veiculoFicticil)
            {
                delete solucaoAux;
                solucaoAux = NULL;
            }
            else
            {
                int num = int (ceil(solucaoAux->poluicao));

                hash[num]++;

                if(solucaoAux->poluicao  < best->poluicao)
                {
                    delete best;
                    best = solucaoAux;
                    solucaoAux = NULL;

                }
                else
                {
                    delete solucaoAux;
                    solucaoAux = NULL;
                }
            }
        }


    }

    std::string texto;
    std::string sequencia;

    int tentativasViabilizar = 0;
    double tempoConstrutivo = 0.0;
    double tempoVnd = 0.0;
    double tempoViabilizador = 0.0;
    int numChamadasModelo2Rotas = 0;
    double tempoModelo2Rotas = 0.0;
    u_int64_t ultimaAtualizacaoGrasp = 0;

    auto c_start = std::chrono::high_resolution_clock::now();
    auto construtivo_c_start = std::chrono::high_resolution_clock::now();
    auto c_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempoCpu;

    double poluicaoBestHeuristica;

    if(!best->veiculoFicticil)
    {
        poluicaoBestHeuristica = best->poluicao;
    }

    bool numMaxAtualizacoes = false;
    int interacoes = 0;

    u_int64_t i = 0;

    //__int128 a;
    
    tempo_end = std::chrono::high_resolution_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::seconds>(tempo_end - timeStart);

    double tempo = time.count();
    bool segundaFase = false;
    u_int64_t inicioSegundaFase;
    u_int64_t interacoesIls = 0, ultimaAtualizacaoIls = 0;

    if(!best->veiculoFicticil)
    {
        if(alvoTempos)
            alvoTempos->novaSolucao(best->poluicao);

    }

    //while(tempo < instancia->tempoCpuPermitido)
    while(i < numInteracoes)
    {

        u_int64_t diferenca = i - ultimaAtualizacao;
        u_int64_t diferenca2 = i - ultimaAtualizacaoGrasp;


        if (((diferenca >= 400) && (!best->veiculoFicticil) && i > 1400))
        {
            break;
        }




        //if((diferenca % 100) == 0 && (diferenca > 0) && !best->veiculoFicticil && modelo)
        if(!best->veiculoFicticil && (i == 1000) && (opcao == OpcaoGraspIlsMip || opcao ==OpcaoGraspMip))
        {
            bool condicao = true;

            if(condicao)
            {

                    numChamadasModelo2Rotas += 1;

                    double antes = best->poluicao;

                    auto start = std::chrono::high_resolution_clock::now();
                    try
                    {
                        #if Debug
                            cout << "Inicio Mip i 1000 *****\n";
                        #endif

                        Modelo2Rotas::geraRotas_comb_2Rotas(best, modelo, vetorClienteBest, vetClienteBestSecund,
                                                            instancia, &hashRotas, guardaRota, matRotas, guardaRota2,
                                                            0);

                        #if Debug
                            cout << "Fim Mip i 1000 *****\n";
                        #endif

                    }
                    catch (GRBException e)
                    {
                        cout << "code: " << e.getErrorCode() << "\nMessage: " << e.getMessage() << "\n";
                        exit(-1);
                    }

                    auto end = std::chrono::high_resolution_clock::now();

                    auto tempo_ = std::chrono::duration_cast<std::chrono::seconds>(end - start);

                    tempoModelo2Rotas += tempo_.count();

                    if (best->poluicao < antes)
                    {
                        #if DEBUG
                            cout << "Atualizacao mip 2 rotas interacao: " << i << "\n";
                        #endif

                        ultimaAtualizacao = i;
                        listaEstQual.push_back(EstatisticasQualidade(best->poluicao, i, true, construtivo_c_start));

                        if(alvoTempos)
                            alvoTempos->novaSolucao(best->poluicao);


                    }



            }
        }

        if(!best->veiculoFicticil && (i == 1000) && opcao == OpcaoGraspComIlsMip)
        {

            interacoesIls = 0;
            ultimaAtualizacaoIls = 0;

            #if Debug
            cout<<"ILS i 1000\n";
            #endif

            Ils::ils(instancia, &best, 200, 2, 2 * 60, opcao, vetVetClienteRota, &hashRotas, vetGuardaRotas,
                     vetEstatisticaMv, vetLimiteTempo, matRotas, modelo1Rota, modelo, &tempoModelo2Rotas,
                     &interacoesIls, &ultimaAtualizacaoIls, NULL, NULL, NULL, NULL, NULL, alvo, alvoTempos,
                     listaEstQual, 0, Ils::Parametros());



            if(ultimaAtualizacaoIls > 0)
            {
                ultimaAtualizacao = i;

                if(alvoTempos)
                    alvoTempos->novaSolucao(best->poluicao);
            }

        }


        tempo_end = std::chrono::high_resolution_clock::now();
        time =  std::chrono::duration_cast<std::chrono::seconds>(tempo_end - timeStart);
        tempo = time.count();

        if(tempo > tempoMax)
            break;

        #if DEBUG

            if((i % 100) == 0)
                cout<<"Interacao "<<i<<'\n';

        #endif

        //Atualiza probabilidade
        if(((i%numIntAtualizarProb) == 0) && (i > 0) || (i == 50))
        {

            atualizaProbabilidade(vetorProbabilidade, vetorFrequencia, solucaoAcumulada, vetorMedia, proporcao, tamAlfa, best->poluicao);

        }

/*        if((i == numInteracoes/2) && usarDuasHeur)
        {

            double taxa = numSolInviaveis/double(numInteracoes/2);

            if(taxa >= 0.6)
            {
                heuristica1 = false;

            }


        }*/

        if((i - ultimaAtualizacaoHeuristica) == 150)
        {

            posicaoHeuristica++;
            posicaoHeuristica %= tamVetHeuristica;

            heuristica = VetHeuristica[posicaoHeuristica];
            ultimaAtualizacaoHeuristica = i;


        }


        //Escolher alfa, gerar solução, atualizar vetores, ....
        somaProb = posicaoAlfa = 0;

        valAleatorio = rand_u32() % 100;

        for(int j=0;somaProb < valAleatorio; ++j)
        {

            if(j >= tamAlfa)
            {

                j = 0;


            }

            somaProb+= (100.0 * vetorProbabilidade[j]);
            posicaoAlfa = j;

        }


        //Cria solução com o alfa escolhido

        sequencia = "";


        c_start = std::chrono::high_resolution_clock::now();

        solucaoAux = geraSolucao(instancia, vetorAlfa[posicaoAlfa], vetorClienteBest, vetorClienteAux, &sequencia, log, vetorCandidatos,
                                 heuristica, vetorParametros, matrixClienteBest, tempoCriaRota, vetCandInteracoes, vetLimiteTempo);

        c_end = std::chrono::high_resolution_clock::now();

        tempoCpu = c_end - c_start;

        tempoConstrutivo += tempoCpu.count();


        bool inviavel = false, mip = false;

        if(solucaoAux->veiculoFicticil)
            inviavel = true;

        double poluicaoHeuriAux;
        instancia->atualizaPoluicaoSolucao(solucaoAux);



            c_start = std::chrono::high_resolution_clock::now();

            if(!modelo || opcao == OpcaoGrasp || opcao == OpcaoGraspIls)
            {

                Vnd::vnd(instancia, solucaoAux, vetorClienteBest, vetorClienteAux, false, vetClienteBestSecund,
                         vetClienteRotaSecundAux, i, vetEstatisticaMv, vetLimiteTempo, NULL, NULL, guardaRota,
                         guardaRota2, -1);
            }
            else
            {

                Vnd::vnd(instancia, solucaoAux, vetorClienteBest, vetorClienteAux, false, vetClienteBestSecund,
                         vetClienteRotaSecundAux, i, vetEstatisticaMv, vetLimiteTempo, NULL, &hashRotas, guardaRota,
                         guardaRota2, -1);
            }

#           if Debug
            if(solucaoAux->poluicao - best->poluicao < -0.001 && (!solucaoAux->veiculoFicticil))
                cout<<"Atualizacao vnd i "<<i<<'\n';

#           endif

            c_end = std::chrono::high_resolution_clock::now();

            tempoCpu = c_end - c_start;
            tempoVnd  += tempoCpu.count();


            poluicaoHeuriAux = 0.0;

            HashRotas::HashNo *hashNo;
            Solucao::ClienteRota *cliente;
            double aux, aux2;

            if(!solucaoAux->veiculoFicticil && modelo && (opcao == OpcaoGraspMip || opcao == OpcaoGraspIlsMip || opcao == OpcaoGraspComIlsMip))
            {

                //Soma a poluico heuristica da solucao
                for (auto veiclulo : solucaoAux->vetorVeiculos)
                {
                    cliente = (*veiclulo->listaClientes.begin());



                    if (!cliente->rotaMip)
                        poluicaoHeuriAux += veiclulo->poluicao;
                    else
                    {
                        if(veiclulo->listaClientes.size() <= 2)
                        {
                            continue;
                        }

                        //Recupera o veiculo na hash
                        hashNo = hashRotas.getVeiculo(veiclulo);

                        if (!hashNo)
                        {
                            Solucao::ClienteRota *ptrClienterota = vetorClienteBest;

                            cout << "Erro, rotaMip e a rota nao esta na hash.\n";
                            for(auto cliente : veiclulo->listaClientes)
                            {
                                cout << cliente->cliente << ' ';
                                ptrClienterota->cliente = cliente->cliente;
                                ++ptrClienterota;
                            }
                            cout<<'\n';

                            bool r = hashRotas.getVeiculo(vetorClienteBest, veiclulo->listaClientes.size(), veiclulo->tipo, &aux, &aux2);
                            if(r)
                                cout<<"encontrou\n??";
                            else
                                cout<<"nao encontrou\n";

                            exit(-1);
                        }

                        if (hashNo->poluicaoH > 0.0)
                            poluicaoHeuriAux += hashNo->poluicaoH;
                        else
                        {
                            //Copia a rota para vetor

                            Solucao::ClienteRota *clienteRota_ptr = vetorClienteBest;

                            for (auto cliente : veiclulo->listaClientes)
                            {
                                clienteRota_ptr->cliente = cliente->cliente;

                                ++clienteRota_ptr;
                            }

                            bool resultado = Movimentos_Paradas::criaRota(instancia, vetorClienteBest,
                                                                          veiclulo->listaClientes.size(),
                                                                          veiclulo->carga, veiclulo->tipo, &aux,
                                                                          &aux2, NULL, NULL, vetLimiteTempo,
                                                                          vetClienteRotaSecundAux, NULL);

                            if (!resultado)
                            {
                                cout << "Nao existe solucao heuristica\n";
                                hashNo->poluicaoH = veiclulo->poluicao + 0.15 * veiclulo->poluicao;

                            } else
                                hashNo->poluicaoH = aux2;

                            poluicaoHeuriAux += hashNo->poluicaoH;
                        }
                    }

                }

            }


            if((!solucaoAux->veiculoFicticil) && (modelo) && ((opcao == OpcaoGraspMip || opcao == OpcaoGraspIlsMip || opcao == OpcaoGraspComIlsMip) && ( (i - ultimaAtualizacao) > 300 &&
                    (solucaoAux->poluicao - best->poluicao < -0.001))) && i > 1000)
            {

                double gap = (poluicaoHeuriAux - poluicaoBestHeuristica)/poluicaoBestHeuristica;

                static bool mip = false;

                if(!mip && ((i-ultimaAtualizacao)%100==0))
                    mip = true;


                if(gap < 0.15 && mip)
                {
                    const static int p = 100 - 40;

                    int valA = rand_u32() % 100;

                    if ((p <= valA) || (best->veiculoFicticil) || (poluicaoHeuriAux < poluicaoBestHeuristica) || (gap <= 0.1))
                    {
                        mip = false;
                        #if Debug

                        cout<<"Grasp MIP i: "<<i<<'\n';

                        #endif


                        Modelo_1_rota::geraRotasOtimas(solucaoAux, modelo1Rota, vetorClienteAux, instancia, &hashRotas, guardaRota);
                        mip = true;

                        if(opcao == OpcaoGraspMip)
                        {

#                           if Debug

                            cout<<"Grasp MIP 2 rotas i "<<i<<'\n';

#                           endif

                            auto start = std::chrono::high_resolution_clock::now();
                            try
                            {
                                Modelo2Rotas::geraRotas_comb_2Rotas(solucaoAux, modelo, vetorClienteBest,
                                                                    vetClienteBestSecund, instancia, &hashRotas,
                                                                    guardaRota, matRotas, guardaRota2, 0);

                            }
                            catch (GRBException e)
                            {
                                cout << "code: " << e.getErrorCode() << "\nMessage: " << e.getMessage() << "\n";
                                exit(-1);
                            }

                            auto end = std::chrono::high_resolution_clock::now();

                            auto tempo_ = std::chrono::duration_cast<std::chrono::seconds>(end - start);

#                           if Debug
                            if(solucaoAux->poluicao - best->poluicao < -0.001)
                                cout<<"Atualizacao MIP 2 rotas i "<<i<<'\n';

#                           endif

                            tempoModelo2Rotas += tempo_.count();
                        }
                        else
                        {


#                           if Debug

                            cout<<"Grasp MIP ILS i "<<i<<'\n';

#                           endif

                            Ils::ils(instancia, &solucaoAux, 200, 2, 2 * 60, opcao, vetVetClienteRota, &hashRotas,
                                     vetGuardaRotas,
                                     vetEstatisticaMv, vetLimiteTempo, matRotas, modelo1Rota, modelo,
                                     &tempoModelo2Rotas, &interacoesIls, &ultimaAtualizacaoIls, NULL, NULL, NULL, NULL,
                                     NULL, alvo, alvoTempos, listaEstQual, 0, Ils::Parametros());


#                           if Debug
                            if(solucaoAux->poluicao - best->poluicao < -0.001)
                                cout<<"Atualizacao ILS i "<<i<<'\n';

#                           endif


                        }


                    }
                }

            }

            if(!solucaoAux->veiculoFicticil)
            {
                solucaoAcumulada[posicaoAlfa] += poluicaoHeuriAux;
                vetorFrequencia[posicaoAlfa] += 1;
            }
            else
            {
                solucaoAcumulada[posicaoAlfa] += solucaoAux->poluicao + solucaoAux->poluicaoPenalidades;
                vetorFrequencia[posicaoAlfa] += 1;
            }


        if(solucaoAux->veiculoFicticil)
            numSolInviaveis += 1;
        else if(inviavel)
        {
            ++tentativasViabilizar;
        }
            if(log)
            {

                texto+="***************************************************************************\n\n";

                texto += "Poluicao: " + to_string(solucaoAux->poluicao) + "\n";
                texto += "Interacao: " + to_string(i) + '\n';

                texto += "Sequencia: " + sequencia + "\nRota:\n\n";

                for(auto veiculo:solucaoAux->vetorVeiculos)
                {

                    for(auto it:(*veiculo).listaClientes)
                    {

                        texto+= to_string((*it).cliente) + " ";

                    }
                    texto+= "\n";

                }

                texto+="\n***************************************************************************\n";

            }


        //Atualiza best
        if(best->veiculoFicticil)
        {
            if(!solucaoAux->veiculoFicticil)
            {
                delete best;
                best = solucaoAux;
                solucaoAux = NULL;
                ultimaAtualizacaoGrasp = ultimaAtualizacao = i;
                poluicaoUltima = best->poluicao;

                listaEstQual.push_back(EstatisticasQualidade(best->poluicao, i, mip, construtivo_c_start));

                poluicaoBestHeuristica = poluicaoHeuriAux;

                if(alvoTempos)
                    alvoTempos->novaSolucao(best->poluicao);


            }
            else
            {
                if((solucaoAux->poluicao + solucaoAux->poluicaoPenalidades) < (best->poluicao + best->poluicaoPenalidades))
                {
                    delete best;
                    best = solucaoAux;
                    solucaoAux = NULL;
                    poluicaoBestHeuristica = poluicaoHeuriAux;

                }
                else
                {
                    delete solucaoAux;
                    solucaoAux = NULL;
                }
            }
        }
        else
        {
            if(solucaoAux->veiculoFicticil)
            {
                delete solucaoAux;
                solucaoAux = NULL;
            }
            else
            {
                double diferenca = (solucaoAux->poluicao - best->poluicao);
                if( diferenca < -0.001)
                {

                    delete best;
                    best = solucaoAux;

                    listaEstQual.push_back(EstatisticasQualidade(best->poluicao, i, mip, construtivo_c_start));

                    solucaoAux = NULL;
                    ultimaAtualizacaoGrasp = ultimaAtualizacao = i;
                    poluicaoUltima = best->poluicao;

                    ultimaAtualizacaoHeuristica = i;
                    poluicaoBestHeuristica = poluicaoHeuriAux;

                    #if DEBUG
                        cout<<"Atualizacao Grasp, interacao: "<<i<<"\n";
                    #endif
                    if(alvoTempos)
                        alvoTempos->novaSolucao(best->poluicao);

                }
                else
                {
                    delete solucaoAux;
                    solucaoAux = NULL;
                }
            }
        }

        ++i;

        tempo_end = std::chrono::high_resolution_clock::now();
        time =  std::chrono::duration_cast<std::chrono::seconds>(tempo_end - timeStart);
        tempo = time.count();

        if(alvoTempos && (tempo > tempoMax || alvoTempos->antingilTodosAlvos()))
        {


            break;

        }

    }


    //std::cout<<"Numero de solucoes inviaveis: "<<numSolInviaveis<<'\n';

/*    float tamanhoMedio;
    int maior;

    hashRotas.estatisticasHash(&tamanhoMedio, &maior);
    cout<<"Estatisticas hash: \nTamnho medio: "<<tamanhoMedio<<"\nMaior: "<<maior<<'\n';*/

    /*

    if((best->poluicao > alvo) && tempo < tempoMax  && !best->veiculoFicticil && (opcao == OpcaoGraspMip || opcao==OpcaoGraspIlsMip || opcao == OpcaoGraspComIlsMip))
    {
        double poluicao = best->poluicao;
        Modelo2Rotas::geraRotas_comb_2Rotas(best, modelo, vetorClienteBest, vetClienteBestSecund, instancia, &hashRotas, guardaRota, matRotas, guardaRota2);

        if(best->poluicao + 1e-3 < poluicao)
        {
            ultimaAtualizacao = i;
        }

    }
     */

    //ILS Condicao de parada : limite de tempo do grasp


    /*

    auto tempoStartIls = std::chrono::high_resolution_clock::now();
    auto tempoEndIls   = std::chrono::high_resolution_clock::now();

    auto timeIls = std::chrono::duration_cast<std::chrono::seconds>(tempoEndIls - tempoStartIls);

    interacoesIls = 0;
    ultimaAtualizacaoIls = 0;

    if((best->poluicao > alvo) && tempo < tempoMax && (opcao == OpcaoGraspIlsMip || opcao == OpcaoGraspComIlsMip) && !best->veiculoFicticil)
    {
        Ils::ils(instancia, &best, 200, 2, 2*60, opcao, vetVetClienteRota, &hashRotas, vetGuardaRotas,
                 vetEstatisticaMv, vetLimiteTempo, matRotas, modelo1Rota, modelo, &tempoModelo2Rotas, &interacoesIls, &ultimaAtualizacaoIls, NULL, NULL, NULL, NULL, NULL, alvo);
    }


    if(ultimaAtualizacaoIls > 0)
        ultimaAtualizacao = i + ultimaAtualizacaoIls;

    double antes = best->poluicao;

    if(opcao == OpcaoGraspMip && (best->poluicao > alvo) && tempo < tempoMax)
    {

        Modelo2Rotas::geraRotas_comb_2Rotas(best, modelo, vetorClienteBest, vetClienteBestSecund, instancia, &hashRotas,
                                            guardaRota, matRotas, guardaRota2);

        if (best->poluicao < antes)
            ultimaAtualizacao = interacoesIls + i;
    }
    */

    *ptr_tempoMip2rotas = tempoModelo2Rotas;

    //Libera memória
    delete []vetorClienteBest;
    delete []vetorClienteAux;
    delete []vetClienteBestSecund;
    delete []vetorProbabilidade;
    delete []vetorFrequencia;
    delete []solucaoAcumulada;
    delete []vetorMedia;
    delete []proporcao;
    delete []vetClienteRotaSecundAux;
    delete []vetorCandidatos;
    delete []vetRotasOpt;

    delete []guardaRota;
    delete []guardaRota2;




    for(int i = 0; i < instancia->numVeiculos; ++i)
        delete []matRotas[i];

    *totalInteracoes = i;

    delete []matRotas;

    #if DEBUG

        cout<<"Numero de chamadas Mip duas rotas: "<<numChamadasModelo2Rotas<<'\n'<<"Tempo Mip duas rotas: "<<tempoModelo2Rotas<<"\n";

    #endif

    proporcao = NULL;
    vetorMedia = NULL;
    solucaoAcumulada = NULL;
    vetorFrequencia = NULL;
    vetorProbabilidade = NULL;
    vetorClienteAux = NULL;
    vetorClienteBest = NULL;
    vetClienteBestSecund = NULL;
    /*
    cout<<"Ultima Atualizacao: "<<ultimaAtualizacao<<'\n';
    cout<<"Poluicao: "<<poluicaoUltima<<'\n';
    cout<<"Veiculo Ficticio: "<<best->veiculoFicticil<<'\n';*/

    best->numSolucoesInv = numSolInviaveis;
    best->ultimaAtualizacao = ultimaAtualizacao;
    best->solucoesViabilizadas = tentativasViabilizar;
    best->tempoVnd = tempoVnd;
    best->tempoViabilizador = tempoViabilizador;
    best->tempoConstrutivo = tempoConstrutivo;

    if(log)
    {
        (*strLog) << texto;


        (*strLog) << "\n\nPoluicao\tFrequencia\n";

        for (auto it : hash)
        {
            (*strLog) << it.first << "\t\t\t" << it.second << '\n';
        }
        (*strLog) << "\n\n";

    }
    return best;
}

void Construtivo::atualizaProbabilidade(double *vetorProbabilidade, int *vetorFrequencia, double *solucaoAcumulada, double *vetorMedia, double *proporcao, int tam, double melhorSolucao)
{

    double somaProporcoes = 0.0;

    //Calcular média

    for(int i = 0; i < tam; ++i)
    {
        vetorMedia[i] = solucaoAcumulada[i]/double(vetorFrequencia[i]);


    }


    //Calcula proporção.
    for(int i = 0; i < tam; ++i)
    {
        proporcao[i] = melhorSolucao/vetorMedia[i];
        somaProporcoes += proporcao[i];
    }


    //Calcula probabilidade
    for(int i = 0; i<tam; ++i)
    {
        vetorProbabilidade[i] = proporcao[i]/somaProporcoes;
    }


}

void Construtivo::atualizaPesos(double *beta, double *teta, int numClientes, const int k, double *gama, const int Heuristica, const double *const vetorParametros)
{
    if((Heuristica == Heuristica_0) || (Heuristica == Heuristica_3))
    {
        *teta = abs(sin(2 * M_PI * (k) / (vetorParametros[Heuristica] * numClientes)));
        *beta = 1 - *teta;
    }
    else if((Heuristica == Heuristica_1) || (Heuristica == Heuristica_4) || (Heuristica == Heuristica_5) || (Heuristica == Heuristica_6))
    {

        static int i = 0, j = 0;
        static double F;
        static const int max = int(numClientes);

        F = numClientes * vetorParametros[Heuristica];

        if(numClientes == max)
            i = j = 0;

        *beta = abs(sin(2.0 * M_PI * i) / F);
        *teta = (1-*beta) * abs(sin(2.0*M_PI*j)/F);
        *gama = 1.0 - *beta - *teta;

        if(j+1 < max)
            j += 1;
        else
        {
            j = 0;
            if(i+1 < max)
                i += 1;
            else
                i = 0;
        }
    }

}

Solucao::Solucao * Construtivo::geraSolucao(const Instancia::Instancia *const instancia, float alfa,
                                            Solucao::ClienteRota *vetorClienteBest,
                                            Solucao::ClienteRota *vetorClienteAux, string *sequencia, bool log,
                                            Construtivo::Candidato *vetorCandidatos, boost::tuple<int, int> heuristica,
                                            const double *const vetorParametros,
                                            Solucao::ClienteRota **matrixClienteBest,
                                            Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
                                            GuardaCandInteracoes *vetCandInteracoes, double *vetLimiteTempo)
{

    string texto;

    //Inicializa a lista de candidatos(Clientes)
    list<Instancia::Cliente> listaCandidatos;



    for (int i = 0; i < instancia->numClientes; ++i)
    {

        if (instancia->vetorClientes[i].cliente != 0)
        {
            listaCandidatos.push_back(instancia->vetorClientes[i]);
            vetCandInteracoes[i].valido = false;
        }
    }




    //Cria uma solução com o mínimo de veiculos
    int minVeiculos = instancia->numVeiculos;
    Solucao::Solucao *solucao = new Solucao::Solucao(minVeiculos);
    double beta, teta, gama;
    int numClientesSol = 0;

    Solucao::Veiculo *melhorVeiculo;
    bool nullMelhorVeiculo = true;
    std::list<Solucao::ClienteRota *>::iterator melhorPosicao, posicaoAux;
    double melhorPoluicao, melhorCombustivel, auxPoluicao, auxCombustivel;
    double folgaRota, folgaRotaAux;
    double distRotaBest;

    double bestDistanciaRotaCompleta;
    double auxDistanciaRotaCompleta;

    auto *candidato = new Solucao::ClienteRota;
    Instancia::Cliente clienteAux;

    Solucao::ClienteRota *vetorClienteSwap; //Ponteiro para trocar os vetores.

    int tamVetBest, tamVetAux, posicaoVetor;

    double combustivelParcial, poluicaoParcial, combAux, polAux; //Acumula combustivel e poluicao da rota parcial
    auto clienteItAux = solucao->vetorVeiculos[0]->listaClientes.begin();
    bool viavel;
    uint32_t tamLista2Crit;
    uint32_t escolhido;
    int b;

    /* *******************************************************************************************************************************
     *
     * Enquanto a lista de candidatos for diferente de vazio, escolha um cliente, calcule o acrescimo de poluição para cada
     * posição possível da solução, desde que a solução seja viável.
     *
     **********************************************************************************************************************************/

    //vetorCandidatos marca inicio

    Candidato *ultimo = NULL;

    bool incrementarIt = true;
    int ultimoVeiculoAtualizado = -1;

    while (!listaCandidatos.empty())
    {

        if(heuristica.get<0>() != 2)
            atualizaPesos(&beta, &teta, instancia->numClientes, numClientesSol, &gama, heuristica.get<0>(), vetorParametros);

        numClientesSol += 1;


        //Pega a melhor solucao de cada candidato.
        for (auto iteratorLisCand = listaCandidatos.begin(); iteratorLisCand != listaCandidatos.end(); )
        {
            incrementarIt = true;
            int indiceVeiculo = 0;


            melhorPoluicao = HUGE_VALF;
            folgaRota = HUGE_VALF;
            nullMelhorVeiculo = true;

            clienteAux = (*iteratorLisCand);

            candidato->cliente = clienteAux.cliente;

            bool somentePercorreUmVeiculo = false;

            if( ((ultimoVeiculoAtualizado == vetCandInteracoes[candidato->cliente].indiceVeiculo) && (heuristica.get<0>() == Heuristica_2) )|| (ultimoVeiculoAtualizado == -1))
                vetCandInteracoes[candidato->cliente].valido = false;
            else
            {
                if(vetCandInteracoes[candidato->cliente].valido && (heuristica.get<0>() == Heuristica_2))
                {
                    /*cout<<"Linha: "<<__LINE__<<"\n**********************************\n!!Cliente "<<candidato->cliente<<" "<<"com solucao da interacao anterior\n\n";
                    cout<<"Solucao: ";
                    auto ptr = matrixClienteBest[candidato->cliente];

                    for(int p = 0; p < vetCandInteracoes[candidato->cliente].tam; ++p)
                        cout<<ptr[p].cliente<<" ";
                    cout<<"\n\n**********************************\n";*/



                    somentePercorreUmVeiculo = true;
                    indiceVeiculo = ultimoVeiculoAtualizado;
                    nullMelhorVeiculo = false;
                    //melhorVeiculo = solucao->vetorVeiculos[vetCandInteracoes[candidato->cliente].indiceVeiculo];
                    melhorVeiculo = vetCandInteracoes[candidato->cliente].veiculo;
                    melhorPoluicao = vetCandInteracoes[candidato->cliente].poluicao;
                    melhorCombustivel = vetCandInteracoes[candidato->cliente].combustivel;
                    tamVetBest = vetCandInteracoes[candidato->cliente].tam;

                    melhorPosicao = vetCandInteracoes[candidato->cliente].it;
                    folgaRota = vetCandInteracoes[candidato->cliente].folga;
                    bestDistanciaRotaCompleta = vetCandInteracoes[candidato->cliente].distanciaRotaCompleta;
                }
            }

            if (log)
                (*sequencia) += to_string(candidato->cliente) + ' ';

            texto += std::to_string(candidato->cliente) + " ";

            //Percorre os veículos
            for(auto veiculo = std::next(solucao->vetorVeiculos.begin(), indiceVeiculo); veiculo != solucao->vetorVeiculos.end(); ++veiculo, ++indiceVeiculo)
            {

                if ((*veiculo)->tipo == 2)
                    break;

                //Verifica se a capacidade max será atingida
                if ((*veiculo)->carga + clienteAux.demanda > instancia->vetorVeiculos->capacidade)
                {
                    if(somentePercorreUmVeiculo)
                        break;
                    else
                        continue;

                }

                //Variaveis para armazenar a soma do combustivel e da poluicao do veiculo até clienteIt
                poluicaoParcial = 0.0;
                combustivelParcial = 0.0;
                posicaoVetor = 0;
                const int peso = (*veiculo)->carga + clienteAux.demanda;
                vetorClienteAux[0] = **((*veiculo)->listaClientes.begin());


                //Percorrer os clientes.
                //Candidato tentara ser inserido após clienteIt

                for (auto clienteIt = (*veiculo)->listaClientes.begin(); clienteIt != (*veiculo)->listaClientes.end();)
                {

                    double aux = folgaRotaAux;
                    double disTemp = -HUGE_VAL;
                    tie(viavel, tamVetAux, auxPoluicao, auxCombustivel) = viabilidadeInserirCandidato(
                            vetorClienteAux,
                            clienteIt,
                            instancia,
                            candidato,
                            combustivelParcial,
                            poluicaoParcial,
                            *veiculo, peso,
                            posicaoVetor,
                            &aux, tempoCriaRota, vetLimiteTempo, vetorClienteBest);

                    if (viavel)
                    {

                        auxDistanciaRotaCompleta = vetorClienteAux[tamVetAux-1].distanciaAteCliente;

                        for (int i = 0; i + 2 < tamVetAux; ++i)
                        {
                            double dist = instancia->matrizDistancias[vetorClienteAux[i].cliente][vetorClienteAux[i + 1].cliente];
                            dist += instancia->matrizDistancias[vetorClienteAux[i + 1].cliente][vetorClienteAux[i+2].cliente];

                            if (dist > disTemp)
                                disTemp = dist;
                        }


                        if (nullMelhorVeiculo)
                        {

                            nullMelhorVeiculo = false;
                            melhorVeiculo = (*veiculo);
                            melhorPoluicao = auxPoluicao;
                            melhorCombustivel = auxCombustivel;
                            tamVetBest = tamVetAux;
                            melhorPosicao = clienteIt;

                            folgaRota = aux;



                            distRotaBest = disTemp;

                            bestDistanciaRotaCompleta = auxDistanciaRotaCompleta;

/*                            //trocar vetores
                            vetorClienteSwap = vetorClienteBest;
                            vetorClienteBest = vetorClienteAux;
                            vetorClienteAux = vetorClienteSwap;

                            for (int i = 0; i <= posicaoVetor; ++i)
                                vetorClienteAux[i] = vetorClienteBest[i];*/

                            if((posicaoVetor + 1) > MaxTamVetClientesMatrix)
                            {
                                cout<<"Erro, tamanho do vetor MaxTamVetClientesMatrix[x] eh insuficientente\n";
                                exit(-1);
                            }

                            auto ptrVetMat = matrixClienteBest[candidato->cliente];
                            auto ptrVetCliente = vetorClienteAux;

                            for (int i = 0; i < (*veiculo)->listaClientes.size() + 1; ++i)
                            {
                                *ptrVetMat = *ptrVetCliente;

                                ++ptrVetMat;
                                ++ptrVetCliente;
                            }

                            if(heuristica.get<0>() == Heuristica_2)
                            {
                                vetCandInteracoes[candidato->cliente].valido = true;
                                vetCandInteracoes[candidato->cliente].indiceVeiculo = (*veiculo)->id;
                                vetCandInteracoes[candidato->cliente].combustivel = melhorCombustivel;
                                vetCandInteracoes[candidato->cliente].poluicao = melhorPoluicao;
                                vetCandInteracoes[candidato->cliente].distanciaRotaCompleta = bestDistanciaRotaCompleta;
                                vetCandInteracoes[candidato->cliente].tam = (*veiculo)->listaClientes.size() + 1;
                                vetCandInteracoes[candidato->cliente].it = melhorPosicao;
                                vetCandInteracoes[candidato->cliente].veiculo = (*veiculo);


                            }




                        }
                        else
                        {
                            //Variaveis auxiliares de folga e poluicao

                            double folgaSolParcial;    //Armazena a folga do veiculo da solucao parcial.
                            double folgaBest = folgaRota;

                            double distParcial;
                            double distBest;

                            if(heuristica.get<0>() == Heuristica_1)
                            {
                                distParcial = disTemp;
                                distBest = distRotaBest;
                            }

                            folgaSolParcial = aux;


                            //Valor que eh incrementado na funcao objetivo
                            double incAtual = (auxPoluicao - (*veiculo)->poluicao);
                            double incBest = (melhorPoluicao - (*melhorVeiculo).poluicao);
                            double distRotaCompletaAtual = auxDistanciaRotaCompleta;
                            double distRotaCompletaBest = bestDistanciaRotaCompleta;

                            double normaAtual;
                            double normaBest;

                            if(heuristica.get<0>() == Heuristica_0)
                            {
                                //Normaliza os valores
                                normaAtual = pow(folgaSolParcial, 2) + pow(incAtual, 2);
                                normaBest = pow(folgaBest, 2) + pow(incBest, 2);

                            }
                            else if(heuristica.get<0>() == Heuristica_1)
                            {
                                normaAtual = pow(folgaSolParcial,2) + pow(incAtual, 2) + pow(distParcial, 2);
                                normaBest =  pow(folgaBest,2) + pow(incBest, 2) + pow(distBest, 2);
                            }
                            else if(heuristica.get<0>() == Heuristica_3)
                            {
                                normaAtual = pow(distRotaCompletaAtual, 2) + pow(incAtual, 2);
                                normaBest = pow(distRotaCompletaBest, 2) + pow(incBest, 2);
                            }
                            else if((heuristica.get<0>() == Heuristica_4) || (heuristica.get<0>() == Heuristica_5) || (heuristica.get<0>() == Heuristica_6))
                            {
                                normaAtual = pow(distRotaCompletaAtual, 2) + pow(incAtual, 2) + pow(folgaSolParcial, 2);
                                normaBest = pow(distRotaCompletaBest, 2) + pow(incBest, 2) + pow(folgaBest,2);
                            }

                            if((heuristica.get<0>() == Heuristica_0) || ( heuristica.get<0>() == Heuristica_1) || (heuristica.get<0>() == Heuristica_3) ||
                               (heuristica.get<0>() == Heuristica_4) || (heuristica.get<0>() == Heuristica_5) || (heuristica.get<0>() == Heuristica_6))
                            {
                                folgaSolParcial /= normaAtual;
                                incAtual /= normaAtual;
                                distRotaCompletaAtual /= normaAtual;

                                folgaBest /= normaBest;
                                incBest /= normaBest;
                                distRotaCompletaBest /= normaBest;

                                if (heuristica.get<0>() == Heuristica_1)
                                {
                                    distParcial /= normaAtual;
                                    distBest /= normaBest;
                                }
                            }

                            bool condicao = false;

                            if(heuristica.get<0>() == Heuristica_0)
                                condicao = ((beta*incAtual + teta*(1.0/folgaSolParcial)) < (beta*incBest + teta*(1.0/folgaBest) ));

                            else if(heuristica.get<0>() == Heuristica_1)
                                condicao = ((beta*incAtual + teta*(1.0/folgaSolParcial) + gama*distParcial) < (beta*incBest + teta*(1.0/folgaBest) + gama*distBest));

                            else if(heuristica.get<0>() == Heuristica_2)
                                condicao = (auxDistanciaRotaCompleta < bestDistanciaRotaCompleta);

                            else if(heuristica.get<0>() == Heuristica_3)
                                condicao = ((beta*incAtual + teta*(distRotaCompletaAtual)) < (beta*incBest + teta*(distRotaCompletaBest)));

                            else if((heuristica.get<0>() ==Heuristica_4) || (heuristica.get<0>() == Heuristica_5) || (heuristica.get<0>() == Heuristica_6))
                                condicao = ((beta*incAtual + gama*(distRotaCompletaAtual) + teta*(1.0/folgaSolParcial)) < (beta*incBest + gama*(distRotaCompletaBest) + teta*(1.0/folgaBest)));

                            if(condicao)
                            {

                                melhorVeiculo = (*veiculo);
                                melhorPoluicao = auxPoluicao;
                                melhorCombustivel = auxCombustivel;
                                tamVetBest = tamVetAux;
                                melhorPosicao = clienteIt;
                                bestDistanciaRotaCompleta = auxDistanciaRotaCompleta;


/*                                distRotaBest = distParcial;
                                folgaRota = folgaSolParcial;
                                //trocar vetores
                                vetorClienteSwap = vetorClienteBest;
                                vetorClienteBest = vetorClienteAux;
                                vetorClienteAux = vetorClienteSwap;

                                for (int i = 0; i <= posicaoVetor; ++i)
                                    vetorClienteAux[i] = vetorClienteBest[i];*/


                                if((posicaoVetor + 1) > MaxTamVetClientesMatrix)
                                {
                                    cout<<"Erro, tamanho do vetor MaxTamVetClientesMatrix[x] eh insuficientente\n";
                                    exit(-1);
                                }

                                auto ptrVetMat = matrixClienteBest[candidato->cliente];
                                auto ptrVetCliente = vetorClienteAux;

                                for (int i = 0; i < (*veiculo)->listaClientes.size() + 1; ++i)
                                {
                                    *ptrVetMat = *ptrVetCliente;

                                    ++ptrVetMat;
                                    ++ptrVetCliente;
                                }

                                if(heuristica.get<0>() == Heuristica_2)
                                {
                                    /*if(vetCandInteracoes[candidato->cliente].valido)
                                        cout<<"Linha "<<__LINE__<<" Cliente "<<candidato->cliente<<" com sulucao da interacao anterior\n\n";*/

                                    vetCandInteracoes[candidato->cliente].valido = true;
                                    vetCandInteracoes[candidato->cliente].indiceVeiculo =  (*veiculo)->id;
                                    vetCandInteracoes[candidato->cliente].combustivel = melhorCombustivel;
                                    vetCandInteracoes[candidato->cliente].poluicao = melhorPoluicao;
                                    vetCandInteracoes[candidato->cliente].distanciaRotaCompleta = bestDistanciaRotaCompleta;
                                    vetCandInteracoes[candidato->cliente].tam = (*veiculo)->listaClientes.size() + 1;
                                    vetCandInteracoes[candidato->cliente].it = melhorPosicao;
                                    vetCandInteracoes[candidato->cliente].veiculo = (*veiculo);

                                }



                            }

                        }
                    }

                    ++clienteIt;

                    if ((*clienteIt)->cliente == 0)
                        break;


                    posicaoVetor += 1;
                    vetorClienteAux[posicaoVetor] = **(clienteIt);

                    //Calcula poluicao e combustivel entre --clienteIt --> clienteIt
/*                    if ((*clienteIt)->cliente != 0)
                    {

                        clienteItAux = clienteIt;
                        --clienteItAux;

                        //Calcula folga:

                        double folga =  (instancia->vetorClientes[(*clienteIt)->cliente].fimJanela - instancia->vetorClientes[(*clienteIt)->cliente].tempoServico) - (*clienteIt)->tempoChegada ;

                        if(folga < folgaRotaAux)
                        {
                            folgaRotaAux = folga;
                        }

                        //Poluicao da rota não muda
                        poluicaoParcial += (*clienteIt)->poluicaoRota;

                        //Poluicao das cargas muda
                        polAux = VerificaSolucao::poluicaoCarga(instancia, (*veiculo)->tipo, peso, instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);
                        poluicaoParcial += polAux;

                        combAux = VerificaSolucao::combustivelCarga(instancia, (*veiculo)->tipo, peso, instancia->matrizDistancias[(*clienteItAux)->cliente][(*clienteIt)->cliente]);
                        combustivelParcial += combAux;

                        vetorClienteAux[posicaoVetor] = **(clienteIt);
                        vetorClienteAux[posicaoVetor].combustivel = (*clienteIt)->combustivelRota + combAux;
                        vetorClienteAux[posicaoVetor].poluicao = (*clienteIt)->poluicaoRota + polAux;

                        vetorClienteAux[posicaoVetor].combustivelRota = (*clienteIt)->combustivelRota;
                        vetorClienteAux[posicaoVetor].poluicaoRota = (*clienteIt)->poluicaoRota;


                        combustivelParcial += (*clienteIt)->combustivelRota;

                    }*/
/*                    if ((clienteIt) != (*veiculo)->listaClientes.end())
                        peso -= instancia->vetorClientes[(*clienteIt)->cliente].demanda;*/



                }

                if(somentePercorreUmVeiculo && vetCandInteracoes[candidato->cliente].valido)
                    break;

            }

            /* *********************************************************************************
             * Se encontrou uma solução, incluir candidato no veículo de posicao melhorVeiculo,
             * Caso não encontrou uma solução, criar um novo veículo com o candidato.
             *
            ***********************************************************************************/

            if (melhorPoluicao == HUGE_VALF)
            {

                /* *********************************************************************************************************************
                 *
                 * Não conseguiu inserir o cliente em nenhuma posição. Cria um novo veiculo.
                 *corrigir horário de saida do deposito, tempo de espera. Solucao passa a ser inviavel, insere um veiculo ficticil.
                 *
                 ***********************************************************************************************************************/


                Solucao::Veiculo *veiculo;

                if (!solucao->veiculoFicticil)
                {
                    solucao->veiculoFicticil = true;
                    veiculo = new Solucao::Veiculo(2, -1);
                    solucao->vetorVeiculos.push_back(veiculo);

                } else
                {
                    veiculo = solucao->vetorVeiculos[instancia->numVeiculos];
                }

                auto iterador = veiculo->listaClientes.begin();
                ++iterador;

                veiculo->listaClientes.insert(iterador, candidato);


                candidato = new Solucao::ClienteRota;

                const int valPenalizacao = instancia->penalizacao.at(instancia->numClientes - 1);

                solucao->poluicaoPenalidades += valPenalizacao;




                iteratorLisCand = listaCandidatos.erase(iteratorLisCand);                 //Apaga escolhido da lista


                incrementarIt = false;


            }else
            {

                 //inserir a melhor posicao do cliente na lista de candidatos.

                if(ultimo==NULL)
                    ultimo = &vetorCandidatos[0];
                else
                    ultimo++;

                ultimo->distanciaDeposito = instancia->distanciaDeposito[candidato->cliente];
                ultimo->incrementoPoluicao = melhorPoluicao - (*melhorVeiculo).poluicao;
                ultimo->veiculo = melhorVeiculo;
                ultimo->candidato = candidato;
                ultimo->posicao = melhorPosicao;
                ultimo->folgaRota = folgaRota;
                ultimo->distanciaRotaCompleta = bestDistanciaRotaCompleta;
                ultimo->combustivel = melhorCombustivel;
                ultimo->poluicao = melhorPoluicao;
                ultimo->indiceVeiculo =  melhorVeiculo->id;

                candidato = new Solucao::ClienteRota;


            }

            if(incrementarIt)
                ++iteratorLisCand;

            auto vetAux = ultimo;
            vetAux++;


        }

        if(listaCandidatos.size() != 0)
        {
            //Ordenar vetCandidatos
            auto vetAux = ultimo;
            vetAux++;

            unsigned long tam = vetAux - vetorCandidatos;




            int tamLista1Crit = tam * alfa + 1;

            auto ptrEscolhido = vetorCandidatos;
            int tamLista2Crit, tamLista3Crit;


            /*  0: compCandidatoFolga, compCandidatoDist, compCandidato
             *  1: compCandidatoFolga, compCandidato, compCandidatoDist
             *  2: compCandidatoDist, compCandidatoFolga, compCandidato
             */
            int index;

            if(heuristica.get<1>() == Heuristica_Cliente_0)// Tempo
            {

                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoFolga);

                index = rand_u32() % tamLista1Crit;
                ptrEscolhido = &vetorCandidatos[index];

            }
            else if(heuristica.get<1>() == Heuristica_Cliente_1)// poluicao
            {

                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidato);

                index = rand_u32() % tamLista1Crit;
                ptrEscolhido = &vetorCandidatos[index];

            }
            else if(heuristica.get<1>() == Heuristica_Cliente_2)//Distancia deposito
            {

                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoDist);

                index = rand_u32() % tamLista1Crit;
                ptrEscolhido = &vetorCandidatos[index];

            }
            else if(heuristica.get<1>() == Heuristica_Cliente_3) // Distancia
            {
                std::qsort(vetorCandidatos, tam, sizeof(Candidato), compCandidatoDistanciaRota);

                index = rand_u32() % tamLista1Crit;
                ptrEscolhido = &vetorCandidatos[index];
            }


            listaCandidatos.remove(instancia->vetorClientes[ptrEscolhido->candidato->cliente]);

            double polAntes = ptrEscolhido->veiculo->poluicao;

            //Insere candidato escolhido
            //insereCandidato(ptrEscolhido, instancia, vetorClienteAux, solucao);

            auto veiculoAux = ptrEscolhido->veiculo;

            auto clienteAux = new Solucao::ClienteRota;

            veiculoAux->listaClientes.push_back(clienteAux);
            clienteAux = matrixClienteBest[ptrEscolhido->candidato->cliente];


            for(auto cliente : veiculoAux->listaClientes)
            {

                *cliente = *clienteAux;

                ++clienteAux;
            }



            --clienteAux;


            if(clienteAux->cliente != 0)
            {
                cout<<"Linha "<<__LINE__<<"\nErro fim do veiculo nao e 0\n";

                exit(-1);
            }

            veiculoAux->carga += instancia->vetorClientes[ptrEscolhido->candidato->cliente].demanda;
            veiculoAux->poluicao = ptrEscolhido->poluicao;
            veiculoAux->combustivel = ptrEscolhido->combustivel;
            ultimoVeiculoAtualizado = ptrEscolhido->indiceVeiculo;

            solucao->poluicao -= polAntes;
            solucao->poluicao += ptrEscolhido->veiculo->poluicao;

            for (auto ptr = vetorCandidatos; ptr != &vetorCandidatos[tam]; ptr++)
            {
                if (ptr->candidato)
                {
                    delete ptr->candidato;
                    ptr->candidato = NULL;
                }

            }
            



        }



        ultimo = NULL;


    }

    for(auto veiculo : solucao->vetorVeiculos)
    {
        if(!veiculo->listaClientes.empty())
        {
            auto cliente = (*veiculo->listaClientes.begin());
            cliente->rotaMip = false;
        }
    }

    delete candidato;
    return solucao;

}
int Construtivo::compCandidato(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->incrementoPoluicao > ((const Candidato *)cand2)->incrementoPoluicao;
}

int Construtivo::compCandidatoFolga(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->folgaRota < ((const Candidato *)cand2)->folgaRota;
}

int Construtivo::compCandidatoDist(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->distanciaDeposito < ((const Candidato *)cand2)->distanciaDeposito;
}

int Construtivo::compCandidatoDistanciaRota(const void* cand1, const void* cand2)
{
    return ((const Candidato *)cand1)->distanciaRotaCompleta > ((const Candidato *)cand2)->distanciaRotaCompleta;
}

void Construtivo::insereCandidato(Candidato *candidato, const Instancia::Instancia *instancia, Solucao::ClienteRota *vetCliente, Solucao::Solucao *solucao, Movimentos_Paradas::TempoCriaRota *tempoCriaRota)
{

    double poluicao = 0.0;
    double combustivel = 0.0;
    const int peso = candidato->veiculo->carga + instancia->vetorClientes[candidato->candidato->cliente].demanda;


    auto it = candidato->posicao;
    it++;

    //Insere candidato
    (*candidato->veiculo).listaClientes.insert(it, candidato->candidato);
    candidato->candidato = NULL;


    int posicaoVetor = 0;


    for(auto cliente:candidato->veiculo->listaClientes)
    {
        vetCliente[posicaoVetor].cliente = (*cliente).cliente;
        ++posicaoVetor;
    }

    if(!Movimentos_Paradas::criaRota(instancia, vetCliente, posicaoVetor, peso, candidato->veiculo->tipo, &combustivel,
                                     &poluicao, NULL, tempoCriaRota, nullptr, nullptr, NULL))
    {
        cout<<"Erro. func Construtivo::insereCandidato.\nRota deveria ser viavel\n";
        exit(-1);
    }

    posicaoVetor = 0;

    for(auto cliente:candidato->veiculo->listaClientes)
    {
        (*cliente) = vetCliente[posicaoVetor];
        ++posicaoVetor;
    }

    candidato->veiculo->poluicao = poluicao;
    candidato->veiculo->combustivel = combustivel;
    candidato->veiculo->carga = peso;

    //bool verificacao = VerificaSolucao::verificaVeiculo(candidato->veiculo, instancia);

    //if(!verificacao)
    //    cout<<"LINHA "<<__LINE__<<" VEICULO ERADO!!!!!\n\n";


}


/* **********************************************************************************************************************************************
 *
 * Realiza a viagem de cliente1 para cliente2, considerando que cliente1 já esta inserido na solução.
 * Verifica a viabilidade (janela de tempo, tempo de espera maximo). insere informações da rota em cliente2.
 *
 * ***********************************************************************************************************************************************
 */
bool Construtivo::determinaHorario(Solucao::ClienteRota *cliente1, Solucao::ClienteRota *cliente2,
                                   const Instancia::Instancia *const instancia, const int peso, const int tipoVeiculo,
                                   string *erro, double *tempoParaJanela)
{

    if(tempoParaJanela)
        *tempoParaJanela = 0.0;

    long double distancia = instancia->matrizDistancias[cliente1->cliente][cliente2->cliente];
    const long double distanciaTotal = distancia;

    if(cliente1->cliente == 0)
    {
        cliente1->distanciaAteCliente = 0.0;
        cliente2->distanciaAteCliente = distanciaTotal;
    }
    else
        cliente2->distanciaAteCliente = cliente1->distanciaAteCliente + distanciaTotal;


    long double horaPartida = cliente1->tempoSaida;
    long double velocidade, tempoRestantePeriodo, horario, horaChegada, poluicaoAux = 0.0, poluicao = 0.0, combustivel = 0.0, combustivelAux = 0.0;
    long double tempoEspera;
    int periodoSaida = instancia->retornaPeriodo(horaPartida);//Periodo[0, ..., 4]
    int periodoChegada;
    static bool percorrePeriodo[5];

    percorrePeriodo[0] = percorrePeriodo[1] = percorrePeriodo[2] = percorrePeriodo[3] = percorrePeriodo[4] = false;

    cliente2->tempoPorPeriodo[0] = cliente2->tempoPorPeriodo[1] = cliente2->tempoPorPeriodo[2] = cliente2->tempoPorPeriodo[3] =
            cliente2->tempoPorPeriodo[4] = 0.0;

    cliente2->distanciaPorPeriodo[0] = cliente2->distanciaPorPeriodo[1] = cliente2->distanciaPorPeriodo[2] = cliente2->distanciaPorPeriodo[3] =
    cliente2->distanciaPorPeriodo[4] = 0.0;

    velocidade = instancia->matrizVelocidade[cliente1->cliente][cliente2->cliente][periodoSaida];




    if((distancia == 0.0) || (velocidade == 0.0))
    {
        if(erro)
        {


            if(distancia == 0)
                *erro += "distancia == 0\n";
            else
                *erro += "velocidade == 0\n";

            cout<<cliente1->cliente<<" "<<cliente2->cliente<<'\n';
        }


        return false;

    }

    if(periodoSaida < 0)
    {
        cout<<"Antes do do\nErro periodo negativo\n";
        cout<<"Hora de partida: "<<horaPartida<<'\n';
        cout<<"Arquivo: Contrutivo.cpp linha: "<<__LINE__<<'\n';
        exit(-1);
    }


    do
    {

        if(periodoSaida >= 5)
        {
            if(erro)
                *erro += "periodo > 5\n";

            if(tempoParaJanela)
                *tempoParaJanela = 0.25;

            return false;

        }

        if(periodoSaida < 0)
        {
            cout<<"Erro periodo negativo\n";
            cout<<"Hora de partida: "<<horaPartida<<'\n';
            cout<<"Arquivo: Contrutivo.cpp linha: "<<__LINE__<<'\n';
            exit(-1);
        }

        velocidade = instancia->matrizVelocidade[cliente1->cliente][cliente2->cliente][periodoSaida];//velocidade -> km/h
        horario = horaPartida + (distancia / velocidade); //Horario de chegada considerando somente uma velocidade. Horario em horas
        periodoChegada = (instancia->retornaPeriodo(horario));
        percorrePeriodo[periodoSaida] = true;

        tempoRestantePeriodo = instancia->vetorPeriodos[periodoSaida].fim - horaPartida;
        long double distanciatemp = distancia - tempoRestantePeriodo * velocidade;

        if((periodoChegada!=periodoSaida) && (distanciatemp == 0.0))
            periodoChegada = periodoSaida;


        //cout<<cliente1->cliente<<" "<<cliente2->cliente<<": "<<periodoSaida<<" "<<periodoChegada<<'\n';

        //Periodo de chegada diferente da saida, não é possível percorrer a distancia em somente um periodo.
        if ((periodoChegada != periodoSaida))
        {

            // Percorreu todo o periodoSaida e não chegou ao destino.


            distancia = distanciatemp;
            horaPartida = instancia->vetorPeriodos[periodoSaida + 1].inicio;

            poluicaoAux += VerificaSolucao::poluicaoRota(instancia, tipoVeiculo, tempoRestantePeriodo * velocidade,cliente1->cliente, cliente2->cliente, periodoSaida );
            combustivelAux += VerificaSolucao::combustivelRota(instancia, tipoVeiculo, tempoRestantePeriodo * velocidade,cliente1->cliente, cliente2->cliente, periodoSaida );

            cliente2->tempoPorPeriodo[periodoSaida] = tempoRestantePeriodo;
            cliente2->distanciaPorPeriodo[periodoSaida] = tempoRestantePeriodo * velocidade;

            periodoSaida += 1;

        }
        //horario é do mesmo periodo de periodoSaida, então o veículo chegou ao destino
        else
        {


            poluicaoAux += VerificaSolucao::poluicaoRota(instancia, tipoVeiculo, distancia,cliente1->cliente, cliente2->cliente, periodoSaida );
            combustivelAux += VerificaSolucao::combustivelRota(instancia, tipoVeiculo, distancia,cliente1->cliente, cliente2->cliente, periodoSaida );
            cliente2->poluicaoRota = poluicaoAux;
            cliente2->combustivelRota = combustivelAux;


            poluicaoAux += VerificaSolucao::poluicaoCarga(instancia, tipoVeiculo, peso,  instancia->matrizDistancias[cliente1->cliente][cliente2->cliente]);
            combustivelAux += VerificaSolucao::combustivelCarga(instancia, tipoVeiculo, peso,  instancia->matrizDistancias[cliente1->cliente][cliente2->cliente]);

            cliente2->poluicao = poluicaoAux;
            cliente2->combustivel = combustivelAux;
            cliente2->tempoPorPeriodo[periodoSaida] = distancia/velocidade;
            cliente2->distanciaPorPeriodo[periodoSaida] = distancia;


            distancia = 0;
            horaChegada = horario;

        }


    } while (distancia != 0.0);

        cliente2->tempoChegada = horaChegada;

        for(int i = 0; i < 5; ++i)
            cliente2->percorrePeriodo[i] =  percorrePeriodo[i];

        //Chegou após o inicio da janela
        if ((cliente2)->tempoChegada >= instancia->vetorClientes[(cliente2)->cliente].inicioJanela)
        {

            // Verificar se tempoChegada + tempo de atendimento <= fim janela e tempo saida é igua a igual a tempoChegada + tempo de atendimento

            cliente2->tempoSaida = (cliente2)->tempoChegada + instancia->vetorClientes[(cliente2)->cliente].tempoServico;

            if ((cliente2->tempoChegada <= instancia->vetorClientes[(cliente2)->cliente].fimJanela) || ((cliente2->tempoChegada - instancia->vetorClientes[(cliente2)->cliente].fimJanela) <= 5e-3))
            {


                return true;

            }
            else
            {
                if(tempoParaJanela)
                    *tempoParaJanela = (cliente2->tempoChegada - instancia->vetorClientes[(cliente2)->cliente].fimJanela) + 0.25;

                if(erro)
                    *erro += "tempo saida maior que o final da janela de tempo\n";
                return false;
            }


        } else //Chegou antes do inicio da janela
        {

            cliente2->tempoSaida = instancia->vetorClientes[(cliente2)->cliente].inicioJanela + instancia->vetorClientes[(cliente2)->cliente].tempoServico;

            return true;

        }


}

/* *********************************************************************************************************************************************************************
 *
 * Tenta inserir candidato após iteradorCliente. Ajusta e verifica a viabilidade dos próximos clientes
 * combustivel e poluicao do veiculo até iteratorCliente (inclusive).
 * 1° parametro de retorno: viabilidade da solução.
 * 2° parametro de retorno: tamanho do vetor vetorClientes.
 * 3° parametro de retorno: poluicao.
 * 4° parametro de retorno: combustível.
 *
 ***********************************************************************************************************************************************************************/
TupleBID Construtivo::viabilidadeInserirCandidato(Solucao::ClienteRota *vetorClientes, ItClienteRota iteratorCliente,
                                                  const Instancia::Instancia *const instancia,
                                                  Solucao::ClienteRota *candidato, double combustivelParcial,
                                                  double poluicaoParcial, Solucao::Veiculo *veiculo, int peso,
                                                  int posicao, double *folga,
                                                  Movimentos_Paradas::TempoCriaRota *tempoCriaRota,
                                                  double *vetLimiteTempo, Solucao::ClienteRota *vetClienteRotaAux)
{

    //inicialisa vetor de clientes

    vetorClientes[posicao + 1] = *candidato;

    int i;

    double poluicao = 0.0;
    double combustivel = 0.0;


    ++iteratorCliente;



    //Percorre o resto dos clientes
    for(i = posicao+1; (iteratorCliente) !=  veiculo->listaClientes.end(); ++i)
    {

        //Adiciona o próximo cliente da lista na nova solução
        vetorClientes[i+1] = (**iteratorCliente);
        ++iteratorCliente;

    }


    bool resultado = Movimentos_Paradas::criaRota(instancia, vetorClientes, i + 1, peso, veiculo->tipo, &combustivel,
                                                  &poluicao, folga, tempoCriaRota, vetLimiteTempo, vetClienteRotaAux,
                                                  NULL);



    if(resultado)
    {
        return {true, i, poluicao, combustivel};
    }
    else
        return {false, -1, -1, -1};



}
