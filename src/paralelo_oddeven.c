/*  Odd Even Transposition Sort - Código Paralelo
	Leandro S Ferreira Jr */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

void fill(int* vetor, int n);
void oddevenSortLocal(int* vetor, int n);
void oddevenSortGlobal(int n, int* vetor, MPI_Comm comm);
void sortPthreads(int n, int* local_vetor, int sendrank, int recvrank, MPI_Comm comm);
void merge(int* vetorA, int sizeA, int* vetorB, int sizeB, int* mergedVetor);
void print(int* vetor, int n);

int main(int argc, char* argv[]) { 
        
    int n = atoi(argv[1]); 						// Atribui ao int n o tamanho do vetor passado por parâmetro
    int* vetor = malloc(n*sizeof(int)); 		// Aloca o vetor de tamanho n
    srand(0);									//Seed 0 padroniza os vetores gerados em cada execução do programa
    struct timeval start, stop; 				// Struct para medir tempo
    int rank;			
    
    MPI_Init(NULL, NULL);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        fill(vetor, n);								//Preenche o vetor 
        gettimeofday(&start, NULL);                 // Tempo inicial
    }
    
    oddevenSortGlobal(n, vetor, MPI_COMM_WORLD);   //Realiza todo o ordenamento
    
    
    if(rank==0) {
        gettimeofday(&stop, NULL);                  // Tempo final
        print(vetor, n);                       // Imprime no .out (para averiguação)

        char arquivo[] = "paralelo_oddeven_time.txt";
        FILE *fp;     
        fp = fopen(arquivo, "a");                   //Abre o arquivo txt
       
        if (fp == NULL) exit(1);
        else {
            fprintf(fp, "%.3f\t", (stop.tv_sec + stop.tv_usec*1e-6)-(start.tv_sec + start.tv_usec*1e-6));   //Salva o tempo de execução na tabela
            fclose(fp);
        }
        
    }  
    
    MPI_Finalize();
    free(vetor);
    return (0);
}
void fill(int* vetor, int n) {             //Função para preencher o vetor
	srand(0);
    for (int i=0; i<n; i++) {
        vetor[i] = rand()%10000;			//Gera números entre 0 e 10.000
    }
}
void oddevenSortLocal(int* vetor, int n) {			// Função Odd-Even 
    bool sorted = false;             			// Indicador se está ordenado
    int aux; 			            			// Auxiliar para troca
  
    while (!sorted) {
    	sorted = true;              			// Controle de saída do laço
    	
        for (int i=1; i<=n-2; i=i+2) {			// Fase Odd 
            if (vetor[i] > vetor[i+1]) { 
                aux = vetor[i];
                vetor[i] = vetor[i+1];
                vetor[i+1] = aux;
                sorted = false; 
            } 
        } 
        for (int i=0; i<=n-2; i=i+2) {    		// Fase Even	
        	if (vetor[i] > vetor[i+1]) {
            	aux = vetor[i];
                vetor[i] = vetor[i+1];
                vetor[i+1] = aux;
                sorted = false;
            } 
        }
    }
} 
void oddevenSortGlobal(int n, int* vetor, MPI_Comm comm) {				//Realiza todo o ordenamento	

	int rank, pthreads,fase;
	int* local_vetor;

	MPI_Comm_rank(comm, &rank); 
    MPI_Comm_size(comm, &pthreads);
    
    local_vetor = malloc(n/pthreads*sizeof(int));       //Aloca o tamanho do vetor que fica para cada thread
    
	MPI_Scatter(vetor, n/pthreads, MPI_INT, local_vetor, n/pthreads, MPI_INT, 0, comm);		//Direciona as partes do vetor às pthreads 
	oddevenSortLocal(local_vetor, n/pthreads);                     		 // Ordena a parte do vetor inicial destinada à cada pthread
      
	for (fase = 1; fase <= pthreads; fase++) {      					//Generalizado para as fases even-odd respectivamente
    	if ( (fase + rank) % 2 == 0) {  								//Garante a mesma natureza ao Rank e a Pthread (Even-Odd)
            if (rank < pthreads-1) {
    		sortPthreads(n/pthreads, local_vetor, rank, rank +1, comm);  //sorteia com a pthread seguinte
    		} 
    	} 
    	else if (rank > 0) {                       						//Garante a natureza diferente entre o Rank e a Pthread (Even-Odd)
    		sortPthreads(n/pthreads, local_vetor, rank-1, rank, comm);  //sorteia com a pthread anterior
 		}      
    }  
    
    MPI_Gather(local_vetor, n/pthreads, MPI_INT, vetor, n/pthreads, MPI_INT, 0, comm);    //Une os vetores locais novamente à um vetor principal na pthread 0   	
}
void sortPthreads(int local_n, int* local_vetor, int sendrank, int recvrank, MPI_Comm comm) {		//função para comunicar e sortear os vetores entre duas pthreads	
	
	int rank;
	int aux[local_n];						//vetor auxiliar para receber o vetor da pthread anterior(sendrank)
	int mergedVetor[2*local_n];				//vetor fundido
	const int sendVetor = 1;				//comunicação entre send e recv
    const int givebackVetor = 2;
	
	MPI_Comm_rank(comm, &rank);

	if (rank == sendrank) {																							//pthread envia e recebe de volta o vetor																	
    	MPI_Send(local_vetor, local_n, MPI_INT, recvrank, sendVetor, MPI_COMM_WORLD);
    	MPI_Recv(local_vetor, local_n, MPI_INT, recvrank, givebackVetor, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
    } else {        																								//pthread+1 
        MPI_Recv(aux, local_n, MPI_INT, sendrank, sendVetor, MPI_COMM_WORLD, MPI_STATUS_IGNORE);					//recebe o vetor da pthread
        merge(local_vetor, local_n, aux, local_n, mergedVetor);														//funde os vetores
        oddevenSortLocal(mergedVetor, 2*local_n);																	//ordena o vetor fundido
        	
        int theirstart = 0, mystart = local_n;																		//Endereço correspondente à cada pthread para o vetor fundido
        if (sendrank > rank) {																						//para garantir que pthread+1 devolva a primeira parte para a pthread
            theirstart = local_n;
            mystart = 0;
        }
    
        MPI_Send(&(mergedVetor[theirstart]), local_n, MPI_INT, sendrank, givebackVetor, MPI_COMM_WORLD);			//pthread+1 devolve o vetor para pthread 
    
        for (int i=mystart; i<mystart+local_n; i++) { 																//pthread+1 grava no vetor seu local a segunda parte do vetor fundido
            local_vetor[i-mystart] = mergedVetor[i];
        }  
    }     
}
void merge(int* vetorA, int sizeA, int* vetorB, int sizeB, int* mergedVetor) {			//função para fundir dois vetores em um terceiro	
	
	for(int i=0; i<sizeA; i++) {			// recebendo a primeira parte
		mergedVetor[i] = vetorA[i];
	}
	
	for(int i=0, j=sizeA; i<sizeB; i++, j++) { 			//recebendo a segunda parte
		mergedVetor[j] = vetorB[i];
	}	
}
void print(int* vetor, int n) { 		//Função para imprimir o vetor no .out	
	
	printf("\n ********************************************************** \n");
	
	for (int i=0; i<n; i++) {
		printf("%d  ",vetor[i]);
	}	
}
