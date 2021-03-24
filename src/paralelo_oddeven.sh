#!/bin/bash

#SBATCH --partition=test
#SBATCH --job-name=paralelo
#SBATCH --output=paralelo_oddeven.out
#SBATCH --error=paralelo_oddeven.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --time=0-0:30
#SBATCH --hint=compute_bound
#SBATCH --exclusive

#Carrega os módulos do sistema
rm runtime_oddeven_paralelo.txt
module load compilers/gnu/7.3
module load compilers/intel/2018.2.199
eval $loadIntelLibs

#Compila o código
mpicc paralelo_oddeven.c -o paralelo_oddeven

tentativas=15 #Quantas vezes o código será executado

for cores in 2 4 8 16 32 #número de cores utilizados
do
	for size in 16000 32000 64000 128000 #tamanho do problema			
	do   									
		echo -e "\n$cores\t$size\t\t\c" >> "paralelo_oddeven_time.txt" 
			
		for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"				
		do
			echo -e `srun -n $cores ./paralelo_oddeven $size`	#MPI usando $cores como número de processos e $size como tamanho						
		done			
	done
done	
		
exit 
