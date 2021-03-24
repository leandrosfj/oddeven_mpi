#!/bin/bash

#SBATCH --partition=test
#SBATCH --job-name=serial
#SBATCH --output=serial_oddeven.out
#SBATCH --error=serial_oddeven.err
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --time=0-0:30
#SBATCH --hint=compute_bound
#SBATCH --exclusive

#Carrega os módulos do sistema
rm runtime_oddeven_serial.txt
module load compilers/gnu/7.3
module load compilers/intel/2018.2.199
eval $loadIntelLibs

#Compila o código
gcc serial_oddeven.c -o serial_oddeven

tentativas=15 #Quantas vezes o código será executado
			
for size in 16000 32000 64000 128000 #tamanho do problema			
do   									
	echo -e "\n$size\t\t\c" >> "serial_oddeven_time.txt" 
		
	for tentativa in $(seq $tentativas) #Cria uma vetor de 1 a "tentativas"				
	do
		echo -e `./serial_oddeven $size`						
	done					
done
	
exit 


