
/*
 ============================================================================
 Name        : ArrayPartition.c
 Author      : CiccioTecchio
 Version     :
 Copyright   : copyright CiccioTecchio
 Description : Partitioned sum of array in C
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"


void checkArgs(int arg){

	if(arg!=3){
		printf("Insert parameters <size array> <seed>\n");
		exit(0);
	}
}

void generateArray(int array[],int size,int seed){
	srand(seed);
	for(int i=0; i<size; i++){
	 	array[i]= rand()%50;
							}
}

void toCopy(int array[],int size){
printf("\n%d",array[0]);
for(int i=1;i<size;i++){
			printf("+%d",array[i]);
		}
printf("\n");
}

int seqSum(int array[],int size){
	int sum=0;
	for(int i=0;i<size;i++){
		sum+=array[i];
	}
	return sum;
}

int main(int argc, char* argv[]){
	int  my_rank; /* rank of process */
	int  p;       /* number of processes */
	int tag=0;    /* tag for messages */
	MPI_Status status ;   /* return status for receive */

	/* start up MPI */

	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);


	int size= atoi(argv[1]);
	int array[size];

		//MASTER
	if (my_rank ==0){
		checkArgs(argc);
		generateArray(array,size,atoi(argv[2]));
		toCopy(array,size);
		printf("\nSEQUENTIAL SUM= %d\n\n",seqSum(array,size));

		int reminder = size % (p-1);
		int dimPart = size / (p-1); //at least the size of partition
		int init=0;
		int subP=0;
		int sum=0;
		int k;
		for(k=1;k<p;k++){
			if(k <= reminder){
				int remaining= dimPart+1;
				MPI_Send(&remaining,sizeof(MPI_INT),MPI_INT,k,tag,MPI_COMM_WORLD);
				printf("Send to %d, remaining= %d, start form: %d, finish to: %d\n",k,remaining,init,init+remaining);
				MPI_Send(&array[init],sizeof(MPI_INT)*remaining,MPI_INT,k,tag,MPI_COMM_WORLD);
				init+=remaining;
			}else{
					MPI_Send(&dimPart,sizeof(MPI_INT),MPI_INT,k,tag,MPI_COMM_WORLD);
					printf("Send to %d, dimPart= %d, start form: %d, finish to: %d\n",k,dimPart,init,init+dimPart);
					MPI_Send(&array[init],sizeof(MPI_INT)*dimPart,MPI_INT,k,tag,MPI_COMM_WORLD);
					init+=dimPart;
			}
		}

		for(k=1;k<p;k++){
			MPI_Recv(&subP,sizeof(MPI_INT),MPI_INT,k,tag,MPI_COMM_WORLD,&status); 
			sum+=subP;
			printf("\nRiceved %d from slave %d\n",sum,k);
		}
		printf("\nPARALLEL SUM= %d\n",sum);

	}
	//SLAVE
	else{
		int part;

		MPI_Recv(&part,sizeof(MPI_INT),MPI_INT,0,tag,MPI_COMM_WORLD,&status);
		MPI_Recv(&array[0],sizeof(MPI_INT)*part,MPI_INT,0,tag,MPI_COMM_WORLD,&status); 
		
		printf("\nSlave %d riceive size %d with elements:",my_rank, part);
		toCopy(array,part);
		int sum=seqSum(array,part);
		printf("Sum= %d\n\n",sum);
		MPI_Send(&sum,sizeof(MPI_INT),MPI_INT,0,tag,MPI_COMM_WORLD);	
	}
	/* shut down MPI */
	MPI_Finalize();
	return 0;
}