/*  Odd Even Transposition Sort - Código Serial 
	Leandro S Ferreira Jr */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

void fill(int* vetor, int n);
void oddevenSortLocal(int* vetor, int n);
void print(int* vetor, int n);

int main(int argc, char* argv[]) { 
    int n = atoi(argv[1]); 						// Atribui ao int n o tamanho do vetor passado por parâmetro
    int* vetor = malloc(n*sizeof(int)); 		// Aloca o vetor de tamanho n
    struct timeval start, stop; 				// Struct para medir tempo
    
    fill(vetor, n);
    gettimeofday(&start, NULL);                 // Tempo inicial
    oddevenSortLocal(vetor, n);                      // Ordena o vetor
    gettimeofday(&stop, NULL);                  // Tempo final
    print(vetor, n);                       // Imprime no .out (para averiguação)

    char arquivo[] = "serial_oddeven_time.txt";
    FILE *fp;     
    fp = fopen(arquivo, "a");                   //Abre o arquivo txt
       
    if (fp == NULL) {
    	exit(1);
    } else {
        fprintf(fp, "%.3f\t", (stop.tv_sec + stop.tv_usec*1e-6)-(start.tv_sec + start.tv_usec*1e-6));   //Salva o tempo de execução na tabela
        fclose(fp);
    }

    free (vetor);
    return (0);
}
void fill(int* vetor, int n) {             //Função para preencher o vetor
	srand(0);
    for (int i=0; i<n; i++) {
        vetor[i] = rand()%10000;
    }
}

void oddevenSortLocal(int* vetor, int n) {			// Função Odd-Even 
    bool sorted = false;             			// Indicador se está ordenado
    int aux; 			            			// Auxiliar para troca
    while (!sorted) {
    	sorted = true;              			// Controle de saída do laço
    	
        for (int i=1; i<=n-2; i=i+2) {			// Odd Phase 
            if (vetor[i] > vetor[i+1]) { 
                aux = vetor[i];
                vetor[i] = vetor[i+1];
                vetor[i+1] = aux;
                sorted = false; 
            } 
        } 
        for (int i=0; i<=n-2; i=i+2) {    		// Even Phase	
        	if (vetor[i] > vetor[i+1]) {
            	aux = vetor[i];
                vetor[i] = vetor[i+1];
                vetor[i+1] = aux;
                sorted = false;
            } 
        }
    }
} 

void print(int* vetor, int n) {            //Função para imprimir o vetor no .out		
	printf("\n ********************************************************** \n");
	
	for (int i=0; i<n; i++) {
		printf("%d  ",vetor[i]);
	}
}